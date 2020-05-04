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

void lia_compiler(FILE *output, lia_t *lia, int pretty)
{
  long int lastpos;
  long int diff;
  inst_t *this = lia->instlist;

  fputs("#!/usr/bin/env ases\n"
        "# Lia " TAG "\n\n",
        output);

  while (this) {
    lastpos = ftell(output);
    lia_inst_compile(output, this, lia);

    diff = ftell(output) - lastpos;

    if (pretty) {
      fprintf(output, "%-*c# Line %04d: ", 30 - (int) diff,
        ' ', this->child->line);

      for (token_t *tk = this->child; tk; tk = tk->next) {
        if (tk->type == TK_CHAR)
          fprintf(output, "'%s' ", tk->text);
        else
          fprintf(output, "%s ", tk->text);
      }
      
      if (this->next)
        putc('\n', output);
    }

    this = this->next;
  }
}

void lia_inst_compile(FILE *output, inst_t *inst, lia_t *lia)
{
  operand_t operands[CMD_ARGC];
  cmd_t *cmd;
  token_t *tk = inst->child->next;

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

  switch (inst->type) {
    case INST_CMD:
      cmd = tree_find( lia->cmdtree, hash(inst->child->text) );
      lia_cmd_compile(lia->proctree, output, cmd, operands);
      break;
    case INST_FUNC:
      putc(inst->child->next->text[0], output);
      break;
    default:
      lia_error(inst->file->filename, inst->child->line, inst->child->column,
        "Sorry but my programmers not implemented `%s' yet!",
        inst->child->text);
  }
}