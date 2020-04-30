/**
 * @file    procedure.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Procedure system
 * @version 0.1
 * @date    2020-04-29
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include "lia/procedure.h"

/**
 * @brief Inserts a new procedure or gets your index.
 * 
 * This function is thread-unsafe.
 * 
 * @param root           The tree's root
 * @param name           The name of the procedure
 * @return unsigned int  The index of the procedure
 */
unsigned int proc_add(proc_t *root, char *name)
{
  static unsigned int index = PROCINDEX;
  unsigned long int hashname = hash(name);
  proc_t *elem = tree_find(root, hashname);

  if (elem)
    return elem->index;
  
  elem = tree_insert(root, sizeof (proc_t), hashname);
  elem->index = index++;

  return elem->index;
}

/**
 * @brief Writes the procedure's call
 * 
 * @param output   The file to write
 * @param index    The index of the procedure
 */
void proc_call(FILE *output, unsigned int index)
{
  fputs(PROC_CALL1, output);

  for (int i = 0; i < index; i++)
    putc('>', output);
  
  fputs(PROC_CALL2, output);
}

/**
 * @brief Writes the procedure's return instruction without *
 * 
 * @param output   The file to write
 * @param index    The index of the procedure
 */
void proc_ret(FILE *output, unsigned int index)
{
  unsigned int total = PROC_CALLSIZE + index;
  fputs("<=", output);

  for (int i = 0; i < total/10; i++)
    putc('6', output);
  
  total %= 10;

  if (total > 5) {
    putc('6', output);

    for (int i = 10 - total; i > 0; i--)
      putc('-', output);
  } else {
    for (int i = total; i > 0; i--)
      putc('+', output);
  }

  fputs("d", output);
}