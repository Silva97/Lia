#ifndef _LIA_TREE_H
#define _LIA_TREE_H

#define EXTENDS_TREE(strname) \
  struct strname *left;        \
  struct strname *right;      \
  unsigned long int hashname;  \
  char *name

typedef struct tree {
  EXTENDS_TREE(tree);
} tree_t;

void *tree_insert(void *tree, unsigned int size, unsigned long int hashname);
void *tree_find(void *tree, unsigned long int hashname);
void tree_free(void *tree);

#endif /* _LIA_TREE_H */