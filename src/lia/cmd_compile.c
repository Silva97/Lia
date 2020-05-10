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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lia/lia.h"

/**
 * @brief Generate the code to get or set a register
 * 
 * @param output   The file to write
 * @param reg      The register's name
 * @param get      0 to set, nonzero to get
 * @return 0       If register not exists
 * @return nonzero If all ok
 */
int reg_compile(FILE *output, char *reg, int get)
{
  char result;
  if ( !strcmp(reg, "ss") )
    return 1;
  
  if ( !strcmp(reg, "dp") ) {
    result = 'p';
  } else if (reg[0] == 'r'){
    result = reg[1];
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

/**
 * @brief Generate code to set a immediate value
 * 
 * @param output   The file to write the instruction
 * @param imm      The value
 */
void imm_compile(FILE *output, uint8_t imm)
{
  putc('.', output);

  for (int i = 0; i < imm/10; i++)
    putc('6', output);
  

  unsigned int total = imm%10;

  if (total > 5) {
    putc('6', output);

    for (int i = 10 - total; i > 0; i--)
      putc('-', output);
  } else {
    for (int i = total; i > 0; i--)
      putc('+', output);
  }
}

/**
 * @brief Compiles a string at a sequence of output
 * 
 * @param filename  The filename.
 * @param output    The file to writes the output.
 * @param tk        The token to reads the text.
 * @return token_t* Last string token compiled.
 * @return NULL     If error.
 */
token_t *str_compile(char *filename, FILE *output, token_t *tk)
{
  int index;
  int diff;
  int chone[] = {'+', '-'};
  int chten[] = {'6', '7'};
  int last = 0;
  putc('.', output);

  while (tk->type == TK_STRING) {
    for (int ch, i = 0; tk->text[i]; i++) {
      if (tk->text[i] == '\\') {
        i++;
        ch = chresc(tk->text[i]);
        if (ch < 0) {
          lia_error(filename, tk->line, tk->column + i + 1,
            "Invalid escape '\\%c' at string.", tk->text[i]);

          return NULL;
        }
      } else {
        ch = tk->text[i];
      }

      diff = abs(last - ch);
      if ( !diff ) {
        putc('1', output);
        continue;
      }

      index = (last > ch);
      for (int i = 0; i < diff/10; i++)
        putc(chten[index], output);
      
      unsigned int total = diff%10;
      if (total > 5) {
        putc(chten[index], output);

        for (int i = 10 - total; i > 0; i--)
          putc(chone[ !index ], output);
      } else {
        for (int i = total; i > 0; i--)
          putc(chone[index], output);
      }
    
      putc('1', output);
      last = ch;
    }

    if ( !tk->next )
      break;

    tk = metanext(tk);
  }

  return tk;
}

/**
 * @brief Verify if a token is a valid register name
 * 
 * @param tk    The token to verify
 * @return int  0 if not, nonzero if yes
 */
int isreg(token_t *tk)
{
  if (tk->type != TK_ID)
    return 0;
  
  if ( !strcmp(tk->text, "ss") )
    return 1;
  if ( !strcmp(tk->text, "dp") )
    return 1;
  
  if (tk->text[0] == 'r') {
    if (tk->text[1] >= 'a' && tk->text[1] <= 'l' && !tk->text[2])
      return 1;
  }

  return 0;
}

/**
 * @brief Compile a command in the Ases code
 * 
 * @param proctree The tree of procedures
 * @param output   The file to write the code
 * @param cmd      The command to compile
 * @param ops      The operands
 * @return nonzero If all ok
 * @return 0       If error
 */
int lia_cmd_compile(proc_t *proctree, char *filename, FILE *output, cmd_t *cmd, operand_t *ops)
{
  token_t *tk;
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

  for (tk = cmd->body; tk && tk->type == TK_STRING; tk = metanext(tk)) {
    for (int i = 0; tk->text[i]; i++) {
      position = strchr( arglist, tolower(tk->text[i]) );

      if (position) {
        index = (int) (position - arglist);

        switch (cmd->args[index].type) {
        case 'r':
          reg_compile( output, ops[index].reg, isupper(tk->text[i]) );
          break;
        case 'i':
          imm_compile(output, ops[index].imm);
          break;
        case 'p':
          proc_call( output, proc_add(proctree, ops[index].procedure) );
          break;
        case 's':
          str_compile(filename, output, ops[index].string);
          break;
        default:
          return 0;
        }
      } else {
        putc(tk->text[i], output);
      }
    }
  }

  return 1;
}
