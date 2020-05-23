/**
 * @file    parser.h
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Header file declaring the parser's prototypes
 * @version 0.1
 * @date    2020-04-30
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#ifndef _LIA_PARSER_H
#define _LIA_PARSER_H

#include "lia/types.h"

/** The name of the special expression macro */
#define MACRO_EXPR "expr"
#define EXPR_LVALUE "rc"


metakeyword_t ismetakey(token_t *tk);
token_t *metanext(token_t *tk);
token_t *lasttype(token_t *tk, token_type_t type);
mtk_t *macro_tkseq_add(mtk_t *list, token_type_t type, char *name);
void chrrep(char *dest, char *src, int placeholder, const char *new);
void macro_seq_print(void *tree_node);
token_t *macro_expand(token_t *tk, imp_t *file, lia_t *lia);
token_t *meta_new(KEY_ARGS);
token_t *meta_import(KEY_ARGS);
token_t *meta_macro(KEY_ARGS);
token_t *meta_require(KEY_ARGS);

keyword_t iskey(token_t *tk);
token_t *cmd_verify(KEY_ARGS);
token_t *key_func(KEY_ARGS);
token_t *key_load(KEY_ARGS);
token_t *key_store(KEY_ARGS);
token_t *key_push(KEY_ARGS);
token_t *key_pop(KEY_ARGS);
token_t *key_call(KEY_ARGS);
token_t *key_proc(KEY_ARGS);
token_t *key_endproc(KEY_ARGS);
token_t *key_prtab(KEY_ARGS);
token_t *key_ret(KEY_ARGS);
token_t *key_if(KEY_ARGS);
token_t *key_endif(KEY_ARGS);
token_t *key_say(KEY_ARGS);
token_t *key_ases(KEY_ARGS);

int tkseq(token_t *tk, unsigned int number, ...);
inst_t *inst_add(inst_t *list, inst_type_t type);
token_t *macro_set(macro_t *tree, char *name, token_type_t type);
void macrostr_set(macro_t *tree, char *name, const char *value);
int lia_parser(lia_t *lia, imp_t *file);
token_t *inst_parser(lia_t *lia, imp_t *file, token_t *this);

#endif /* _LIA_PARSER_H */
