#include <stdio.h>
#include <stdlib.h>
#include "metric.h"
#include "lia/cmd.h"

static void cmd_print(cmd_t *cmd)
{
  printf("%s ", cmd->name);
  
  for (int i = 0; i < CMD_ARGC; i++)
    printf("%c:%c ", cmd->args[i].name, cmd->args[i].type);
  
  printf("= \"%s\"\n", cmd->body);
}

static void tree_print(cmd_t *tree, unsigned int level)
{
  const int lv = 4;
  if (tree->right)
    tree_print(tree->right, level + lv);

  
  if (level > lv) {
    printf("%*c+", level - lv + 2, '\0');
    for (int i = 0; i < lv - 1; i++)
      putchar('-');
    
    putchar(' ');
  } else if (level == lv) {
    fputs(" +--- ", stdout);
  } else {
    fputs("+ ", stdout);
  }

  cmd_print(tree);
  
  if (tree->left)
    tree_print(tree->left, level + lv);
}


test_t test_cmdtree(void)
{
  cmd_t *tree = calloc(1, sizeof *tree);

  lia_cmd_new(tree, "add",  (CMDT){ {'X', 'r'}, {'Y', 'r'}, {0, 0} }, "XaY4");
  lia_cmd_new(tree, "iadd", (CMDT){ {'X', 'r'}, {'Y', 'i'}, {0, 0} }, "XaY4");
  lia_cmd_new(tree, "sub",  (CMDT){ {'X', 'r'}, {'Y', 'r'}, {0, 0} }, "XaY5");
  lia_cmd_new(tree, "isub", (CMDT){ {'X', 'r'}, {'Y', 'i'}, {0, 0} }, "XaY5");

  lia_cmd_new(tree, "xxx", (CMDT){ {'X', 'r'}, {'Y', 'i'}, {0, 0} }, "XXXX");
  lia_cmd_new(tree, "yyy", (CMDT){ {'X', 'r'}, {'Y', 'i'}, {0, 0} }, "YYYY");
  lia_cmd_new(tree, "zzz", (CMDT){ {'X', 'r'}, {'Y', 'i'}, {0, 0} }, "ZZZZ");

  tree_print(tree, 0);

  cmd_t *tst = tree->right;

  if ( tst->hashname <= tst->left->hashname )
    METRIC_TEST_FAIL("Left field is greater than his parent");
  
  if ( tst->hashname >= tst->right->hashname )
    METRIC_TEST_FAIL("Right field is less than his parent");

  puts("---------------");

  cmd_t *find = tree_find(tree, hash("isub"));

  if ( !find )
    METRIC_TEST_FAIL("Element not found");
  
  if ( strcmp(find->name, "isub") )
    METRIC_TEST_FAIL("Element not match correct");
  
  cmd_print(find);
  tree_free(tree);
  
  METRIC_TEST_OK("");
}

int main(void)
{
  METRIC_TEST(test_cmdtree);

  METRIC_TEST_END();
  return metric_count_tests_fail;
}