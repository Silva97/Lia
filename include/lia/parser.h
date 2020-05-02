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
token_t *meta_new(KEY_ARGS);
token_t *meta_import(KEY_ARGS);

keyword_t iskey(token_t *tk);
int tkseq(token_t *tk, unsigned int number, ...);

inst_t *inst_add(inst_t *list, inst_type_t type);
int lia_parser(lia_t *lia, imp_t *file);

#endif /* _LIA_PARSER_H */