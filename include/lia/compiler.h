/**
 * @file    compiler.h
 * @author  Luiz Felipe (felipe.silva337@yahoo.com)
 * @brief   Header file declaring the compiler's prototypes
 * @version 0.1
 * @date    2020-05-04
 * 
 * @copyright Copyright (c) 2020 Luiz Felipe
 */
#ifndef _LIA_COMPILER_H
#define _LIA_COMPILER_H

imp_t *lia_process(char *filename, FILE *input, lia_t *lia);
int lia_compiler(FILE *output, lia_t *lia, int pretty);
int lia_inst_compile(FILE *output, inst_t *inst, lia_t *lia, int pretty);

#endif /* _LIA_COMPILER_H */