/**
 * @file    ases.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Functions to generate code to Ases target.
 * @version 0.1
 * @date    2020-05-20
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lia/lia.h"
#include "lia/target.h"

void target_ases_start(ARGTARGET)
{
  fputs("#!/usr/bin/env ases\n"
        "# Lia " LIA_TAG "\n\n",
        output);

  // Reserving space at start of the memory.
  for (int i = 0; i < PROCINDEX; i++)
    putc('>', output);
  
  if (lia->target->pretty)
    fputs("\n\n", output);
}

void target_ases_end(ARGTARGET)
{
  fputs(".3\n", output);
}

inst_t *target_ases_compile(ARGCOMPILE)
{
  long int lastpos;
  long int diff;
  operand_t operands[CMD_ARGC];
  cmd_t *cmd;
  proc_t *proc;
  ctx_t *ctx;
  inst_t *ret_inst = inst;
  token_t *tk = inst->child->next;

  int pretty = lia->target->pretty;

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
      tk = lasttype(tk, TK_STRING);
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
    
    lia->target->pretty = false;
    target_ases_compile(output, inst->next, lia);
    lia->target->pretty = pretty;
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
    if ( !str_compile(inst->file->filename, output, inst->child->next) )
      lia->errcount++;
    break;
  case INST_ASES:
    for (token_t *tk = inst->child->next; tk && tk->type == TK_STRING; tk = metanext(tk))
      fputs(tk->text, output);
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

  if (lia->target->pretty) {
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
