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


/**
 * @brief Hash a string using djb2 algorithm
 * 
 * @param str              The string to hash
 * @return unsigned long   The hash result
 */
unsigned long int hash(unsigned char *str)
{
  unsigned long int hash = 5381;

  while (*str)
    hash = ( (hash << 5) + hash ) + *str++; /* hash * 33 + c */

  return hash;
}