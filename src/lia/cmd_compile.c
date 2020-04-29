/**
 * @file    cmd_compile.c
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Source to the commands' compile system
 * @version 0.1
 * @date    2020-04-29
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 * 
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lia/cmd.h"

static int reg_compile(FILE *output, char *reg, int get)
{
  char result;
  if ( !strcmp(reg, "ss") )
    return 1;
  
  if ( !strcmp(reg, "ra") ) {
    result = 'a';
  } else if ( !strcmp(reg, "rb") ) {
    result = 'b';
  } else if ( !strcmp(reg, "rc") ) {
    result = 'c';
  } else if ( !strcmp(reg, "rd") ) {
    result = 'd';
  } else if ( !strcmp(reg, "dp") ) {
    result = 'p';
  } else {
    return 0;
  }

  if (get) {
    putc(toupper(result), output);
  } else {
    putc(result, output);
  }

  return 1;
}

static int imm_compile(FILE *output, uint8_t imm)
{
  putc('.', output);

  for (int i = 0; i < imm/10; i++)
    putc('6', output);
  
  for (int i = imm%10; i > 0; i--)
    putc('+', output);
}

/**
 * @brief Compile a command in the Ases code
 * 
 * @param output   The file to write the code
 * @param cmd      The command to compile
 * @param ops      The operands
 * @return >0      If all ok
 * @return 0       If error
 */
int lia_cmd_compile(FILE *output, cmd_t *cmd, operand_t *ops)
{
  int index;
  char *position;

  if (!cmd || !ops)
    return 0;

  char arglist[] = {
    tolower(cmd->args[0].name),
    tolower(cmd->args[1].name),
    tolower(cmd->args[2].name),
    0
  };

  for (int i = 0; cmd->body[i]; i++) {
    position = strchr( arglist, tolower(cmd->body[i]) );

    if (position) {
      index = (int) (position - arglist);

      switch (cmd->args[index].type) {
      case 'r':
        reg_compile( output, ops[index].reg, isupper(cmd->body[i]) );
        break;
      case 'i':
        imm_compile(output, ops[index].imm);
        break;
      default:
        return 0;
      }
    } else {
      putc(cmd->body[i], output);
    }
  }

  return 1;
}