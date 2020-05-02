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
#include <string.h>
#include "lia/parser.h"
#include "lia/error.h"

/**
 * @brief Verify if a token is a keyword
 * 
 * @param tk            The token to verify
 * @return keyword_t    The keyword's type
 * @return KEY_NONE     If is not a keyword
 */
keyword_t iskey(token_t *tk)
{
  if (tk->type != TK_ID)
    return KEY_NONE;
  
  if ( !strcmp(tk->text, "func") )
    return KEY_FUNC;
  
  return KEY_NONE;
}