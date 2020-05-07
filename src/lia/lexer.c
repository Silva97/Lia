/**
 * @file    lexer.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Lexer to the Lia language.
 * @version 0.1
 * @date    2020-04-25
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 * 
 */
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lia/lexer.h"
#include "lia/error.h"
#include "lia/free.h"

static int istkid(int c)
{
  return isupper(c) || islower(c) || isdigit(c) || c == '_';
}

static int isstrvalid(int c)
{
  return c >= 1 && c <= 127;
}

/**
 * @brief Escape the character
 * 
 * @param c 
 * @return int  -1 if no valid escape.
 */
int chresc(int c)
{
  switch (c) {
  case '\\':
    return '\\';
  case 'n':
    return '\n';
  case 'r':
    return '\r';
  case 'b':
    return '\b';
  case 't':
    return '\t';
  case 'a':
    return '\a';
  case 'v':
    return '\v';
  case 'f':
    return '\f';
  case 'e':
    return '\x1b';
  case '0':
    return '\0';
  default:
    return -1;
  }
}

/**
 * @brief Do lexical analyze of a Lia code.
 * 
 * @param input      Input file to read the code.
 * @return token_t*  If successful
 * @return NULL      If error
 */
token_t *lia_lexer(char *filename, FILE *input)
{
  int ch;
  int line = 1;
  int column = 0;
  token_t *new;
  token_t *this = calloc(1, sizeof *this);
  token_t *first = this;

  this->last = NULL;

  while ( 1 ) {
    do {
      ch = getc(input);
      column++;
    } while ( isblank(ch) );

    if (ch == '#') {
      do {
        ch = getc(input);
      } while (ch != '\n');
    }

    this->line = line;
    this->column = column;

    switch (ch) {
    case EOF:
      this->type = TK_EOF;
      this->next = NULL;
      this->line = line;
      this->column = column;
      strcpy(this->text, ":EOF:");
      return first;

    case '\n':
      line++;
      column = 0;
      // No break here!
    case ';':
      this->type = TK_SEPARATOR;
      strcpy(this->text, ":SEPARATOR:");
      break;

    case '[':
      this->type = TK_OPENBRACKET;
      strcpy(this->text, "[");
      break;

    case ']':
      this->type = TK_CLOSEBRACKET;
      strcpy(this->text, "]");
      break;

    case '=':
      this->type = TK_EQUAL;
      strcpy(this->text, "=");
      break;

    case ':':
      this->type = TK_COLON;
      strcpy(this->text, ":");
      break;

    case ',':
      this->type = TK_COMMA;
      strcpy(this->text, ",");
      break;

    case '\'':
      this->type = TK_CHAR;
      
      ch = getc(input);

      if (ch == '\\') {
        ch = getc(input);
        int esc = chresc(ch);
        
        if (esc < 0) {
          lia_error(filename, line, column, "'\\%c' is not a valid escape character", ch);
          tkfree(first);
          return NULL;
        }

        this->value = esc;
        this->text[0] = '\\';
        this->text[1] = ch;
        column += 2;
      } else if ( !isstrvalid(ch) ) {
        lia_error(filename, line, column, "'%c' is not a valid character", ch);
        tkfree(first);
        return NULL;
      } else {
        this->value = ch;
        this->text[0] = ch;
        column++;
      }

      if (getc(input) != '\'') {
        lia_error(filename, line, column, "%s",
          "Literal character should have one byte of length");
        tkfree(first);
        return NULL;
      }

      column++;
      break;

    case '"':
      this->type = TK_STRING;
      
      for (int i = 0; (ch = getc(input)) != '"'; i++) {
        column++;

        if (i >= TKMAX) {
          lia_error(filename, this->line, this->column,
            "Maximum size of a string is %d characters", TKMAX-1);
          tkfree(first);
          return NULL;
        }

        if (ch == '\r' || ch == '\n') {
          lia_error(filename, line, column,
            "%s", "Unexpected break of line inside a string");
          tkfree(first);
          return NULL;
        }

        if ( !isstrvalid(ch) ) {
          lia_error(filename, line, column,
            "Invalid character 0x%02x ('%c') inside a string", ch, ch);
          tkfree(first);
          return NULL;
        }

        this->text[i] = ch;
      }

      column++;
      break;

    default:
      if ( isdigit(ch) ) {
        int (*filter)(int) = isdigit;
        this->type = TK_IMMEDIATE;
        this->text[0] = ch;

        ch = getc(input);
        column++;

        if ( isalnum(ch) ) {
          if ( ch == 'x' || ch == 'X' ) {
            filter = isxdigit;
          }

          this->text[1] = ch;

          for (int i = 2;; i++) {
            if (i >= TKMAX) {
              lia_error(filename, this->line, this->column,
                "Maximum size of a token is %d characters", TKMAX-1);
              tkfree(first);
              return NULL;
            }
            
            ch = getc(input);

            if ( !filter(ch) )
              break;

            this->text[i] = ch;
            column++;
          }
        }

        char *end;

        int i = strtoul(this->text, &end, 0);

        if ( *end ) {
          lia_error(filename, this->line, this->column,
            "Invalid literal number '%s'", this->text);
          tkfree(first);
          return NULL;
        }

        if (i < 0 || i > 255) {
          lia_error(filename, this->line, this->column,
            "Literal number should be between 0 and 255, instead: %d", i);
          tkfree(first);
          return NULL;
        }

        this->value = i;
        ungetc(ch, input);
      } else if ( istkid(ch) ) {
        this->type = TK_ID;
        this->text[0] = ch;

        for (int i = 1;; i++) {
          if (i >= TKMAX) {
            lia_error(filename, this->line, this->column,
              "Maximum size of a token is %d characters", TKMAX-1);
            tkfree(first);
            return NULL;
          }

          ch = getc(input);

          if ( !istkid(ch) )
            break;

          this->text[i] = ch;
          column++;
        }

        ungetc(ch, input);
      } else {
        lia_error(filename, line, column, "Unexpected character '%c'", ch);
        tkfree(first);
        return NULL;
      }
    }

    
    new = calloc(1, sizeof *new);
    new->last = this;
    this->next = new;
    this = new;
  }
}
