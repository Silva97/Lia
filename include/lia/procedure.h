/**
 * @file    procedure.h
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Header file declaring the procedures's prototypes
 * @version 0.1
 * @date    2020-04-30
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#ifndef _LIA_PROCEDURE_H
#define _LIA_PROCEDURE_H

#include <stdio.h>
#include "lia/types.h"

/** First index of a procedure */
#define PROCINDEX 2

#define PROC_CALL1 "Pd.pD!$D!>"
#define PROC_CALL2 "=d.p=p*"

/** The number of instructions after $ */
#define PROC_CALLSIZE ( sizeof (PROC_CALL2) + 3 )


proc_t *proc_add(proc_t *root, char *name);
void proc_call(FILE *output, proc_t *proc);
void proc_ret(FILE *output, proc_t *proc);

#endif /* _LIA_PROCEDURE_H */