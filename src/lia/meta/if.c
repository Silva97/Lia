/**
 * @file    if.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   The meta-if parser.
 * @version 0.1
 * @date    2020-05-23
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "lia/lia.h"
#include "tree.h"

/** Returns the macro's value */
static token_t *getmacro(macro_t *tree, char *name)
{
  macro_t *macro = tree_find(tree, hash(name));
  if ( !macro )
    return NULL;
  
  macro_var_t *variant = tree_find(macro->variants, INITIAL_HASH);
  if ( !variant )
    return NULL;
  
  return variant->body;
}

/** Returns if a token is a specific keyword */
static bool mif_key(token_t *tk, char *name)
{
  return ( tk && tk->type == TK_ID && !strcmp(tk->text, name) );
}

/** Returns if the token is any of the types */
static bool isany(token_t *tk, int n, ...)
{
  va_list ap;
  if ( !tk )
    return false;

  va_start(ap, n);
  while (n--) {
    if ( tk->type == va_arg(ap, token_type_t) )
      return true;
  }

  va_end(ap);
  return false;
}

static bool mif_expr(lia_t *lia, imp_t *file, token_t **tk)
{
  bool isnot = mif_key(*tk, "not");
  if (isnot)
    *tk = metanext(*tk);
  
  token_t *v1 = *tk;
  if ( !isany(v1, 4, TK_STRING, TK_IMMEDIATE, TK_CHAR, TK_ID) ) {
    lia_error(file->filename, v1->line, v1->column,
      "Expected a value, instead have `%s'", v1->text);
    *tk = NULL;
    return false;
  }

  *tk = metanext(*tk);

  if ( !isany(*tk, 2, TK_EQUAL, TK_EXCLAMATION) ) {
    if (v1->type == TK_ID)
      return (getmacro(lia->macrotree, v1->text) != NULL) != isnot;
    
    return (v1->type == TK_STRING || v1->value) != isnot;
  }

  if ( (*tk)->type != TK_EQUAL )
    isnot = !isnot;

  *tk = metanext(*tk);

  if ((*tk)->type != TK_EQUAL) {
    lia_error(file->filename, (*tk)->line, (*tk)->column,
      "Unexpected token '%s' inside if's expression", (*tk)->text);
    *tk = NULL;
    return false;
  }

  *tk = metanext(*tk);
  if ( mif_key(*tk, "then") ) {
    lia_error(file->filename, (*tk)->line, (*tk)->column,
      "%s", "Expected a value before 'then' keyword");
    *tk = NULL;
    return false;
  }

  token_t *v2 = *tk;
  if ( !isany(v2, 4, TK_STRING, TK_IMMEDIATE, TK_CHAR, TK_ID) ) {
    lia_error(file->filename, v2->line, v2->column,
      "Expected a value, instead have `%s'", v2->text);
    *tk = NULL;
    return false;
  }

  *tk = metanext(*tk);

  if (v1->type == TK_ID)
    v1 = getmacro(lia->macrotree, v1->text);
  if (v2->type == TK_ID)
    v2 = getmacro(lia->macrotree, v2->text);
  
  if ( !v1 || !v2 || v1->type != v2->type )
    return false;
  
  if (v1->type == TK_STRING)
    return !strcmp(v1->text, v2->text) != isnot;
  
  return (v1->value == v2->value) != isnot;
}

token_t *meta_if(KEY_ARGS)
{
  tk = metanext(tk);

  bool expr = mif_expr(lia, file, &tk);
  if ( !tk )
    return NULL;
    
  if ( !mif_key(tk, "then") ) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected 'then' keyword, instead have `%s'", tk->text);
    return NULL;
  }

  tk = metanext(tk);

  if ( !expr ) {
    for (int ctx = 1; tk && ctx; tk = tk->next) {
      if (tk->type == TK_OPENBRACKET)
        ctx++;
      else if (tk->type == TK_CLOSEBRACKET)
        ctx--;
    }

    return tk->last;
  }

  if (tk->type == TK_EXCLAMATION) {
    if ( !action(tk->next, file, lia) )
      return NULL;
    
    return tk->next->next;
  }

  while (tk && tk->type != TK_CLOSEBRACKET && !file->stop) {
    if (tk->type == TK_EOF) {
      lia_error(file->filename, tk->line, tk->column,
        "%s", "Unexpected end-of-file inside meta-if");
      return NULL;
    }
    
    tk = inst_parser(lia, file, tk);
  }

  return tk;
}
