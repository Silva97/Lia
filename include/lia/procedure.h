#ifndef _LIA_PROCEDURE_H
#define _LIA_PROCEDURE_H

#include <stdio.h>
#include "tree.h"
#include "lexer.h"

/** First index of a procedure */
#define PROCINDEX 2

#define PROC_CALL1 "Pd.pD!$D!>"
#define PROC_CALL2 "=d.p=p*"

/** The number of instructions after $ */
#define PROC_CALLSIZE ( sizeof (PROC_CALL2) + 3 )

/** Binary tree for procedures */
typedef struct proc {
  EXTENDS_TREE(proc);

  unsigned int index;
  token_t *body;
} proc_t;


proc_t *proc_add(proc_t *root, char *name);
void proc_call(FILE *output, proc_t *proc);
void proc_ret(FILE *output, proc_t *proc);

#endif /* _LIA_PROCEDURE_H */