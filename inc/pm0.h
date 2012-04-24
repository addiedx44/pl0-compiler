/*
 * PL/0 Compiler
 * Written by Adam Dunson
 * Filename: pm0.h
 *
 * PL/0 Virtual Machine header.
 *
 * Some enums for the op and opr codes.
 */

#ifndef PM0_H
#define PM0_H

// Not really sure about these values...
#define MAX_STACK_HEIGHT 2000
#define MAX_LEXI_LEVELS 10

// op codes
enum {
  LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SIO_OUT, SIO_IN
};

// opr m codes
enum {
  OPR_RET, OPR_NEG, OPR_ADD, OPR_SUB, OPR_MUL, OPR_DIV, OPR_ODD, OPR_MOD,
  OPR_EQL, OPR_NEQ, OPR_LSS, OPR_LEQ, OPR_GTR, OPR_GEQ
};

int pm0(FILE *input_file, int v_flag);
const char *get_op_code_symbol(int op);
const char *get_opr_symbol(int op);
int base(int *stack, int l, int bp);
void print_stack(int *stack, int sp, int *activation_records, int ar);

#endif
