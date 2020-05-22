#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lia/lia.h"
#include "metric.h"

#define BASENAME "tests/compilation/test%u.lia"

#ifdef _WIN32
# define NULLFILE "nul"
#else
# define NULLFILE "/dev/null"
#endif

test_t test_compiler(void)
{
  int expected;
  static char comment[1025];
  char filename[513];
  FILE *input;
  lia_t *lia;
  target_t target = {
    .pretty = true,
    .name = "ases",
    .start = target_ases_start,
    .end = target_ases_end,
    .compile = target_ases_compile
  };

  FILE *output = fopen(NULLFILE, "w");

  for (unsigned int i = 1; i < 9999; i++) {
    snprintf(filename, sizeof filename - 1, BASENAME, i);
    input = fopen(filename, "r");
    if ( !input )
      break;
    
    expected = 0;
    if (fscanf(input, "# %d %1024[^\n]", &expected, comment) == 2)
      printf("* %s", comment);
    else
      printf("* %s", filename);
    
    fseek(input, 0, SEEK_SET);

    lia = calloc(1, sizeof *lia);
    lia->target = &target;
    lia_process(filename, input, lia);

    if (lia->errcount) {
      putchar('\n');
      METRIC_TEST_FAIL("Syntactic error");
    }

    lia_compiler(output, lia);

    printf(" | %d\n", lia->errcount);
    METRIC_ASSERT(lia->errcount == expected);

    lia_free(lia);
    metric_count_tests_ok++;
  }

  metric_count_tests_ok--;
  METRIC_TEST_OK("");
}

int main(void)
{
  METRIC_TEST(test_compiler);
  METRIC_TEST_END();

  return metric_count_tests_fail;
}
