/*
Copyright (c) 2014 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#include "frontends/lean/tokens.h"
#include "frontends/lean/parser.h"
#include "frontends/lean/parser_config.h"
#include "frontends/lean/calc.h"
#include "frontends/lean/builtin_cmds.h"
#include "frontends/lean/builtin_exprs.h"
#include "frontends/lean/inductive_cmds.h"
#include "frontends/lean/structure_instance.h"
#include "frontends/lean/parse_table.h"
#include "frontends/lean/token_table.h"
#include "frontends/lean/scanner.h"
#include "frontends/lean/pp.h"
#include "frontends/lean/local_ref_info.h"
#include "frontends/lean/decl_cmds.h"
#include "frontends/lean/nested_declaration.h"
#include "frontends/lean/prenum.h"
#include "frontends/lean/old_attributes.h"
#include "frontends/lean/elaborator.h"
#include "frontends/lean/match_expr.h"
#include "frontends/lean/notation_cmd.h"

namespace lean {
void initialize_frontend_lean_module() {
    initialize_old_attributes();
    initialize_prenum();
    initialize_tokens();
    initialize_token_table();
    initialize_parse_table();
    initialize_builtin_cmds();
    initialize_builtin_exprs();
    initialize_scanner();
    initialize_parser();
    initialize_parser_config();
    initialize_calc();
    initialize_inductive_cmds();
    initialize_structure_instance();
    initialize_pp();
    initialize_local_ref_info();
    initialize_decl_cmds();
    initialize_nested_declaration();
    initialize_match_expr();
    initialize_elaborator();
    initialize_notation_cmd();
}
void finalize_frontend_lean_module() {
    finalize_notation_cmd();
    finalize_elaborator();
    finalize_match_expr();
    finalize_old_attributes();
    finalize_nested_declaration();
    finalize_decl_cmds();
    finalize_local_ref_info();
    finalize_pp();
    finalize_structure_instance();
    finalize_inductive_cmds();
    finalize_calc();
    finalize_parser_config();
    finalize_parser();
    finalize_scanner();
    finalize_builtin_exprs();
    finalize_builtin_cmds();
    finalize_parse_table();
    finalize_token_table();
    finalize_tokens();
    finalize_prenum();
}
}
