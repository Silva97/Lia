/**
 * @file    meta_keywords.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   The parsers of the meta-keywords
 * @version 0.1
 * @date    2020-05-01
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "filepath.h"
#include "lia/lia.h"

/**
 * @brief Verify if a token is a meta-keyword
 * 
 * @param tk              The token to verify
 * @return metakeyword_t  The meta-keyword's type
 * @return META_NONE      If is not a meta-keyword
 */
metakeyword_t ismetakey(token_t *tk)
{
  if (tk->type != TK_ID)
    return META_NONE;
  
  if ( !strcmp(tk->text, "new") )
    return META_NEW;
  if ( !strcmp(tk->text, "import") )
    return META_IMPORT;
  
  return META_NONE;
}

/**
 * @brief Returns next token ignoring separators
 * 
 * @param tk          The tokens' list
 * @return token_t*   The next token
 */
token_t *metanext(token_t *tk)
{
  tk = tk->next;
  while (tk->type == TK_SEPARATOR)
    tk = tk->next;

  return tk;
}


token_t *meta_new(KEY_ARGS)
{
  int number;
  cmd_arg_t args[CMD_ARGC] = { CMDNULL };
  char *name;

  tk = metanext(tk);

  if ( tk->type != TK_ID ) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected a identifier to command's name, instead have `%s'", tk->text);
  }

  name = tk->text;

  tk = metanext(tk);

  for (int i = 0; i < CMD_ARGC && tk->type != TK_EQUAL; i++) {
    number = tkseq(tk, 3, TK_ID, TK_COLON, TK_ID);
    if ( number >= 0 ) {
      switch (number) {
      case 0:
        lia_error(file->filename, tk->line, tk->column,
          "Expected a argument name, instead have `%s'", tk->text);
        break;
      case 1:
        lia_error(file->filename, tk->next->line, tk->next->column,
          "Expected ':', instead have `%s'", tk->next->text);
        break;
      case 2:
        lia_error(file->filename, tk->next->next->line, tk->next->next->column,
          "Expected a type name, instead have `%s'", tk->next->next->text);
        break;
      }

      return NULL;
    }

    if (strlen(tk->text) > 1 || strchr( REGLIST, tolower(tk->text[0]) ) ||
        !( isupper(tk->text[0]) || islower(tk->text[0]) ) ) {
      lia_error(file->filename, tk->line, tk->column,
        "`%s' is a invalid argument name", tk->text);
      return NULL;
    }

    char *find = strchr( "irps", tolower(tk->next->next->text[0]) );
    if ( !find || tk->next->next->text[1] ) {
      lia_error(file->filename, tk->next->next->line, tk->next->next->column,
        "`%s' is a invalid type name", tk->next->next->text);
      return NULL;
    }

    args[i].name = tk->text[0];
    args[i].type = tk->next->next->text[0];

    tk = metanext(tk->next->next);
  }

  if (tk->type != TK_EQUAL) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected '=', instead have `%s'", tk->text);
    return NULL;
  }

  tk = metanext(tk);

  if (tk->type != TK_STRING) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected a string, instead have `%s'", tk->text);
    return NULL;
  }

  lia_cmd_new(lia->cmdtree, name, args, tk->text);
  return metanext(tk);
}

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