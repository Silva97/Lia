#include <stdio.h>
#include "lia/lexer.h"

int main(int argc, char **argv)
{
  FILE *input = fopen(argv[1], "r");
  token_t *code = lia_lexer(argv[1], input);

  return 0;
}