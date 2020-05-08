#include <stdio.h>
#include <stdlib.h>
#include "lia/lia.h"
#include "metric.h"

#define TESTMACRO "tests/macro.lia"

static void macro_print(macro_t *macro)
{
  printf("%s( ", macro->name);

  for (mtk_t *this = macro->tkseq; this; this = this->next) {
    printf("%d ", this->type);
  }

  fputs(") = ", stdout);

  for (token_t *this = macro->body; this; this = this->next) {
    printf("%s ", this->text);
  }

  putchar('\n');
}

static void tree_print(macro_t *tree, unsigned int level)
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

  macro_print(tree);
  
  if (tree->left)
    tree_print(tree->left, level + lv);
}



test_t test_macros(void)
{
  FILE *input = fopen(TESTMACRO, "r");
  lia_t *lia = calloc(1, sizeof *lia);

  if ( !input )
    METRIC_TEST_FAIL("Test file not found.");

  lia_process(TESTMACRO, input, lia);
  tree_print(lia->macrotree, 0);

  METRIC_ASSERT(lia->errcount == 2);
  METRIC_TEST_OK("");
}

int main(void)
{
  METRIC_TEST(test_macros);

  METRIC_TEST_END();
  return metric_count_tests_fail;
}
