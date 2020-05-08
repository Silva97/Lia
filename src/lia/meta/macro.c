/**
 * @file    macro.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   The meta-macro parser.
 * @version 0.1
 * @date    2020-05-08
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lia/lia.h"

token_t *meta_macro(KEY_ARGS)
{
  token_type_t type;
  macro_t *macro;
  token_t *first;
  mtk_t *macro_tkseq = NULL;

  tk = metanext(tk);
  first = tk;

  if (tk->type != TK_ID) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected a valid macro name, instead have `%s'", tk->text);
    return NULL;
  }

  if ( tree_find(lia->macrotree, hash(tk->text)) ) {
    lia_error(file->filename, tk->line, tk->column,
      "Redeclaration of the macro '%s'", tk->text);
    return NULL;
  }

  macro = tree_insert(lia->macrotree, sizeof *macro, hash(tk->text));
  macro->name = tk->text;

  tk = metanext(tk);
  if (tk->type == TK_OPENPARENS) {
    tk = metanext(tk);

    if (tk->type != TK_CLOSEPARENS) {
      for (; tk->type != TK_CLOSEPARENS; tk = tk->next) {
        if (tk->type == TK_ID) {
          switch ( tkseq(tk, 3, TK_ID, TK_COLON, TK_ID) ) {
          case -1:
            break;
          case 0:
            lia_error(file->filename, tk->line, tk->column,
              "Expected a argument name, instead have `%s'", tk->text);
            return NULL;
          case 1:
            lia_error(file->filename, tk->next->line, tk->next->column,
              "Expected ':', instead have `%s'", tk->next->text);
            return NULL;
          case 2:
            tk = tk->next->next;
            lia_error(file->filename, tk->line, tk->column,
              "Expected a token type name, instead have `%s'", tk->text);
            return NULL;
          }

          tk = tk->next->next;
          type = name2tktype(tk->text);
          if (type < 0) {
            lia_error(file->filename, tk->line, tk->column,
              "`%s' is a invalid token type name.", tk->text);
            return NULL;
          }

          if ( !macro_tkseq )
            macro_tkseq = macro_tkseq_add(NULL, type, tk->last->last->text);
          else
            macro_tkseq_add(macro_tkseq, type, tk->last->last->text);
        } else if (tk->type == TK_CHAR) {
          type = name2tktype(tk->text);
          if (type < 0) {
            lia_error(file->filename, tk->line, tk->column,
              "`%s' is a invalid token.", tk->text);
            return NULL;
          }

          if (type == TK_CLOSEPARENS) {
            lia_error(file->filename, tk->line, tk->column,
              "%s", "You can't use ')' inside a macro.");
            return NULL;
          }

          if ( !macro_tkseq )
            macro_tkseq = macro_tkseq_add(NULL, type, NULL);
          else
            macro_tkseq_add(macro_tkseq, type, NULL);
        } else {
          lia_error(file->filename, tk->line, tk->column,
            "Expected a name or literal character, instead have: `%s'",
            tk->text);
          return NULL;
        }
      }
    }
    
    tk = metanext(tk);
  }

  if (tk->type != TK_EQUAL) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected '=', instead have: `%s'", tk->text);
    return NULL;
  }

  tk = metanext(tk);
  if (tk->type == TK_CLOSEBRACKET) {
    lia_error(file->filename, tk->line, tk->column,
      "%s", "Expected a minimum of one token for the macro's body.");
    return NULL;
  }

  macro->tkseq = macro_tkseq;
  macro->body = tk;

  for (int ctx = 1; tk && ctx; tk = tk->next) {
    if (tk->type == TK_OPENBRACKET)
      ctx++;
    else if (tk->type == TK_CLOSEBRACKET)
      ctx--;
  }

  if ( !tk ) {
    lia_error(file->filename, first->line, first->column,
      "%s", "Unexpected end-of-file inside macro declaration.");
    return NULL;
  }

  tk->last->last->next = NULL;
  return tk->last;
}
