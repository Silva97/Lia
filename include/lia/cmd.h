#ifndef _LIA_CMD_H
#define _LIA_CMD_H

#define CMD_ARGC 3
#define CMDT cmd_arg_t[CMD_ARGC]   /**< For use in compound literals */
#define CMDNULL {0, 0}
#define CMDNOARG (CMDT){ CMDNULL, CMDNULL, CMDNULL }

typedef struct cmd_arg {
  int name;
  int type;
} cmd_arg_t;

/** Binary tree for commands */
typedef struct cmd {
  struct cmd *left;
  struct cmd *right;

  unsigned long int hashname;
  char *name;
  cmd_arg_t args[CMD_ARGC];
  char *body;
} cmd_t;


unsigned long int hash(unsigned char *str);
cmd_t *tree_insert(cmd_t *root, unsigned long int hashname);
cmd_t *tree_find(cmd_t *root, unsigned long int hashname);
cmd_t *lia_cmd_new(cmd_t *tree, char *name, cmd_arg_t *args, char *body);

#endif /* _LIA_CMD_H */