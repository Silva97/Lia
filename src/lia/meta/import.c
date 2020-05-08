/**
 * @file    import.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   The meta-import parser.
 * @version 0.1
 * @date    2020-05-08
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include "lia/lia.h"
#include "filepath.h"

token_t *meta_import(KEY_ARGS)
{
  token_t *first = tk;

  while (tk) {
    tk = metanext(tk);

    if (tk->type != TK_STRING) {
      lia_error(file->filename, tk->line, tk->column,
        "Expected string, instead have: `%s'", tk->text);
      return NULL;
    }

    FILE *input = pfind(tk->text, "r", lia->pathlist);

    if ( !input ) {
      lia_error(file->filename, first->line, first->column,
        "Module \"%s\" not found.", tk->text);
      return NULL;
    }

    lia_process(tk->text, input, lia);
    fclose(input);

    tk = metanext(tk);
    if (tk->type != TK_COMMA)
      break;
  }

  return tk;
}
