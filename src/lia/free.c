/**
 * @file    free.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Functions to free structs of the Lia.
 * @version 0.1
 * @date    2020-05-07
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdlib.h>
#include "lia/types.h"
#include "tree.h"

/**
 * @brief Free a token list
 * 
 * @param tk   Pointer to the first item in the list
 */
void tkfree(token_t *tk)
{
  if ( !tk )
    return;

  while (tk->next) {
    tk = tk->next;
    free(tk->last);
  }

  free(tk);
}

/**
 * @brief Free a instruction list.
 * 
 * @param list   The list to free.
 */
void inst_free(inst_t *list)
{
  inst_t *inst_next;
  token_t *token_next;
  token_t *this;

  while (list) {
    inst_next = list->next;

    for (this = list->child; this; this = token_next) {
      token_next = this->next;
      free(this);
    }

    free(list);
    list = inst_next;
  }
}

/**
 * @brief Free a lia_t struct.
 * 
 * @param lia   The struct to free.
 */
void lia_free(lia_t *lia)
{
  void *next;

  if ( !lia )
    return;

  tree_free(lia->proctree);
  tree_free(lia->imptree);
  tree_free(lia->cmdtree);

  inst_free(lia->instlist);
  
  for (path_t *this = lia->pathlist; this; this = next) {
    next = this->next;
    free(this);
  }

  for (ctx_t *this = lia->ctx; this; this = next) {
    next = this->last;
    free(this);
  }
}
