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
#include <stdbool.h>
#include <string.h>
#include "lia/lia.h"
#include "tree.h"

/**
 * @brief Prints the sequence of tokens of a variant.
 * 
 * This function can be used with tree_map().
 * 
 * @param variant  The variant of a macro.
 */
void macro_seq_print(void *tree_node)
{
  macro_var_t *variant = tree_node;
  mtk_t *tkseq = variant->tkseq;

  putc('(', stderr);
  for (; tkseq; tkseq = tkseq->next) {
    putc(' ', stderr);
    if (tkseq->name)
      fprintf(stderr, "%s: ", tkseq->name);
    
    switch (tkseq->type) {
    case TK_STRING:
      fputs("str", stderr);
      break;
    case TK_CHAR:
      fputs("char", stderr);
      break;
    case TK_IMMEDIATE:
      fputs("number", stderr);
      break;
    case TK_REGISTER:
      fputs("reg", stderr);
      break;
    case TK_ID:
      fputs("id", stderr);
      break;
    default:
      fprintf(stderr, "'%s'", tktype2name(tkseq->type));
    }
  }

  fputs(" )\n", stderr);
}


token_t *meta_macro(KEY_ARGS)
{
  token_type_t type;
  macro_t *macro;
  token_t *first;
  mtk_t *macro_tkseq = NULL;
  unsigned long int tkseq_hash = INITIAL_HASH;

  tk = metanext(tk);
  first = tk;

  if (tk->type != TK_ID) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected a valid macro name, instead have `%s'", tk->text);
    return NULL;
  }

  macro = tree_find(lia->macrotree, hash(tk->text));
  if ( !macro ) {
    macro = tree_insert(lia->macrotree, sizeof *macro, hash(tk->text));
    macro->name = tk->text;
    macro->variants = calloc(1, sizeof (macro_var_t));
  }

  tk = metanext(tk);
  if (tk->type == TK_OPENPARENS) {
    tk = metanext(tk);

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
        if (type == TK_INVALID) {
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
        if (type == TK_INVALID) {
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

      hashint(&tkseq_hash, type);
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

  if ( tree_find(macro->variants, tkseq_hash) ) {
    lia_error(file->filename, first->line, first->column,
      "Redeclaration of macro '%s' with the same sequence of tokens.", macro->name);
    return NULL;
  }

  macro_var_t *variant = tree_insert(macro->variants, sizeof *variant, tkseq_hash);

  variant->tkseq = macro_tkseq;
  variant->body = tk;

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
  bool expr = false;
  macro_t *macro;
  macro_var_t *variant;
  macro_arg_t *argtree;
  macro_arg_t *arg = NULL;
  unsigned long int tkseq_hash = INITIAL_HASH;
  token_t *first = tk;
  token_t *next;
  token_t *firstseq = NULL;

  if (tk->type == TK_OPENPARENS) {
    macro = tree_find(lia->macrotree, hash(MACRO_EXPR));
    tk = tk->last;
    expr = true;
  } else {
    macro = tree_find(lia->macrotree, hash(tk->text));
    expr = !strcmp(tk->text, MACRO_EXPR);
  }

  if ( !macro )
    return NULL;

  if (tk->next->type == TK_OPENPARENS) {
    firstseq = tk->next;
    tk = tk->next->next;
    for (; tk->type != TK_CLOSEPARENS; tk = tk->next) {
      if (tk->type == TK_ID || tk->type == TK_OPENPARENS) {
        next = macro_expand(tk, file, lia);
        if (next) {
          tk->last->next = next->next;
          tk = next->next;
          
        }
      }

      if ( isreg(tk) )
        hashint(&tkseq_hash, TK_REGISTER);
      else
        hashint(&tkseq_hash, tk->type);
    }
  }

  variant = tree_find(macro->variants, tkseq_hash);
  if ( !variant ) {
    lia_error(file->filename, first->line, first->column,
      "Macro '%s' don't have a variant with this sequence. Instead try:",
      macro->name);
    tree_map(macro->variants, macro_seq_print);
    return NULL;
  }

  argtree = calloc(1, sizeof *argtree);

  if (firstseq) {
    firstseq = firstseq->next;
    for (mtk_t *this = variant->tkseq; this; this = this->next) {
      switch (this->type) {
      case TK_REGISTER:
        if ( !isreg(firstseq) ) {
          lia_error(file->filename, firstseq->line, firstseq->column,
            "Expected a register name, instead have: `%s'", firstseq->text);
          return NULL;
        }
        break;
      default:
        if (this->type != firstseq->type) {
          lia_error(file->filename, firstseq->line, firstseq->column,
            "Expected `%s' token, instead have: `%s'", tktype2name(this->type),
            firstseq->text);
          return NULL;
        }
      }

      if (this->name) {
        arg = tree_insert(argtree, sizeof *arg, hash(this->name));
        
        arg->name = this->name;
        arg->type = firstseq->type;
        arg->body = firstseq;
      }

      firstseq = firstseq->next;
    }
  }

  token_t *new;
  token_t *this = variant->body;
  token_t *body = malloc(sizeof *body);
  first->last->next = body;

  if (this->type == TK_ID)
    arg = tree_find(argtree, hash(this->text));
  else
    arg = NULL;
  
  if (arg) {
    memcpy(body, arg->body, sizeof *body);
  } else {
    memcpy(body, variant->body, sizeof *body);
    body->line = first->line;
    body->column = first->column;
  }

  body->last = first->last;

  for (this = this->next; this; this = this->next) {
    new = malloc(sizeof *new);
    body->next = new;

    arg = NULL;
    if (this->type == TK_ID)
      arg = tree_find(argtree, hash(this->text));
    
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

  tree_free(argtree);

  if (expr) {
    this = first->last->next;
    while (this && this->type != TK_EOF) {
      this = inst_parser(lia, file, this);
    }

    this = calloc(1, sizeof (token_t));
    this->type = TK_ID;
    this->line = first->line;
    this->column = first->column;
    strcpy(this->text, EXPR_LVALUE);

    first->last->next = this;
    this->last = first->last;
    this->next = tk->next;
    tk->next->last = this;
  } else {
    body->next = tk->next;
    tk->next->last = body;
  }

  return first->last;
}
