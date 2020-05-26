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
#include "lia/target.h"
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

  if ( !file->tklist ) {
    lia->errcount++;
    return NULL;
  }
  
  lia_parser(lia, file);
  return file;
}

/**
 * @brief Compile the lia_t struct to final code.
 * 
 * @param output    The file to writes the code.
 * @param lia       The lia_t struct.
 * @param target    Target to generate the code.
 * @return int      The number of errors.
 */
int lia_compiler(FILE *output, lia_t *lia)
{
  if (!output || !lia) {
    fputs("Compiler: Invalid call", stderr);
    return 9999;
  }

  inst_t *next;
  inst_t *this = lia->instlist;
  inst_t *last = this;
  
  if ( !this || !this->child )
    return lia->errcount;
  
  lia->target->start(output, lia);

  // Compiling the procedures first.
  for (; this; this = next) {
    if (this->type != INST_PROC) {
      last = this;
      next = this->next;
      continue;
    }

    while (this->type != INST_ENDPROC) {
      this = lia->target->compile(output, this, lia);

      if ( !this->next )
        break;

      this = this->next;
    }

    this = lia->target->compile(output, this, lia);
    next = this->next;

    if ( !next || next->type != INST_PROC ) {
      if ( last == lia->instlist && last->type == INST_PROC )
        lia->instlist = next;
      else
        last->next = next;

      last = next;
    }

    this->next = NULL;
  }

  this = lia->instlist;
  while (this) {
    this = lia->target->compile(output, this, lia);

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

  lia->target->end(output, lia);
  return lia->errcount;
}
