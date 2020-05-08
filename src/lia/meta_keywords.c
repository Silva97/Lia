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
  if ( !strcmp(tk->text, "macro") )
    return META_MACRO;
  
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

/**
 * @brief Inserts in a list of tokens.
 * 
 * @param list      The list to insert
 * @param type      The type of the token
 * @return mtk_t*   The new added item
 */
mtk_t *macro_tkseq_add(mtk_t *list, token_type_t type, char *name)
{
  if ( !list ) {
    list = calloc(1, sizeof *list);
    list->type = type;
    list->name = name;
    return list;
  }

  while (list->next)
    list = list->next;

  list->next = calloc(1, sizeof *list);
  list->next->last = list;
  list->next->type = type;
  list->next->name = name;
  return list->next;
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

token_t *meta_macro(KEY_ARGS)
{
  token_type_t type;
  macro_t *macro;
  token_t *first;
  mtk_t *macro_tkseq = NULL;
  const int namesize = TKMAX + 64;

  tk = metanext(tk);
  first = tk;

  if (tk->type != TK_ID) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected a valid macro name, instead have `%s'", tk->text);
    return NULL;
  }

  char *lastchar;
  char *macro_name = malloc(namesize + 1);
  strcpy(macro_name, tk->text);

  tk = metanext(tk);
  if (tk->type == TK_OPENPARENS) {
    tk = metanext(tk);

    if (tk->type != TK_CLOSEPARENS) {
      for (; tk->type != TK_CLOSEPARENS; tk = tk->next) {
        if (tk->type == TK_ID) {
          switch ( tkseq(tk, 3, TK_ID, TK_COLON, TK_ID) ) {
          case -1:
            break;
          case 0:
            lia_error(file->filename, tk->line, tk->column,
              "Expected a argument name, instead have `%s'", tk->text);
            return NULL;
          case 1:
            lia_error(file->filename, tk->next->line, tk->next->column,
              "Expected ':', instead have `%s'", tk->next->text);
            return NULL;
          case 2:
            tk = tk->next->next;
            lia_error(file->filename, tk->line, tk->column,
              "Expected a token type name, instead have `%s'", tk->text);
            return NULL;
          }

          tk = tk->next->next;
          type = name2tktype(tk->text);
          if (type < 0) {
            lia_error(file->filename, tk->line, tk->column,
              "`%s' is a invalid token type name.", tk->text);
            return NULL;
          }


          lastchar = strchr(macro_name, '\0');
          sprintf(lastchar, "_%d", type);

          if ( !macro_tkseq )
            macro_tkseq = macro_tkseq_add(NULL, type, tk->last->last->text);
          else
            macro_tkseq_add(macro_tkseq, type, tk->last->last->text);
        } else if (tk->type == TK_CHAR) {
          type = name2tktype(tk->text);
          if (type < 0) {
            lia_error(file->filename, tk->line, tk->column,
              "`%s' is a invalid token.", tk->text);
            return NULL;
          }

          if (type == TK_CLOSEPARENS) {
            lia_error(file->filename, tk->line, tk->column,
              "%s", "You can't use ')' inside a macro.");
            return NULL;
          }

          lastchar = strchr(macro_name, '\0');
          sprintf(lastchar, "_%d", type);

          if ( !macro_tkseq )
            macro_tkseq = macro_tkseq_add(NULL, type, NULL);
          else
            macro_tkseq_add(macro_tkseq, type, NULL);
        } else {
          lia_error(file->filename, tk->line, tk->column,
            "Expected a name or literal character, instead have: `%s'",
            tk->text);
          return NULL;
        }
      }
    }
    
    tk = metanext(tk);
  }

  if (tk->type != TK_EQUAL) {
    lia_error(file->filename, tk->line, tk->column,
      "Expected '=', instead have: `%s'", tk->text);
    return NULL;
  }

  tk = metanext(tk);
  macro = tree_insert(lia->macrotree, sizeof *macro, hash(macro_name));
  macro->name = macro_name;
  macro->tkseq = macro_tkseq;
  macro->body = tk;

  for (int ctx = 1; tk && ctx; tk = tk->next) {
    if (tk->type == TK_OPENBRACKET)
      ctx++;
    else if (tk->type == TK_CLOSEBRACKET)
      ctx--;
  }

  if ( !tk ) {
    lia_error(file->filename, first->line, first->column,
      "%s", "Unexpected end-of-file inside macro declaration.");
    return NULL;
  }

  tk->last->last->next = NULL;
  return tk->last;
}
