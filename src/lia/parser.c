/**
 * @file    parser.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   The parser of the Lia language
 * @version 0.1
 * @date    2020-05-01
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "lia/lia.h"

/**
 * @brief Finds the next token of the type
 * 
 * Returns the token found or the TK_EOF.
 * 
 * @param tk     The token's list
 * @param type   The type to find
 * @return token_t*  The pointer to the token, or to TK_EOF
 */
static token_t *tknext(token_t *tk, token_type_t type)
{
  while (tk->type != TK_EOF && tk->type != type)
    tk = tk->next;
  
  return tk;
}

/**
 * @brief Try match a sequence of token's types.
 * 
 * @param tk         The token's list
 * @param number     The number of tokens to match
 * @param ...        The list of token's types
 * @return int       -1 if match, or the index of the token not matched
 */
int tkseq(token_t *tk, unsigned int number, ...)
{
  va_list ap;
  va_start(ap, number);

  if ( !tk )
    return 0;

  for (int i = 0; i < number; i++) {
    if ( !tk || tk->type != va_arg(ap, token_type_t) ) {
      va_end(ap);
      return i;
    }

    tk = tk->next;
  }

  va_end(ap);
  return -1;
}

/**
 * @brief Inserts a instruction at end of inst_t.
 * 
 * @param list       The list to insert.
 * @param type       The type of the instruction.
 * @return inst_t*   Pointer to the new instruction.
 */
inst_t *inst_add(inst_t *list, inst_type_t type)
{
  inst_t *last = list;

  while (list && list->child) {
    last = list;
    list = list->next;
  }

  if ( !list ) {
    list = calloc(1, sizeof *list);
    last->next = list;
  }

  list->type = type;
  return list;
}

/**
 * @brief Analyzes the syntax of the Lia code
 * 
 * @param lia     The Lia struct
 * @param file    The file struct
 * @return int    The number of errors
 */
int lia_parser(lia_t *lia, imp_t *file)
{
  token_t *this = file->tklist;

  if ( !lia->proctree )
    lia->proctree = calloc(1, sizeof (proc_t));
  if ( !lia->cmdtree )
    lia->cmdtree = calloc(1, sizeof (cmd_t));
  if ( !lia->macrotree )
    lia->macrotree = calloc(1, sizeof (macro_t));
  if ( !lia->instlist )
    lia->instlist = calloc(1, sizeof (inst_t));

  while (this && this->type != TK_EOF) {
    this = inst_parser(lia, file, this);
  }

  return lia->errcount;
}

token_t *inst_parser(lia_t *lia, imp_t *file, token_t *this)
{
  metakeyword_t meta;
  keyword_t key;
  token_t *tk;
  token_t *next;
  
  token_t *( *metakeys[] )(KEY_ARGS) = {
    [META_NEW] = meta_new,
    [META_IMPORT] = meta_import,
    [META_MACRO] = meta_macro
  };

  token_t *( *keys[] )(KEY_ARGS) = {
    [KEY_NONE] = cmd_verify,
    [KEY_FUNC] = key_func,
    [KEY_LOAD] = key_load,
    [KEY_STORE] = key_store,
    [KEY_PUSH] = key_push,
    [KEY_POP] = key_pop,
    [KEY_CALL] = key_call,
    [KEY_RET] = key_ret,
    [KEY_PROC] = key_proc,
    [KEY_ENDPROC] = key_endproc,
    [KEY_IF] = key_if,
    [KEY_ENDIF] = key_endif,
    [KEY_SAY] = key_say,
    [KEY_ASES] = key_ases
  };


  switch (this->type) {
  case TK_SEPARATOR:
    break; // Just ignore it

  case TK_OPENBRACKET:
    this = metanext(this);
    meta = ismetakey(this);
    if (meta == META_NONE) {
      lia_error(file->filename, this->next->line, this->next->column,
        "Expected a meta-keyword, instead have `%s'", this->next->text);
      
      this = tknext(this, TK_CLOSEBRACKET);
      if (this->next)
        this = this->next;

      lia->errcount++;
      break;
    }

    next = metakeys[meta](this, file, lia);
    if ( !next ) {
      this = tknext(this, TK_CLOSEBRACKET);
      if (this->next)
        this = this->next;
      
      lia->errcount++;
      break;
    }

    if (next->type != TK_CLOSEBRACKET) {
      lia_error(file->filename, next->line, next->column,
        "Expected ']', instead have `%s'", next->text);

      this = next;
      lia->errcount++;
      break;
    }

    if ( next->next->type != TK_SEPARATOR && next->next->type != TK_EOF ) {
      lia_error(file->filename, next->next->line, next->next->column,
        "Expected a instruction separator, instead have `%s'",
        next->next->text);
      
      this = tknext(next->next, TK_SEPARATOR)->last;
      lia->errcount++;
      break;
    }

    this = next->next;
    break;
  
  case TK_OPENPARENS:
    next = macro_expand(this, file, lia);
    if ( !next ) {
      this = tknext(this, TK_CLOSEPARENS);
      lia->errcount++;
      break;
    }

    this = next;
    break;
  
  case TK_ID:
    next = macro_expand(this, file, lia);
    if (next) {
      this = next;
      break;
    }

    if ( this->next->type == TK_OPENPARENS ) {
      this = tknext(this, TK_CLOSEPARENS);
      lia->errcount++;
      break;
    }

    tk = this->next;
    for (; tk->type != TK_SEPARATOR && tk->type != TK_EOF; tk = tk->next) {
      if (tk->type != TK_OPENPARENS && tk->type != TK_ID)
        continue;

      next = macro_expand(tk, file, lia);
      if (next) {
        tk->last->next = next->next;
      }
    }

    key = iskey(this);
    next = keys[key](this, file, lia);
    if ( !next ) {
      this = tknext(this, TK_SEPARATOR)->last;         
      lia->errcount++;
      break;
    }

    if (next->type != TK_SEPARATOR && next->type != TK_EOF) {
      lia_error(file->filename, next->line, next->column,
        "Expected a instruction separator, instead have `%s'",
        next->text);
      
      this = tknext(next, TK_SEPARATOR)->last;
      lia->errcount++;
      break;
    }

    this = next;
    break;
  
  default:
    lia_error(file->filename, this->line, this->column,
      "Parser: Unexpected token `%s'", this->text);
    
    this = tknext(this, TK_SEPARATOR);
    if (this->next)
      this = this->next;

    lia->errcount++;
  }

  return this->next;
}
