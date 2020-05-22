/**
 * @file    import.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   The meta-import parser.
 * @version 0.2
 * @date    2020-05-08
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <string.h>
#include "lia/lia.h"
#include "filepath.h"

token_t *meta_import(KEY_ARGS)
{
  char name[TKMAX + 32];
  token_t *first = tk;

  while (tk) {
    tk = metanext(tk);

    if (tk->type != TK_STRING) {
      lia_error(file->filename, tk->line, tk->column,
        "Expected string, instead have: `%s'", tk->text);
      return NULL;
    }

    if (lia->target)
      chrrep(name, tk->text, '$', lia->target->name);
    else
      strcpy(name, tk->text);

    FILE *input = pfind(name, "r", lia->pathlist);

    if ( !input ) {
      lia_error(file->filename, first->line, first->column,
        "Module \"%s\" not found.", name);
      return NULL;
    }

    lia_process(name, input, lia);
    fclose(input);

    tk = metanext(tk);
    if (tk->type != TK_COMMA)
      break;
  }

  return tk;
}
