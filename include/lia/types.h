/**
 * @file    types.h
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Header file declaring types used in Lia
 * @version 0.1
 * @date    2020-05-01
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#ifndef _LIA_TYPES_H
#define _LIA_TYPES_H

#include <inttypes.h>
#include "tree.h"

/** Token maximum size + 1 */
#define TKMAX 129

/** Number of arguments to a command */
#define CMD_ARGC 3

/** List of register's names in Ases */
#define REGLIST "abcdp"

/** The arguments of the keyword's function */
#define KEY_ARGS  token_t *tk, imp_t *file, lia_t *lia


/** Enumeration to token's type. */
typedef enum token_type {
  TK_EOF,
  TK_ID,
  TK_SEPARATOR,
  TK_OPENBRACKET,
  TK_CLOSEBRACKET,
  TK_COLON,
  TK_COMMA,
  TK_EQUAL,
  TK_IMMEDIATE,
  TK_CHAR,
  TK_STRING
} token_type_t;

/** Structure of a token */
typedef struct token {
  struct token *next;
  struct token *last;
  int line;
  int column;

  token_type_t type;
  char text[TKMAX];
  uint8_t value;
} token_t;


/** Argument to a command */
typedef struct cmd_arg {
  int name;
  int type;
} cmd_arg_t;

/** Binary tree for commands */
typedef struct cmd {
  EXTENDS_TREE(cmd);

  cmd_arg_t args[CMD_ARGC];
  unsigned int argc;
  char *body;
} cmd_t;

/** Operand's union */
typedef union operand {
  char reg[5];
  uint8_t imm;
  char *procedure;
} operand_t;


typedef struct import {
  EXTENDS_TREE(import);
  char filename[TKMAX];
  FILE *input;

  token_t *tklist; /**< The tokens' list */
} imp_t;

/** Type of a instruction */
typedef enum inst_type {
  INST_FUNC,
  INST_LOAD,
  INST_STORE,
  INST_PUSH,
  INST_POP,
  INST_CALL,
  INST_RET,
  INST_PROC,
  INST_ENDPROC,
  INST_IF,
  INST_IFBLOCK,
  INST_ENDIF,
  INST_CMD
} inst_type_t;

/** Instructions' list generated by parser */
typedef struct inst {
  struct inst *next;
  struct inst *last;
  token_t *child;
  imp_t *file;

  inst_type_t type;
} inst_t;

/** Binary tree for procedures */
typedef struct proc {
  EXTENDS_TREE(proc);

  unsigned int index;
  inst_t *body;
} proc_t;

/** Path's list to search imported files */
typedef struct path {
  struct path *next;
  char *text;
} path_t;

/** Context stack to blocks instructions. */
typedef struct ctx {
  struct ctx *last;

  inst_t *start;
  inst_type_t endtype;
} ctx_t;

/** A Lia's struct reserving all informations about a code */
typedef struct lia {
  proc_t *proctree;  /**< The procedures' tree */
  cmd_t *cmdtree;    /**< The commands' tree */
  imp_t *imptree;    /**< The imports' tree */
  inst_t *instlist;  /**< The instructions' list */
  path_t *pathlist;  /**< The paths' list */
  
  ctx_t *ctx;        /**< Context for blocks instructions. */
  proc_t *inproc;    /**< Define context inside a procedure. */
  token_t *thisproc;
  unsigned int errcount;
} lia_t;


/** Meta-keywords */
typedef enum metakeyword {
  META_NONE,
  META_NEW,
  META_IMPORT
} metakeyword_t;

/** Keywords */
typedef enum keyword {
  KEY_NONE,
  KEY_FUNC,
  KEY_LOAD,
  KEY_STORE,
  KEY_PUSH,
  KEY_POP,
  KEY_CALL,
  KEY_RET,
  KEY_PROC,
  KEY_ENDPROC,
  KEY_PRTAB,
  KEY_IF,
  KEY_ENDIF
} keyword_t;

#endif /* _LIA_TYPES_H */