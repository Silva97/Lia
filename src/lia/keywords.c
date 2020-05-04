/**
 * @file    meta_keywords.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   The parsers of the meta-keywords
 * @version 0.1
 * @date    2020-05-01
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lia/lia.h"

/**
 * @brief Verify if a token is a keyword
 * 
 * @param tk            The token to verify
 * @return keyword_t    The keyword's type
 * @return KEY_NONE     If is not a keyword
 */
keyword_t iskey(token_t *tk)
{
  if (tk->type != TK_ID)
    return KEY_NONE;
  
  if ( !strcmp(tk->text, "func") )
    return KEY_FUNC;
  if ( !strcmp(tk->text, "load") )
    return KEY_LOAD;
  if ( !strcmp(tk->text, "store") )
    return KEY_STORE;
  if ( !strcmp(tk->text, "push") )
    return KEY_PUSH;
  if ( !strcmp(tk->text, "pop") )
    return KEY_POP;
  if ( !strcmp(tk->text, "call") )
    return KEY_CALL;
  if ( !strcmp(tk->text, "ret") )
    return KEY_RET;
  if ( !strcmp(tk->text, "proc") )
    return KEY_PROC;
  if ( !strcmp(tk->text, "endproc") )
    return KEY_ENDPROC;
  if ( !strcmp(tk->text, "prtab") )
    return KEY_PRTAB;
  if ( !strcmp(tk->text, "ifz") || !strcmp(tk->text, "ifnz") )
    return KEY_IF;
  if ( !strcmp(tk->text, "endif") )
    return KEY_ENDIF;
  
  return KEY_NONE;
}

token_t *cmd_verify(KEY_ARGS)
{
  token_t *first = tk;
  cmd_t *cmd = tree_find(lia->cmdtree, hash(tk->text));

  if ( !cmd ) {
    lia_error(file->filename, tk->line, tk->column,
      "`%s' is not a valid keyword or defined command.", tk->text);
    return NULL;
  }

  for (int i = 0; i < cmd->argc; i++) {
    tk = tk->next;

    switch (cmd->args[i].type) {
    case 'r':
      if ( !isreg(tk) ) {
        lia_error(file->filename, tk->line, tk->column,
          "Command '%s' expects a register at operand %d.", first->text, i+1);
        return NULL;
      }
      break;
    case 'i':
      if (tk->type != TK_IMMEDIATE && tk->type != TK_CHAR) {
        lia_error(file->filename, tk->line, tk->column,
          "Command '%s' expects a immediate value at operand %d.",
          first->text, i+1);
        return NULL;
      }
      break;
    case 'p':
      if ( tk->type != TK_ID || isreg(tk) ) {
        lia_error(file->filename, tk->line, tk->column,
          "Command '%s' expects a procedure name at operand %d.",
          first->text, i+1);
        return NULL;
      }
      break;
    }

    if (tk->next->type != TK_COMMA)
      break;
    
    tk = tk->next;
  }

  token_t *next = tk->next;

  if (next->type != TK_SEPARATOR && next->type != TK_EOF) {
    lia_error(file->filename, next->line, next->column,
      "Command '%s' expects %d operands.", first->text, cmd->argc);
    return NULL;
  }

  inst_t *inst = inst_add(lia->instlist, INST_CMD);
  inst->child = first;
  inst->file = file;
  tk->next = NULL;

  return next;
}

/** op1 = register */
static token_t *key_op1reg(KEY_ARGS, inst_type_t type)
{
  tk = tk->next;

  if ( !isreg(tk) ) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected a register name, instead have `%s'", tk->text);
    return NULL;
  }

  token_t *next = tk->next;
  inst_t *inst = inst_add(lia->instlist, type);
  inst->child = tk->last;
  inst->file = file;
  tk->next = NULL;

  return next;
}

/** op1 = register/immediate */
static token_t *key_op1ri(KEY_ARGS,  inst_type_t type)
{
  tk = tk->next;

  if ( !isreg(tk) && tk->type != TK_IMMEDIATE && tk->type != TK_CHAR ) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected a register name or immediate value, instead have `%s'", tk->text);
    return NULL;
  }

  token_t *next = tk->next;
  inst_t *inst = inst_add(lia->instlist, type);
  inst->child = tk->last;
  inst->file = file;
  tk->next = NULL;

  return next;
}

