#include <stdlib.h>
#include "tree.h"

/**
 * @brief Search new space to insert the element
 * 
 * @param tree      The root node
 * @param hashname  The hash for use in binary tree
 * @return void*    Pointer to the new element in the tree
 * @return NULL     If the element is repeated
 */
void *tree_insert(void *tree, unsigned int size, unsigned long int hashname)
{
  tree_t *elem;
  tree_t *root = tree;

  if ( !root->hashname ) {
    root->hashname = hashname;
    return root;
  }
  
  if (root->hashname == hashname)
    return NULL;

  if (hashname > root->hashname) {
    if (root->right)
      return tree_insert(root->right, size, hashname);
    
    elem = calloc(1, size);
    elem->hashname = hashname;
    root->right = elem;
    return elem;
  } else if (root->left) {
    return tree_insert(root->left, size, hashname);
  }

  elem = calloc(1, size);
  elem->hashname = hashname;
  root->left = elem;
  return elem;
}

/**
 * @brief Find a element in the binary tree
 * 
 * @param tree      The tree's root
 * @param hashname  The value to find
 * @return void*    Pointer to the element
 * @return NULL     If not found
 */
void *tree_find(void *tree, unsigned long int hashname)
{
  if ( !tree )
    return NULL;
  
  tree_t *root = tree;
  if ( !root->hashname )
    return NULL;

  if (root->hashname == hashname)
    return root;
  
  if (hashname > root->hashname)
    return tree_find(root->right, hashname);
  
  return tree_find(root->left, hashname);
}

/**
 * @brief Free all the tree
 * 
 * @param tree  The root element
 */
void tree_free(void *tree)
{
  if ( !tree )
    return;
  
  tree_t *root = tree;
  
  if (root->left)
    tree_free(root->left);

  if (root->right)
    tree_free(root->right);
  
  free(root);
}

/**
 * @brief Calls a function in all tree elements
 * 
 * @param tree    The tree to map
 * @param mapper  The callee function.
 */
void tree_map(void *tree, void (*mapper)(void *))
{
  if ( !tree || !mapper )
    return;
  
  tree_t *root = tree;

  if (root->left)
    tree_map(root->left, mapper);
  
  mapper(root);

  if (root->right)
    tree_map(root->right, mapper);
}
