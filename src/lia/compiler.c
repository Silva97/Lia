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
 * @brief Push new context to stack.
 * 
 * @param lia      The lia_t struct.
 * @param start    The initial instruction creating the context.
 * @param endtype  The instruction type that finalizes the block.
 */
void lia_ctx_push(lia_t *lia, inst_t *start, inst_type_t endtype)
{
  ctx_t *new = calloc(1, sizeof *new);
  new->start = start;
  new->endtype = endtype;

  if ( !lia->ctx ) {
    lia->ctx = new;
  } else {
    new->last = lia->ctx;
    lia->ctx = new;
  }
}

/**
 * @brief Pop a context from stack
 * 
 * @param lia      The lia_t struct.
 * @return ctx_t*  The context.
 */
ctx_t *lia_ctx_pop(lia_t *lia)
{
  ctx_t *ret = lia->ctx;

  if (ret)
    lia->ctx = ret->last;

  return ret;
}

/**
 * @brief Process a Lia source code
 * 
 * @param filename   The file's name
 * @param input      The input to read the code
 * @return imp_t*    The imp_t* of the processed file.
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
  inst_t *next;
  inst_t *this = lia->instlist;
  inst_t *last = this;

  fputs("#!/usr/bin/env ases\n"
        "# Lia " LIA_TAG "\n\n",
        output);
  
  if ( !this->child )
    return lia->errcount;
  
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

    while (this->type != INST_ENDPROC) {
      this = lia_inst_compile(output, this, lia, pretty);

      if ( !this->next )
        break;

      this = this->next;
    }

    this = lia_inst_compile(output, this, lia, pretty);
    next = this->next;

    if ( last == lia->instlist )
      lia->instlist = this->next;
    else
      last->next = this->next;

    this->next = NULL;
  }

  this = lia->instlist;
  while (this) {
    this = lia_inst_compile(output, this, lia, pretty);

    if ( !this->next )
      break;

    this = this->next;
  }

  if (lia->inproc) {
    this = lia->thisproc;

    lia_error(this->file->filename, this->child->line, this->child->column,
      "Unexpected end-of-file inside procedure '%s'.",
      this->child->next->text);
    lia->errcount++;
  }

  if (lia->ctx) {
    char *blnames[] = {
      [INST_ENDIF] = "endif"
    };

    for (ctx_t *ctx = lia->ctx; ctx; ctx = ctx->last) {
      lia_error(ctx->start->file->filename, ctx->start->child->line,
        ctx->start->child->column,
        "`%s' is a block, expects `%s' to close it.",
        ctx->start->child->text, blnames[ctx->endtype]);
      
      lia->errcount++;
    }
  }

  fputs(".3\n", output);
  return lia->errcount;
}


/**
 * @brief Generate the code for one instruction.
 * 
 * @param output     File to writes the code.
 * @param inst       Instruction to compile.
 * @param lia        The lia_t struct.
 * @param pretty     If nonzero, writes legible code at output.
 * @return inst_t*   The last instruction compiled.
 */
