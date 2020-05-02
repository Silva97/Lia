#include <stdio.h>
#include <stdlib.h>
#include "lia/lia.h"
#include "metric.h"

#define TESTFILE "tests/import.lia"
#define TESTINST "tests/instruction.lia"

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


test_t test_parser(void)
{
  FILE *input = fopen(TESTFILE, "r");
  lia_t *lia = calloc(1, sizeof *lia);
  
  lia_process(TESTFILE, input, lia);

  tree_print(lia->cmdtree, 0);

  if (lia->errcount != 2) {
    METRIC_TEST_FAIL("Error in parsing the code");
  }

  METRIC_TEST_OK("Two errors expected");
}

test_t test_instlist(void)
{
  FILE *input = fopen(TESTINST, "r");
  lia_t *lia = calloc(1, sizeof *lia);
  
  lia_process(TESTINST, input, lia);

  inst_t *this = lia->instlist;
  while (this) {
    printf("%d = %s ", this->type, this->child->text);

    for (token_t *tk = this->child; tk = tk->next;)
      printf("%s ", tk->text);

    putchar('\n');
    this = this->next;
  }

  if (lia->errcount != 4)
    METRIC_TEST_FAIL("Error on instruction list");

  METRIC_TEST_OK("Four errors expected");
}

int main(void)
{
  METRIC_TEST(test_parser);
  METRIC_TEST(test_instlist);

  METRIC_TEST_END();
  return metric_count_tests_fail;
}