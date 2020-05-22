/**
 * @file    require.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   The meta-require parser.
 * @version 0.2
 * @date    2020-05-13
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lia/lia.h"
#include "tree.h"

token_t *meta_require(KEY_ARGS)
{
  char name[TKMAX + 32];

  while (tk) {
    tk = metanext(tk);

    if (tk->type != TK_STRING) {
      lia_error(file->filename, tk->line, tk->column,
        "Expected string, instead have: `%s'", tk->text);
      file->stop = true;
      return NULL;
    }

    if (lia->target)
      chrrep(name, tk->text, '$', lia->target->name);
    else
      strcpy(name, tk->text);

    if ( !tree_find(lia->imptree, hash(name)) ) {
      lia_error(file->filename, tk->line, tk->column,
        "Required module '%s' not imported yet.", name);
      file->stop = true;
      return NULL;
    }

    tk = metanext(tk);
    if (tk->type != TK_COMMA)
      break;
  }

  return tk;
}
