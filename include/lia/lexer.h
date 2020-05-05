/**
 * @file    lexer.h
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Header file to declare the lexer's prototypes.
 * @version 0.1
 * @date    2020-04-25
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#ifndef _LIA_LEXER_H
#define _LIA_LEXER_H

#include <stdio.h>
#include "lia/types.h"


int chresc(int c);

token_t *lia_lexer(char *filename, FILE *input);
void tkfree(token_t *tk);

#endif /* _LIA_LEXER_H */