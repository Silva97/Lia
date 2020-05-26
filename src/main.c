#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include "lia/lia.h"
#include "filepath.h"

#define DEF_OUT "out.ases"

#ifdef _WIN32
# define GETMOD(x) snprintf(x, sizeof x - 1, \
    "%s\\.lia\\modules", getenv("USERPROFILE"))
#else
# define GETMOD(x) snprintf(x, sizeof x - 1, \
    "%s/.lia/modules", getenv("HOME"))
#endif

int settarget(target_t *target, char *name);
void show_help(void);

int main(int argc, char **argv)
{
  char defmod[513];
  int opt;
  char *outname = DEF_OUT;
  target_t target = {
    .pretty = false,
    .name = "ases",
    .start = target_ases_start,
    .end = target_ases_end,
    .compile = target_ases_compile
  };
  
  if (argc <= 1) {
    puts(
      "Lia " LIA_TAG "\n"
      "Usage: lia [options] source1.lia source2.lia ..."
    );
    return EXIT_SUCCESS;
  }
  
  lia_t *lia = calloc(1, sizeof *lia);
  lia->pathlist = calloc( 1, sizeof (path_t) );
  lia->target = &target;


  GETMOD(defmod);
  path_insert(lia->pathlist, defmod);

  while ( (opt = getopt(argc, argv, "I:o:t:ph")) > 0 ) {
    switch (opt) {
    case 'I':
      path_insert(lia->pathlist, optarg);
      break;
    case 'o':
      outname = optarg;
      break;
    case 't':
      if ( !settarget(&target, optarg) ) {
        fprintf(stderr, "Target '%s' is invalid! See help: lia -h\n", optarg);
        return EXIT_FAILURE;
      }
      break;
    case 'p':
      target.pretty = true;
      break;
    case 'h':
    case '?':
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

  FILE *output;

  if ( !strcmp(outname, "-") )
    output = stdout;
  else
    output = fopen(outname, "w");

  if ( !output ) {
    fprintf(stderr, "Error: The file '%s' could not be opened for writing.\n", outname);
    return EXIT_FAILURE;
  }

  if ( lia_compiler(output, lia) ) {
    if (output != stdout) {
      fclose(output);
      remove(outname);
    }
    return lia->errcount;
  }

#ifndef _WIN32
  chmod(outname, S_IRWXU);
#endif

  return 0;
}


int settarget(target_t *target, char *name)
{
  static const char *list[] = {
    "ases",
    NULL
  };

  static void (*liststart[])() = {
    target_ases_start
  };

  static void (*listend[])() = {
    target_ases_end
  };

  static inst_t *(*listcompile[])() = {
    target_ases_compile
  };

  for (int i = 0; list[i]; i++) {
    if ( !strcmp(list[i], name) ) {
      target->start = liststart[i];
      target->end = listend[i];
      target->compile = listcompile[i];
      target->name = list[i];
      return true;
    }
  }

  return false;
}

void show_help(void)
{
  puts(
    "Lia " LIA_TAG "\n"
    "  Developed by Luiz Felipe (felipe.silva337@yahoo.com)\n"
    "  GitHub: https://github.com/Silva97/Lia\n\n"

    "Usage: lia [options] source1.lia source2.lia ...\n"
    "  -o     Specify the output name. (Default: \"" DEF_OUT "\")\n"
    "  -p     (pretty) If specified, adds comments to the output code.\n"
    "  -t     Specifies the output target.\n"
    "  -I     Define a path to search files in import. It's possible\n"
    "         use multiple times to set more paths.\n"
    "  -h     Show this help message.\n\n"

    "TARGETS\n"
    "  At this moment, only 'ases' target is supported."
  );
}
