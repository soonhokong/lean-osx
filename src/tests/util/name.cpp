/*
Copyright (c) 2013 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#include <cstring>
#include <sstream>
#include "util/test.h"
#include "util/name.h"
using namespace lean;

static void tst1() {
    name n("foo");
    lean_assert(n == name("foo"));
    lean_assert(name(n, 1) != name(n, 2));
    lean_assert(name(n, 1) == name(n, 1));
    lean_assert(name(name(n, 1), 2) != name(name(n, 1), 1));
    lean_assert(name(name(n, 1), 1) == name(name(n, 1), 1));
    lean_assert(name(name(n, 2), 1) != name(name(n, 1), 1));
    lean_assert(name(name(n, "bla"), 1) == name(name(n, "bla"), 1));
    lean_assert(name(name(n, "foo"), 1) != name(name(n, "bla"), 1));
    lean_assert(name(name(name("f"), "bla"), 1) != name(name(n, "bla"), 1));
    lean_assert(n != name());
    lean_assert(name().kind() == name_kind::ANONYMOUS);
    lean_assert(name(name(), "foo") == name("foo"));

    lean_assert(name(n, 1) < name(n, 2));
    std::cout << "cmp(" << name(n, 1) << ", " << name(n, 2) << ") = " << cmp(name(n, 1), name(n, 2)) << "\n";
    lean_assert(!(name(n, 1) >= name(n, 2)));
    lean_assert(name(n, 1) < name(name(n, 1), 1));
    lean_assert(n < name(n, 1));
    lean_assert(name(n, 2) > name(name(n, 1), 1));
    lean_assert(name(name("aa"), 2) < name(name(n, 1), 1));
    lean_assert(name(n, "aaa") < name(n, "xxx"));
    lean_assert(name(n, 1) < name(n, "xxx"));
    lean_assert(name(n, 1) < name(name(n, "xxx"), 1));
    lean_assert(name() < name(name(n, "xxx"), 1));
}

static name mk_big_name(unsigned num) {
    name n("foo");
    for (unsigned i = 0; i < num; i++) {
        n = name(n, "bla");
    }
    return n;
}

static void tst2() {
    name n1 = mk_big_name(2000);
    name n2 = mk_big_name(2000);
    lean_assert(n1.hash() == n2.hash());
    for (unsigned i = 0; i < 10000; i++)
        n1.hash();
    std::cout << "n1.hash(): " << n1.hash() << "\n";
}

static void tst3() {
    name n{"foo", "bla", "tst"};
    std::cout << n << "\n";
    lean_assert(n == name(name(name("foo"), "bla"), "tst"));
}

static void tst4() {
    lean_assert(is_prefix_of(name{"foo", "bla"}, name{"foo", "bla"}));
    lean_assert(is_prefix_of(name{"foo", "bla"}, name{"foo", "bla", "foo"}));
    lean_assert(is_prefix_of(name{"foo"}, name{"foo", "bla", "foo"}));
    lean_assert(!is_prefix_of(name{"foo"}, name{"fo", "bla", "foo"}));
    lean_assert(!is_prefix_of(name{"foo", "bla", "foo"}, name{"foo", "bla"}));
    lean_assert(is_prefix_of(name{"foo", "bla"}, name(name{"foo", "bla"}, 0u)));
}

static void tst5() {
    lean_assert(name().size() > 0);
    lean_assert(name({"foo", "bla", "boing"}).get_prefix() == name({"foo", "bla"}));
    lean_assert(!name({"foo", "bla", "boing"}).is_atomic());
    lean_assert(name({"foo"}).is_atomic());
    lean_assert(strcmp(name({"foo", "bla", "boing"}).get_string(), "boing") == 0);
    lean_assert(name(name("foo"), 1u).get_numeral() == 1u);
    lean_assert(name::anonymous().is_anonymous());
    name n1("foo");
    name n2 = n1;
    lean_assert(n2 == n1);
    std::cout << name::anonymous() << "\n";
    std::cout << name({"foo", "bla", "boing"}).get_prefix() << "\n";
    lean_assert(name("foo").is_string());
    lean_assert(name(name("boo"), "foo").is_string());
    lean_assert(name(name("foo"), 0u).is_numeral());
    lean_assert(name(name("foo"), 0u).get_prefix().is_string());
}

static void tst6() {
    lean_assert(name({"foo", "bla"}).is_safe_ascii());
    lean_assert(!name({"foo", "b\u2200aaa"}).is_safe_ascii());
    lean_assert(!name({"\u2200", "boo"}).is_safe_ascii());
    lean_assert(!name(name(name("baa"), "bla\u2200"), "foo").is_safe_ascii());
}

static void tst7() {
    lean_assert(name("foo") + name("bla") == name({"foo", "bla"}));
    lean_assert(name("foo") + name({"bla", "test"}) == name({"foo", "bla", "test"}));
    lean_assert(name({"foo", "hello"}) + name({"bla", "test"}) == name({"foo", "hello", "bla", "test"}));
    lean_assert(name("foo") + (name("bla") + name({"bla", "test"})) == name(name(name(name("foo"), "bla"), "bla"), "test"));
    lean_assert(name() + name({"bla", "test"}) == name({"bla", "test"}));
    lean_assert(name({"bla", "test"}) + name() == name({"bla", "test"}));
}

static void tst8() {
    name u1 = name::mk_internal_unique_name();
    name u2 = name::mk_internal_unique_name();
    lean_assert(u1 != u2);
    std::cout << u1 << " " << u2 << "\n";
}

int main() {
    tst1();
    tst2();
    tst3();
    tst4();
    tst5();
    tst6();
    tst7();
    tst8();
    return has_violations() ? 1 : 0;
}
