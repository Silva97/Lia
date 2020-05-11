#include <stdio.h>
#include <stdlib.h>
#include "lia/lia.h"
#include "metric.h"

#define TESTMACRO "tests/macro.lia"

static void macro_print(macro_t *macro)
{
  printf("%s ->\n", macro->name);
  tree_map(macro->variants, macro_seq_print);
}

static void tree_print(macro_t *tree)
{
  if (tree->right)
    tree_print(tree->right);

  macro_print(tree);
  
  if (tree->left)
    tree_print(tree->left);
}



test_t test_macros(void)
{
  FILE *input = fopen(TESTMACRO, "r");
  lia_t *lia = calloc(1, sizeof *lia);

  if ( !input )
    METRIC_TEST_FAIL("Test file not found.");

  lia_process(TESTMACRO, input, lia);

  puts("--------------");
  tree_print(lia->macrotree);

  METRIC_ASSERT(lia->errcount == 2);
  METRIC_TEST_OK("");
}

int main(void)
{
  METRIC_TEST(test_macros);

  METRIC_TEST_END();
  return metric_count_tests_fail;
}
