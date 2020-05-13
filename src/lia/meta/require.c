/**
 * @file    require.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   The meta-require parser.
 * @version 0.1
 * @date    2020-05-13
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <stdbool.h>
#include "lia/lia.h"
#include "tree.h"

token_t *meta_require(KEY_ARGS)
{
  while (tk) {
    tk = metanext(tk);

    if (tk->type != TK_STRING) {
      lia_error(file->filename, tk->line, tk->column,
        "Expected string, instead have: `%s'", tk->text);
      file->stop = true;
      return NULL;
    }

    if ( !tree_find(lia->imptree, hash(tk->text)) ) {
      lia_error(file->filename, tk->line, tk->column,
        "Required module '%s' not imported yet.", tk->text);
      file->stop = true;
      return NULL;
    }

    tk = metanext(tk);
    if (tk->type != TK_COMMA)
      break;
  }

  return tk;
}
