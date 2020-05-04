/**
 * @file    compiler.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   The lia_process() and lia_compiler() source code
 * @version 0.1
 * @date    2020-05-02
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
 * @brief Process a Lia source code
 * 
 * @param filename   The file's name
 * @param input      The input to read the code
 * @return lia_t*    The Lia struct with the processed content
 */
imp_t *lia_process(char *filename, FILE *input, lia_t *lia)
{
  if ( !lia->imptree )
    lia->imptree = calloc(1, sizeof (imp_t));

  imp_t *file = tree_insert(lia->imptree, sizeof (imp_t), hash(filename));

  if ( !file )
    return NULL;
  
  strcpy(file->filename, filename);
  file->input = input;

  file->tklist = lia_lexer(filename, input);

  if (file->tklist)
    lia->errcount += lia_parser(lia, file);

  return file;
}

/**
 * @brief Compile the lia_t struct to Ases code.
 * 
 * @param output    The file to writes the code
 * @param lia       The lia_t struct.
 * @param pretty    If nonzero, writes legible code at output.
 * @return int      The number of errors.
 */
int lia_compiler(FILE *output, lia_t *lia, int pretty)
{
  inst_t *first;
  inst_t *last;
  inst_t *next;
  inst_t *this = lia->instlist;
  int errcount = 0;

  fputs("#!/usr/bin/env ases\n"
        "# Lia " LIA_TAG "\n\n",
        output);
  
  // Reserving space at start of the memory.
  for (int i = 0; i < PROCINDEX; i++)
    putc('>', output);
  
  if (pretty)
    fputs("\n\n", output);

  // Compiling the procedures first.
  for (; this; this = next) {
    if (this->type != INST_PROC) {
      last = this;
      next = this->next;
      continue;
    }
    
    first = this;

    while (this->type != INST_ENDPROC) {
      errcount += lia_inst_compile(output, this, lia, pretty);

      if ( !this->next )
        break;

      this = this->next;
    }

    lia_inst_compile(output, this, lia, pretty);
    next = this->next;

    if ( !last )
      lia->instlist = this->next;
    else
      last->next = this->next;

    this->next = NULL;
    inst_free(first);
  }

  this = lia->instlist;
  while (this) {
    errcount += lia_inst_compile(output, this, lia, pretty);

    if ( !this->next )
      break;

    this = this->next;
  }

  if (lia->inproc) {
    lia_error(this->file->filename, lia->thisproc->line, lia->thisproc->column,
      "Unexpected end-of-file inside procedure '%s'.",
      lia->thisproc->next->text);
    errcount++;
  }

  lia->errcount += errcount;
  return errcount;
}

int lia_inst_compile(FILE *output, inst_t *inst, lia_t *lia, int pretty)
{
  long int lastpos;
  long int diff;
  operand_t operands[CMD_ARGC];
  cmd_t *cmd;
  proc_t *proc;
  token_t *tk = inst->child->next;
  int errcount = 0;

  for (int i = 0; tk && i < CMD_ARGC; i++) {
    switch (tk->type) {
    case TK_CHAR:
    case TK_IMMEDIATE:
      operands[i].imm = tk->value;
      break;
    case TK_ID:
      if ( isreg(tk) ) {
        strcpy(operands[i].reg, tk->text);
      } else {
        operands[i].procedure = tk->text;
      }
      break;
    }

    if ( !tk->next )
      break;
    
    tk = tk->next->next;
  }

  lastpos = ftell(output);

  switch (inst->type) {
  case INST_CMD:
    cmd = tree_find( lia->cmdtree, hash(inst->child->text) );
    lia_cmd_compile(lia->proctree, output, cmd, operands);
    break;
  case INST_FUNC:
    putc(inst->child->next->text[0], output);
    break;
  case INST_LOAD:
    putc('=', output);
    reg_compile(output, operands[0].reg, false);
    break;
  case INST_STORE:
    if (inst->child->next->type == TK_ID)
      reg_compile(output, operands[0].reg, true);
    else
      imm_compile(output, operands[0].imm);
    
    putc('!', output);
    break;
  case INST_PUSH:
    if (inst->child->next->type == TK_ID)
      reg_compile(output, operands[0].reg, true);
    else
      imm_compile(output, operands[0].imm);
    
    fputs("!>", output);
    break;
  case INST_POP:
    putc('=', output);
    reg_compile(output, operands[0].reg, false);
    putc('<', output);
    break;
  case INST_CALL:
    proc = tree_find(lia->proctree, hash(operands[0].procedure));
    if ( !proc ) {
      tk = inst->child->next;
      lia_error(inst->file->filename, tk->line, tk->column,
        "Procedure '%s' not defined.", tk->text);
      errcount++;
      break;
    }

    proc_call(output, proc);
    break;
  case INST_RET:
    if ( !lia->inproc ) {
      lia_error(inst->file->filename, inst->child->line, inst->child->column,
        "%s", "`ret' instruction must be used inside a procedure.");
      errcount++;
      break;
    }

    proc_ret(output, lia->inproc);
    if (inst->child->next)
      imm_compile(output, operands[0].imm);
    putc('*', output);
    break;
  case INST_PROC:
    proc = tree_find(lia->proctree, hash(operands[0].procedure));
    if (proc) {
      tk = inst->child->next;
      lia_error(inst->file->filename, tk->line, tk->column,
        "Redefinition of the '%s' procedure.", tk->text);
      errcount++;
      break;
    }

    if (lia->inproc) {
      lia_error(inst->file->filename, inst->child->line, inst->child->column,
        "%s", "You can't declare a procedure inside another.");
      errcount++;
      break;
    }

    lia->inproc = proc_add(lia->proctree, operands[0].procedure);
    lia->thisproc = inst->child;
    fputs("$(", output);
    break;
  case INST_ENDPROC:
    if ( !lia->inproc ) {
      lia_error(inst->file->filename, inst->child->line, inst->child->column,
        "%s", "`endproc' must be used at a procedure declaration.");
      errcount++;
      break;
    }

    proc_ret(output, lia->inproc);
    fputs(".*@D+!>", output);
    lia->inproc = NULL;
    break;
  default:
    lia_error(inst->file->filename, inst->child->line, inst->child->column,
      "Sorry but my programmers not implemented `%s' yet!",
      inst->child->text);
    errcount++;
  }

  diff = ftell(output) - lastpos;

  if (pretty) {
      fprintf(output, "%-*c# Line %04d: ", 40 - (int) diff,
        ' ', inst->child->line);

      for (token_t *tk = inst->child; tk; tk = tk->next) {
        if (tk->type == TK_CHAR)
          fprintf(output, "'%s' ", tk->text);
        else
          fprintf(output, "%s ", tk->text);
      }

      if (inst->type == INST_ENDPROC)
        putc('\n', output);

      putc('\n', output);
    }

  return errcount;
}