#include <stdio.h>
#include <stdlib.h>
#include "metric.h"
#include "lia/procedure.h"

test_t test_procedure(void)
{
  const int size = 5;
  int list[size];
  char names[][16] = {
    "test1",
    "function",
    "sum",
    "sqrt",
    "another"
  };
  proc_t *tree = calloc(1, sizeof *tree);

  for (int i = 0; i < size; i++) {
    list[i] = proc_add(tree, names[i]);
  }
  
  for (int i = 0; i < size; i++) {
    if (proc_add(tree, names[i]) != list[i])
      METRIC_TEST_FAIL("Index not match");
    
    printf("%d: ", list[i]);
    proc_call(stdout, list[i]);
    putchar('\n');

    fputs("   ", stdout);
    proc_ret(stdout, list[i]);
    putchar('\n');
  }

  METRIC_TEST_OK("");
}

int main(void)
{
  METRIC_TEST(test_procedure);

  METRIC_TEST_END();
  return metric_count_tests_fail;
}
