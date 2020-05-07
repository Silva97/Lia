#include <stdlib.h>
#include "metric.h"
#include "lia/lia.h"

#define TESTFILE  "tests/code.lia"

test_t test_lexer_code(void)
{
  /* [import "abc"]
   *
   * set  ss, 'a'
   * iadd ss, 5
   */
  const int seqtype[] = {
    TK_OPENBRACKET, TK_ID, TK_STRING, TK_CLOSEBRACKET, TK_SEPARATOR,
    TK_SEPARATOR,
    TK_ID, TK_ID, TK_COMMA, TK_CHAR, TK_SEPARATOR,
    TK_ID, TK_ID, TK_COMMA, TK_IMMEDIATE, TK_EOF
  };
  const int size = sizeof seqtype / sizeof *seqtype;
  const char types[][16] = {
    "EOF", "ID", "SEPARATOR", "OPENBRACKET", "CLOSEBRACKET",
    "COLON", "COMMA", "EQUAL", "IMMEDIATE", "CHAR", "STRING"
  };

  FILE *input = fopen(TESTFILE, "r");
  token_t *code = lia_lexer(TESTFILE, input);
  token_t *first = code;

  if ( !code )
    METRIC_TEST_FAIL("Lexer returned error");

  for (int i = 0; i < size; i++) {
    printf("%3d:%3d> type: %14s | value: %5d | text: \"%s\"\n",
      code->line,
      code->column,
      types[code->type],
      code->value,
      code->text);

    if (code->type != seqtype[i])
      METRIC_TEST_FAIL("Type not matched");
    
    
    code = code->next;
    if ( !code && i < size-1)
      METRIC_TEST_FAIL("Unexpected end of the code");
  }

  tkfree(first);
  METRIC_TEST_OK("");
}


int main(void)
{
  METRIC_TEST(test_lexer_code);

  METRIC_TEST_END();
  return metric_count_tests_fail;
}
