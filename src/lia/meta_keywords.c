/**
 * @file    meta_keywords.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Auxiliary functions to parsing meta-keywords.
 * @version 0.1
 * @date    2020-05-01
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdlib.h>
#include <string.h>
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
  
  const char *list[] = {
    [META_NEW] = "new",
    [META_IMPORT] = "import",
    [META_MACRO] = "macro",
    [META_REQUIRE] = "require",
    [META_IF] = "if",
    [META_ACTION] = "action",
    [META_NONE] = NULL
  };
  
  for (int i = 0; list[i]; i++) {
    if ( !strcmp(list[i], tk->text) )
      return i;
  }
  
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
  while (tk && tk->type == TK_SEPARATOR)
    tk = tk->next;

  return tk;
}

/**
 * @brief Returns last token in a sequence of type.
 * 
 * @param tk         Initial token
 * @param type       The type to find
 * @return token_t*  Last token of the type
 */
token_t *lasttype(token_t *tk, token_type_t type)
{
  token_t *next;
  if (tk->type != type)
    return tk;

  while (tk) {
    next = metanext(tk);
    if ( !next || next->type != type)
      break;
    tk = next;
  }
  
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

/**
 * @brief Replaces an char to an string.
 * 
 * @param dest         The buffer to writes new string
 * @param src          The initial source string
 * @param placeholder  The char to replaces
 * @param new          The new string to replaces the char
 */
void chrrep(char *dest, char *src, int placeholder, const char *new)
{
  size_t size = strlen(new);

  while (*src) {
    if (*src != placeholder) {
      *dest++ = *src++;
      continue;
    }

    memcpy(dest, new, size);
    dest += size;
    src++;
  }

  *dest = '\0';
}
