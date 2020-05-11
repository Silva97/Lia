/**
 * @file    hash.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Hash string using djb2 algorithm
 * @version 0.1
 * @date    2020-04-27
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 * 
 */
#include "tree.h"

/**
 * @brief Hash a string using djb2 algorithm
 * 
 * @param str              The string to hash
 * @return unsigned long   The hash result
 */
unsigned long int hash(char *str)
{
  unsigned long int hash = INITIAL_HASH;

  while (*str)
    hash = ( (hash << 5) + hash ) + *str++; /* hash * 33 + c */

  return hash;
}

/**
 * @brief Hashing using a cumulative sequence of values
 * 
 * @param output  The variable to writes the output hash
 * @param n       The number to hash.
 */
void hashint(unsigned long int *output, int n)
{
  if ( *output == 0 )
    *output = INITIAL_HASH;
  
  *output = ( (*output << 5) + *output ) + n;
}