/** op1 = TK_ID */
static token_t *key_op1id(KEY_ARGS,  inst_type_t type)
{
  tk = tk->next;

  if ( tk->type != TK_ID || isreg(tk) ) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected a procedure name, instead have `%s'", tk->text);
    return NULL;
  }

  token_t *next = tk->next;
  inst_t *inst = inst_add(lia->instlist, type);
  inst->child = tk->last;
  inst->file = file;
  tk->next = NULL;

  return next;
}

/** No arguments */
static token_t *key_opnone(KEY_ARGS,  inst_type_t type)
{
  token_t *next = tk->next;
  inst_t *inst = inst_add(lia->instlist, type);
  inst->child = tk;
  inst->file = file;
  tk->next = NULL;

  return next;
}

token_t *key_func(KEY_ARGS)
{
  tk = tk->next;

  if (tk->type != TK_IMMEDIATE) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected number, instead have `%s'", tk->text);
    return NULL;
  }

  if (tk->value < 0 || tk->value > 9) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected number between 0 and 9, instead: %d", tk->value);
    return NULL;
  }

  token_t *next = tk->next;
  inst_t *inst = inst_add(lia->instlist, INST_FUNC);
  inst->child = tk->last;
  inst->file = file;
  tk->next = NULL;

  return next;
}

token_t *key_load(KEY_ARGS)
{
  return key_op1reg(tk, file, lia, INST_LOAD);
}

token_t *key_store(KEY_ARGS)
{
  return key_op1ri(tk, file, lia, INST_STORE);
}

token_t *key_push(KEY_ARGS)
{
  return key_op1ri(tk, file, lia, INST_PUSH);
}

token_t *key_pop(KEY_ARGS)
{
  return key_op1reg(tk, file, lia, INST_POP);
}

token_t *key_call(KEY_ARGS)
{
  return key_op1id(tk, file, lia, INST_CALL);
}

token_t *key_proc(KEY_ARGS)
{
  return key_op1id(tk, file, lia, INST_PROC);
}

token_t *key_endproc(KEY_ARGS)
{
  return key_opnone(tk, file, lia, INST_ENDPROC);
}

token_t *key_prtab(KEY_ARGS)
{
  return key_opnone(tk, file, lia, INST_PRTAB);
}

token_t *key_ret(KEY_ARGS)
{
  token_t *next;
  tk = tk->next;

  if (tk->type == TK_IMMEDIATE) {
    next = tk->next;
    tk->next = NULL;
  } else if (tk->type != TK_SEPARATOR && tk->type != TK_EOF) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected a literal number, instead have `%s'", tk->text);
    return NULL;
  } else {
    next = tk;
    tk->last->next = NULL;
  }

  inst_t *inst = inst_add(lia->instlist, INST_RET);
  inst->child = tk->last;
  inst->file = file;

  return next;
}

token_t *key_if(KEY_ARGS)
{
  inst_t *inst;
  token_t *next;

  if (tk->next->type == TK_ID) {
    inst = inst_add(lia->instlist, INST_IF);
    inst->child = tk;
    
    next = calloc(1, sizeof *next);
    next->line = tk->line;
    next->type = TK_SEPARATOR;
    next->last = tk;
    next->next = tk->next;
    tk->next = NULL;
  } else {
    next = tk->next;
    inst = inst_add(lia->instlist, INST_IFBLOCK);
    inst->child = tk;
    tk->next = NULL;
  }

  inst->file = file;
  return next;
}

token_t *key_endif(KEY_ARGS)
{
  return key_opnone(tk, file, lia, INST_ENDIF);
}