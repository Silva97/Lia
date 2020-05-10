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

/**
 * @brief Expands the body of a macro.
 * 
 * @param tk         The token of the macro call.
 * @param file       The file where this token is.
 * @param lia        The lia_t struct.
 * @return token_t*  Last token before the macro's content.
 */
token_t *macro_expand(token_t *tk, imp_t *file, lia_t *lia)
{
  macro_t *macro;
  macro_arg_t *arg = NULL;
  token_t *first = tk;
  token_t *next;

  macro = tree_find(lia->macrotree, hash(tk->text));
  if ( !macro || !macro->body )
    return NULL;
  

  if ( !macro->tkseq ) {
    if (tk->next->type == TK_OPENPARENS) {
      if (tkseq(tk->next, 2, TK_OPENPARENS, TK_CLOSEPARENS) >= 0) {
        lia_error(file->filename, tk->line, tk->column,
          "Macro '%s' don't expects a sequence of tokens.", tk->text);
        return NULL;
      }
      tk = tk->next->next;
    }
  } else {
    if (tk->next->type != TK_OPENPARENS) {
      lia_error(file->filename, tk->next->line, tk->next->column,
        "%s", "Expected a list of tokens inside parens.");
      return NULL;
    }

    macro->argtree = calloc(1, sizeof (macro_arg_t));

    tk = tk->next->next;
    for (mtk_t *this = macro->tkseq; this; this = this->next) {
      if (tk->type == TK_ID) {
        next = macro_expand(tk, file, lia);
        if (next) {
          tk = next->next;
        }
      }

      switch (this->type) {
      case TK_ANY:
        if (tk->type != TK_IMMEDIATE && tk->type != TK_CHAR) {
          lia_error(file->filename, tk->line, tk->column,
            "Expected a literal number or character, instead have: `%s'", tk->text);
          return NULL;
        }
        break;
      case TK_REGISTER:
        if ( !isreg(tk) ) {
          lia_error(file->filename, tk->line, tk->column,
            "Expected a register name, instead have: `%s'", tk->text);
          return NULL;
        }
        break;
      default:
        if (this->type != tk->type) {
          lia_error(file->filename, tk->line, tk->column,
            "Expected `%s' token, instead have: `%s'", tktype2name(this->type),
            tk->text);
          return NULL;
        }
      }

      if (this->name) {
        arg = tree_insert(macro->argtree, sizeof *arg, hash(this->name));
        
        arg->name = this->name;
        arg->type = tk->type;
        arg->body = tk;
      }

      tk = tk->next;
    }
  }

  token_t *new;
  token_t *this = macro->body;
  token_t *body = malloc(sizeof *body);
  first->last->next = body;

  if (this->type == TK_ID)
    arg = tree_find(macro->argtree, hash(this->text));
  
  if (arg) {
    memcpy(body, arg->body, sizeof *body);
  } else {
    memcpy(body, macro->body, sizeof *body);
    body->line = first->line;
    body->column = first->column;
  }

  body->last = first->last;

  for (this = this->next; this; this = this->next) {
    new = malloc(sizeof *new);
    body->next = new;

    arg = NULL;
    if (this->type == TK_ID)
      arg = tree_find(macro->argtree, hash(this->text));
    
    if (arg) {
      memcpy(new, arg->body, sizeof *new);
    } else {
      memcpy(new, this, sizeof *new);
      new->line = first->line;
      new->column = first->column;
    }

    new->last = body;
    body = new;
    if ( !this->next )
      break;
  }

  tree_free(macro->argtree);
  macro->argtree = NULL;

  body->next = tk->next;
  tk->next->last = body;

  return first->last;
}
