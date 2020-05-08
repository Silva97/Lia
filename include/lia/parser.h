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

metakeyword_t ismetakey(token_t *tk);
token_t *metanext(token_t *tk);
mtk_t *macro_tkseq_add(mtk_t *list, token_type_t type, char *name);
token_t *meta_new(KEY_ARGS);
token_t *meta_import(KEY_ARGS);
token_t *meta_macro(KEY_ARGS);

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

int tkseq(token_t *tk, unsigned int number, ...);
inst_t *inst_add(inst_t *list, inst_type_t type);
int lia_parser(lia_t *lia, imp_t *file);

#endif /* _LIA_PARSER_H */
