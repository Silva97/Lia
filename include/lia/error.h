#ifndef _LIA_ERROR_H
#define _LIA_ERROR_H

#ifdef __linux__

#define lia_error(file, line, col, fmt, ...)    \
  fprintf(stderr, "\x1b[37;1m%s"                     \
    ": \x1b[31;1merror\x1b[0m at %d:%d> " fmt "\n",   \
    file, line, col, __VA_ARGS__)

#else

#define lia_error(file, line, col, fmt, ...)   \
  fprintf(stderr, "%s: error at %d:%d> " fmt "\n",  \
    file, line, col, __VA_ARGS__)

#endif

#endif /* _LIA_ERROR_H */
