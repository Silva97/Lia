/**
 * @file    cmd.h
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Header file for the command's system
 * @version 0.1
 * @date    2020-04-27
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#ifndef _LIA_CMD_H
#define _LIA_CMD_H

#include "lia/types.h"


#define CMDT cmd_arg_t[CMD_ARGC]   /**< For use in compound literals */
#define CMDNULL {0, 0}
#define CMDNOARG (CMDT){ CMDNULL, CMDNULL, CMDNULL }
#define OPT  operand_t[CMD_ARGC]   /**< For use in compound literals */
#define OPREG(x)  { .reg = x }
#define OPIMM(x)  { .imm = x }
#define OPPROC(x) { .procedure = x }
#define OPNULL    { .reg = 0 }


cmd_t *lia_cmd_new(cmd_t *tree, char *name, cmd_arg_t *args, char *body);
int isreg(token_t *tk);
int lia_cmd_compile(proc_t *proctree, FILE *output, cmd_t *cmd, operand_t *ops);

#endif /* _LIA_CMD_H */