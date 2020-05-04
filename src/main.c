#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include "lia/lia.h"
#include "filepath.h"

#define DEF_OUT "out.ases"

void show_help(void);

int main(int argc, char **argv)
{
  int opt;
  bool pretty = false;
  char *outname = DEF_OUT;
  
  if (argc <= 1) {
    puts("Usage: lia [options] source1.lia source2.lia ...");
    return EXIT_SUCCESS;
  }
  
  lia_t *lia = calloc(1, sizeof *lia);
  lia->pathlist = calloc( 1, sizeof (path_t) );

  while ( (opt = getopt(argc, argv, "I:o:ph")) > 0 ) {
    switch (opt) {
    case 'I':
      path_insert(lia->pathlist, optarg);
      break;
    case 'o':
      outname = optarg;
      break;
    case 'p':
      pretty = true;
      break;
    case 'h':
      show_help();
      return EXIT_SUCCESS;
    }
  }

  FILE *input;

  for (int i = optind, count_in = 0; i < argc; i++) {
    if ( !strcmp(argv[i], "-") ) {
      if (count_in) {
        fprintf(stderr, "Error: Input file from stdin can't be repeated!\n");
        return EXIT_FAILURE;
      }

      input = stdin;
      count_in++;
    } else {
      input = fopen(argv[i], "r");

      if ( !input ) {
        fprintf(stderr, "Error: File '%s' not found.\n", argv[i]);
        return EXIT_FAILURE;
      }
    }

    lia_process(argv[i], input, lia);

    if (input != stdin)
      fclose(input);

    if (lia->errcount)
      return lia->errcount;
  }

  FILE *output = fopen(outname, "w");
  if ( !output ) {
    fprintf(stderr, "Error: The file '%s' could not be opened for writing.\n", outname);
    return EXIT_FAILURE;
  }

  lia_compiler(output, lia, pretty);
  return 0;
}


void show_help(void)
{
  puts(
    "Developed by Luiz Felipe (felipe.silva337@yahoo.com)\n"
    "GitHub: https://github.com/Silva97/lia\n\n"

    "Usage: lia [options] source1.lia source2.lia ...\n"
    "  -o     Specify the output name. (Default: \"" DEF_OUT "\")\n"
    "  -p     (pretty) If specified, adds comments to the output code.\n"
    "  -I     Define a path to search files in import. It's possible\n"
    "         use multiple times to set more paths.\n"
    "  -h     Show this help message."
  );
}