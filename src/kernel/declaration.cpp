/*
Copyright (c) 2014 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#include "kernel/declaration.h"
#include "kernel/environment.h"
#include "kernel/for_each_fn.h"

namespace lean {
int compare(reducibility_hints const & h1, reducibility_hints const & h2) {
    if (h1.m_kind == h2.m_kind) {
        if (h1.m_kind == reducibility_hints::Regular) {
            if (h1.m_height == h2.m_height)
                return 0; /* unfold both */
            else if (h1.m_height > h2.m_height)
                return -1; /* unfold f1 */
            else
                return 1;  /* unfold f2 */
            return h1.m_height > h2.m_height ? -1 : 1;
        } else {
            return 0; /* reduce both */
        }
    } else {
        if (h1.m_kind == reducibility_hints::Opaque) {
            return 1; /* reduce f2 */
        } else if (h2.m_kind == reducibility_hints::Opaque) {
            return -1; /* reduce f1 */
        } else if (h1.m_kind == reducibility_hints::Abbreviation) {
            return -1; /* reduce f1 */
        } else if (h2.m_kind == reducibility_hints::Abbreviation) {
            return 1; /* reduce f2 */
        } else {
            lean_unreachable();
        }
    }
}

struct declaration::cell {
    MK_LEAN_RC();
    name               m_name;
    level_param_names  m_params;
    expr               m_type;
    bool               m_theorem;
    optional<expr>     m_value;        // if none, then declaration is actually a postulate
    reducibility_hints m_hints;
    /* Definitions are trusted by default, and nested macros are expanded when kernel is instantiated with
       trust level 0. When this flag is false, then we do not expand nested macros. We say the
       associated definitions are "untrusted". We use this feature to define tactical-definitions.
       The kernel type checker ensures trusted definitions do not use untrusted ones. */
    bool              m_trusted;
    void dealloc() { delete this; }

    cell(name const & n, level_param_names const & params, expr const & t, bool is_axiom, bool trusted):
        m_rc(1), m_name(n), m_params(params), m_type(t), m_theorem(is_axiom),
        m_hints(reducibility_hints::mk_opaque()), m_trusted(trusted) {}
    cell(name const & n, level_param_names const & params, expr const & t, bool is_thm, expr const & v,
         reducibility_hints const & h, bool trusted):
        m_rc(1), m_name(n), m_params(params), m_type(t), m_theorem(is_thm),
        m_value(v), m_hints(h), m_trusted(trusted) {}
};

static declaration * g_dummy = nullptr;

declaration::declaration():declaration(*g_dummy) {}
declaration::declaration(cell * ptr):m_ptr(ptr) {}
declaration::declaration(declaration const & s):m_ptr(s.m_ptr) { if (m_ptr) m_ptr->inc_ref(); }
declaration::declaration(declaration && s):m_ptr(s.m_ptr) { s.m_ptr = nullptr; }
declaration::~declaration() { if (m_ptr) m_ptr->dec_ref(); }

declaration & declaration::operator=(declaration const & s) { LEAN_COPY_REF(s); }
declaration & declaration::operator=(declaration && s) { LEAN_MOVE_REF(s); }

bool declaration::is_definition() const    { return static_cast<bool>(m_ptr->m_value); }
bool declaration::is_constant_assumption() const { return !is_definition(); }
bool declaration::is_axiom() const         { return is_constant_assumption() && m_ptr->m_theorem; }
bool declaration::is_theorem() const       { return is_definition() && m_ptr->m_theorem; }
bool declaration::is_trusted() const       { return m_ptr->m_trusted; }

name const & declaration::get_name() const { return m_ptr->m_name; }
level_param_names const & declaration::get_univ_params() const { return m_ptr->m_params; }
unsigned declaration::get_num_univ_params() const { return length(get_univ_params()); }
expr const & declaration::get_type() const { return m_ptr->m_type; }

expr const & declaration::get_value() const { lean_assert(is_definition()); return *(m_ptr->m_value); }
reducibility_hints const & declaration::get_hints() const { return m_ptr->m_hints; }

declaration mk_definition(name const & n, level_param_names const & params, expr const & t, expr const & v,
                          reducibility_hints const & h, bool trusted) {
    return declaration(new declaration::cell(n, params, t, false, v, h, trusted));
}
static unsigned get_max_height(environment const & env, expr const & v) {
    unsigned h = 0;
    for_each(v, [&](expr const & e, unsigned) {
            if (is_constant(e)) {
                auto d = env.find(const_name(e));
                if (d && d->get_hints().get_height() > h)
                    h = d->get_hints().get_height();
            }
            return true;
        });
    return h;
}

declaration mk_definition(environment const & env, name const & n, level_param_names const & params, expr const & t,
                          expr const & v, bool use_self_opt, bool trusted) {
    unsigned h = get_max_height(env, v);
    return mk_definition(n, params, t, v, reducibility_hints::mk_regular(h+1, use_self_opt), trusted);
}
declaration mk_theorem(name const & n, level_param_names const & params, expr const & t, expr const & v) {
    return declaration(new declaration::cell(n, params, t, true, v, reducibility_hints::mk_opaque(), true));
}
declaration mk_axiom(name const & n, level_param_names const & params, expr const & t) {
    return declaration(new declaration::cell(n, params, t, true, true));
}
declaration mk_constant_assumption(name const & n, level_param_names const & params, expr const & t, bool trusted) {
    return declaration(new declaration::cell(n, params, t, false, trusted));
}

static bool use_untrusted(environment const & env, expr const & e) {
    bool found = false;
    for_each(e, [&](expr const & e, unsigned) {
            if (found) return false;
            if (is_constant(e)) {
                if (auto d = env.find(const_name(e))) {
                    if (!d->is_trusted()) {
                        found = true;
                        return false;
                    }
                }
            }
            return true;
        });
    return found;
}

declaration mk_definition_inferring_trusted(environment const & env, name const & n, level_param_names const & params,
                                            expr const & t, expr const & v, reducibility_hints const & hints) {
    bool trusted = !use_untrusted(env, t) && !use_untrusted(env, v);
    return mk_definition(n, params, t, v, hints, trusted);
}
declaration mk_definition_inferring_trusted(environment const & env, name const & n, level_param_names const & params,
                                            expr const & t, expr const & v, bool use_self_opt) {
    bool trusted = !use_untrusted(env, t) && !use_untrusted(env, v);
    unsigned h   = get_max_height(env, v);
    return mk_definition(n, params, t, v, reducibility_hints::mk_regular(h+1, use_self_opt), trusted);
}
declaration mk_constant_assumption_inferring_trusted(environment const & env, name const & n,
                                                     level_param_names const & params, expr const & t) {
    return mk_constant_assumption(n, params, t, !use_untrusted(env, t));
}

void initialize_declaration() {
    g_dummy = new declaration(mk_axiom(name(), level_param_names(), expr()));
}

void finalize_declaration() {
    delete g_dummy;
}
}
