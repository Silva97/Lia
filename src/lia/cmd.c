/**
 * @file    cmd.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Commands to Lia language
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
 * @brief Search new space to insert the element
 * 
 * @param root      The root node
 * @param hashname  The hash for use in binary tree
 * @return cmd_t*   Pointer to the new element in the tree
 * @return NULL     If the element is repeated
 */
cmd_t *tree_insert(cmd_t *root, unsigned long int hashname)
{
  cmd_t *elem;

  if ( !root->hashname ) {
    root->hashname = hashname;
    return root;
  }
  
  if (root->hashname == hashname)
    return NULL;

  if (hashname > root->hashname) {
    if (root->right)
      return tree_insert(root->right, hashname);
    
    elem = calloc(1, sizeof *elem);
    elem->hashname = hashname;
    root->right = elem;
    return elem;
  } else if (root->left) {
    return tree_insert(root->left, hashname);
  }

  elem = calloc(1, sizeof *elem);
  elem->hashname = hashname;
  root->left = elem;
  return elem;
}

/**
 * @brief Find a element in the binary tree
 * 
 * @param root      The tree's root
 * @param hashname  The value to find
 * @return cmd_t*   Pointer to the element
 * @return NULL     If not found
 */
cmd_t *tree_find(cmd_t *root, unsigned long int hashname)
{
  cmd_t *elem;

  if ( !root )
    return NULL;

  if (root->hashname == hashname)
    return root;
  
  if (hashname > root->hashname)
    return tree_find(root->right, hashname);
  
  return tree_find(root->left, hashname);
}

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
cmd_t *lia_cmd_new(cmd_t *tree, char *name, const cmd_arg_t *args, char *body)
{
  unsigned long int hashname = hash(name);
  cmd_t *new = tree_insert(tree, hashname);

  if ( !new )
    return NULL;

  new->name = name;
  memcpy(new->args, args, sizeof *args * CMD_ARGC);
  new->body = body;
  
  return new;
}