#ifndef _LIA_PROCEDURE_H
#define _LIA_PROCEDURE_H

#include "tree.h"

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
} proc_t;


unsigned int proc_add(proc_t *root, char *name);
void proc_call(FILE *output, unsigned int index);
void proc_ret(FILE *output, unsigned int index);

#endif /* _LIA_PROCEDURE_H */