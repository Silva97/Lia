#ifndef _LIA_H
#define _LIA_H

#include "lia/types.h"
#include "lia/error.h"
#include "lia/lexer.h"
#include "lia/cmd.h"
#include "lia/parser.h"
#include "lia/procedure.h"

imp_t *lia_process(char *filename, FILE *input, lia_t *lia);

#endif /* _LIA_H */