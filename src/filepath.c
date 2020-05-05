/**
 * @file    filepath.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Functions to find files in determined path list
 * @version 0.1
 * @date    2020-05-02
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#include <stdio.h>
#include <stdlib.h>
#include "lia/types.h"
#include "filepath.h"

/**
 * @brief Finds a .lia file in the path list and open it
 * 
 * @param name      The file's name
 * @param mode      The mode to open the file
 * @param pathlist  The list of paths
 * @return FILE*    The file opened
 * @return NULL     If not found
 */
FILE *pfind(char *name, char *mode, path_t *pathlist)
{
  char fullname[513];
  FILE *file = NULL;

  while ( !file && pathlist && pathlist->text) {
    snprintf(fullname, sizeof fullname - 1, "%s" FP_BAR "%s.lia",
      pathlist->text, name);
    
    file = fopen(fullname, mode);

    pathlist = pathlist->next;
  }

  if ( !file ) {
    snprintf(fullname, sizeof fullname - 1, "%s.lia", name);
    file = fopen(fullname, mode);
  }

  return file;
}

/**
 * @brief Inserts a new item in a path's list
 * 
 * @param pathlist   The path's list
 * @param name       The file name to insert
 */
void path_insert(path_t *pathlist, char *name)
{
  path_t *this = pathlist;

  while (this->text && this->next)
    this = this->next;

  if ( !this->text ) {
    this->text = name;
    return;
  }

  this->next = calloc( 1, sizeof (path_t) );
  this->next->text = name;
}
