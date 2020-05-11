#ifndef _TREE_H
#define _TREE_H

#define INITIAL_HASH 5381

/** Macro to expand basic struct elements of a tree */
#define EXTENDS_TREE(strname) \
  struct strname *left;        \
  struct strname *right;      \
  unsigned long int hashname;  \
  char *name

/** A basic tree */
typedef struct tree {
  EXTENDS_TREE(tree);
} tree_t;

unsigned long int hash(char *str);
void hashint(unsigned long int *output, int n);
void *tree_insert(void *tree, unsigned int size, unsigned long int hashname);
void *tree_find(void *tree, unsigned long int hashname);
void tree_free(void *tree);
void tree_map(void *tree, void (*mapper)(void *));

#endif /* _TREE_H */
