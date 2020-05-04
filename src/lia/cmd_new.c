/**
 * @file    cmd_new.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Commands' create system to Lia language
 * @version 0.1
 * @date    2020-04-27
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lia/cmd.h"

/**
 * @brief Inserts a new instruction in the tree.
 * 
 * @param tree     The tree root of commands
 * @param name     Name of the new command
 * @param args     Array of arguments
 * @param body     Body of the command
 * @return cmd_t*  Pointer to the new element if all ok
 * @return NULL    If the element already exists
 */
cmd_t *lia_cmd_new(cmd_t *tree, char *name, cmd_arg_t *args, char *body)
{
  unsigned long int hashname = hash(name);
  cmd_t *new = tree_insert(tree, sizeof *new, hashname);

  if ( !new )
    return NULL;

  new->name = name;
  memcpy(new->args, args, sizeof *args * CMD_ARGC);
  new->body = body;

  new->argc = 0;
  while (new->argc < CMD_ARGC && args[new->argc].name)
    new->argc++;
  
  return new;
}