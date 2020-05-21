#ifndef _LIA_TARGET_H
#define _LIA_TARGET_H

#include "lia/types.h"

#define ARGTARGET FILE *output, lia_t *lia, struct target *target
#define ARGCOMPILE FILE *output, inst_t *inst, lia_t *lia, struct target *target

/** Target to generate final code */
typedef struct target {
  int pretty;

  /** Initializes the code */
  void (*start)(ARGTARGET);
  void (*end)(ARGTARGET);
  
  /** Compiles one instruction */
  inst_t *(*compile)(ARGCOMPILE);
} target_t;


void target_ases_start(ARGTARGET);
void target_ases_end(ARGTARGET);
inst_t *target_ases_compile(ARGCOMPILE);

#endif /* _LIA_TARGET_H */
