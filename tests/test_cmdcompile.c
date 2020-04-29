#include <stdio.h>
#include <stdlib.h>
#include "lia/cmd.h"
#include "metric.h"

test_t test_cmdcompile(void)
{
  int ret;
  cmd_t *tree = calloc(1, sizeof *tree);

  lia_cmd_new(tree, "add", (CMDT){ {'X', 'r'}, {'Y', 'r'}, CMDNULL }, "XaYb4");
  lia_cmd_new(tree, "set", (CMDT){ {'X', 'r'}, {'Y', 'i'}, CMDNULL }, "Yx");

  ret = lia_cmd_compile(stdout, tree_find(tree, hash("add")),
    (OPT){ OPREG("rb"), OPREG("ra"), OPNULL });
  
  if ( !ret )
    METRIC_TEST_FAIL("add instruction failed");

  ret = lia_cmd_compile(stdout, tree_find(tree, hash("set")),
    (OPT){ OPREG("rc"), OPIMM(29), OPNULL });
  
  if ( !ret )
    METRIC_TEST_FAIL("set instruction failed");
  
  tree_free(tree);
  putchar('\n');

  METRIC_TEST_OK("");
}

int main(void)
{
  METRIC_TEST(test_cmdcompile);

  METRIC_TEST_END();
  return metric_count_tests_fail;
}