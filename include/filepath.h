#ifndef _FILEPATH_H
#define _FILEPATH_H

#include "lia/types.h"

#ifdef _WIN32
#define FP_BAR "\\"
#else
#define FP_BAR "/"
#endif


FILE *pfind(char *name, char *mode, path_t *pathlist);
void path_insert(path_t *pathlist, char *name);


#endif /* _FILEPATH_H */
