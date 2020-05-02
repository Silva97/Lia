/**
 * @file    compile.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   The lia_process() and lia_compile() source code
 * @version 0.1
 * @date    2020-05-02
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lia/lia.h"

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
  lia->errcount = lia_parser(lia, file);

  return file; 
}