/**
 * @file    meta_keywords.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Auxiliary functions to parsing meta-keywords.
 * @version 0.1
 * @date    2020-05-01
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdlib.h>
#include <string.h>
#include "lia/lia.h"

/**
 * @brief Verify if a token is a meta-keyword
 * 
 * @param tk              The token to verify
 * @return metakeyword_t  The meta-keyword's type
 * @return META_NONE      If is not a meta-keyword
 */
metakeyword_t ismetakey(token_t *tk)
{
  if (tk->type != TK_ID)
    return META_NONE;
  
  if ( !strcmp(tk->text, "new") )
    return META_NEW;
  if ( !strcmp(tk->text, "import") )
    return META_IMPORT;
  if ( !strcmp(tk->text, "macro") )
    return META_MACRO;
  
  return META_NONE;
}

/**
 * @brief Returns next token ignoring separators
 * 
 * @param tk          The tokens' list
 * @return token_t*   The next token
 */
token_t *metanext(token_t *tk)
{
  tk = tk->next;
  while (tk->type == TK_SEPARATOR)
    tk = tk->next;

  return tk;
}

/**
 * @brief Inserts in a list of tokens.
 * 
 * @param list      The list to insert
 * @param type      The type of the token
 * @return mtk_t*   The new added item
 */
mtk_t *macro_tkseq_add(mtk_t *list, token_type_t type, char *name)
{
  if ( !list ) {
    list = calloc(1, sizeof *list);
    list->type = type;
    list->name = name;
    return list;
  }

  while (list->next)
    list = list->next;

  list->next = calloc(1, sizeof *list);
  list->next->last = list;
  list->next->type = type;
  list->next->name = name;
  return list->next;
}

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

      if (this->type != tk->type) {
        lia_error(file->filename, tk->line, tk->column,
          "Expected `%s' token, instead have: `%s'", tktype2name(this->type),
          tk->text);
        return NULL;
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
