/**
 * @file    lexer.h
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Header file to declare the lexer's types.
 * @version 0.1
 * @date    2020-04-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef _LIA_LEXER_H
#define _LIA_LEXER_H

#include <stdio.h>
#include <inttypes.h>

/** Token maximum size + 1 */
#define TKMAX 129

/** Enumeration to token's type. */
typedef enum token_type {
  TK_EOF,
  TK_ID,
  TK_SEPARATOR,
  TK_OPENBRACKET,
  TK_CLOSEBRACKET,
  TK_COLON,
  TK_COMMA,
  TK_EQUAL,
  TK_IMMEDIATE,
  TK_CHAR,
  TK_STRING
} token_type_t;

/** Structure of a token */
typedef struct token {
  struct token *next;
  struct token *last;
  int line;
  int column;

  token_type_t type;
  char text[TKMAX];
  uint8_t value;
} token_t;


token_t *lia_lexer(char *filename, FILE *input);
void tkfree(token_t *tk);

#endif /* _LIA_LEXER_H */