/**
 * @file    action.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Actions system
 * @version 0.1
 * @date    2020-05-24
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lia/lia.h"

/** Executes an action */
int action(KEY_ARGS)
{
  static const char *list[] = {
    "stop",
    NULL
  };

  static int (*dolist[])(KEY_ARGS) = {
    action_stop
  };

  for (int i = 0; list[i]; i++) {
    if ( !strcmp(list[i], tk->text) )
      return dolist[i](tk, file, lia);
  }

  lia_error(file->filename, tk->line, tk->column,
    "Invalid action '%s'", tk->text);
  return 0;
}

int action_stop(KEY_ARGS)
{
  file->stop = true;
  return 1;
}


token_t *meta_action(KEY_ARGS)
{
  tk = metanext(tk);

  if ( !action(tk, file, lia) )
    return NULL;
  
  return metanext(tk);
}