inst_t *lia_inst_compile(FILE *output, inst_t *inst, lia_t *lia, int pretty)
{
  long int lastpos;
  long int diff;
  operand_t operands[CMD_ARGC];
  cmd_t *cmd;
  proc_t *proc;
  ctx_t *ctx;
  inst_t *ret_inst = inst;
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
    case TK_STRING:
      operands[i].string = tk;
      break;
    default:
      lia_error(inst->file->filename, tk->line, tk->column,
        "Unexpected token `%s' at operand %d.", tk->text, i);
      lia->errcount++;
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
    lia_cmd_compile(lia->proctree, inst->file->filename,
      output, cmd, operands);
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
    fputs("<=", output);
    reg_compile(output, operands[0].reg, false);
    break;
  case INST_CALL:
    proc = tree_find(lia->proctree, hash(operands[0].procedure));
    if ( !proc ) {
      tk = inst->child->next;
      lia_error(inst->file->filename, tk->line, tk->column,
        "Procedure '%s' not defined.", tk->text);
      lia->errcount++;
      break;
    }

    proc_call(output, proc);
    break;
  case INST_RET:
    if ( !lia->inproc ) {
      lia_error(inst->file->filename, inst->child->line, inst->child->column,
        "%s", "`ret' instruction must be used inside a procedure.");
      lia->errcount++;
      break;
    }

    proc_ret(output, lia->inproc);
    if (inst->child->next) {
      if (inst->child->next->type == TK_ID)
        reg_compile(output, operands[0].reg, true);
      else
        imm_compile(output, operands[0].imm);
    }
    putc('*', output);
    break;
  case INST_PROC:
    proc = tree_find(lia->proctree, hash(operands[0].procedure));
    if (proc) {
      tk = inst->child->next;
      lia_error(inst->file->filename, tk->line, tk->column,
        "Redefinition of the '%s' procedure.", tk->text);
      lia->errcount++;
      break;
    }

    if (lia->inproc) {
      lia_error(inst->file->filename, inst->child->line, inst->child->column,
        "%s", "You can't declare a procedure inside another.");
      lia->errcount++;
      break;
    }

    lia->inproc = proc_add(lia->proctree, operands[0].procedure);
    lia->thisproc = inst;
    fputs("$(", output);
    break;
  case INST_ENDPROC:
    if ( !lia->inproc ) {
      lia_error(inst->file->filename, inst->child->line, inst->child->column,
        "%s", "`endproc' must be used at a procedure declaration.");
      lia->errcount++;
      break;
    }

    proc_ret(output, lia->inproc);
    fputs(".*@L+!>", output);
    lia->inproc = NULL;
    break;
  case INST_IF:
    if ( !strcmp(inst->child->text, "ifz") )
      fputs("~(", output);
    else
      fputs("?(", output);
    
    lia_inst_compile(output, inst->next, lia, false);
    putc('@', output);

    ret_inst = inst->next;
    break;
  case INST_IFBLOCK:
    if ( !strcmp(inst->child->text, "ifz") )
      fputs("~(", output);
    else
      fputs("?(", output);
    
    lia_ctx_push(lia, inst, INST_ENDIF);
    break;
  case INST_ENDIF:
    ctx = lia_ctx_pop(lia);
    if ( !ctx || ctx->endtype != INST_ENDIF ) {
      lia_error(inst->file->filename, inst->child->line, inst->child->column,
        "%s", "`endif' used outside a if..endif block.");
      lia->errcount++;
    }

    free(ctx);
    putc('@', output);
    break;
  case INST_SAY:
    str_compile(inst->file->filename, output, inst->child->next);
    break;
  case INST_ASES:
    fputs(inst->child->next->text, output);
    break;
  default:
    lia_error(inst->file->filename, inst->child->line, inst->child->column,
      "Sorry but my programmers not implemented `%s' yet!",
      inst->child->text);
    lia->errcount++;
  }

  diff = 40 - (int) (ftell(output) - lastpos);
  if (diff < 0)
    diff = 2;

  if (pretty) {
    fprintf(output, "%-*c# Line %04d: ", (int) diff,
      ' ', inst->child->line);
    
    if (inst->type == INST_IF) {
      fprintf(output, "%s ", inst->child->text);
      inst = inst->next;
    }

    for (token_t *tk = inst->child; tk; tk = tk->next) {

      switch (tk->type) {
      case TK_CHAR:
        fprintf(output, "'%s' ", tk->text);
        break;
      case TK_STRING:
        fprintf(output, "\"%s\" ", tk->text);
        break;
      default:
        fprintf(output, "%s ", tk->text);
      }
    }

    if (inst->type == INST_ENDPROC)
      putc('\n', output);

    putc('\n', output);
  }

  return ret_inst;
}
