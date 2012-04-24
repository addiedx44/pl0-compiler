/*
 * PL/0 Compiler
 * Written by Adam Dunson
 * Filename: pm0.c
 *
 * The PL/0 Virtual Machine. The v_flag controls what to output.
 */

#include <stdio.h>
#include <stdlib.h>

#include "pl0-compiler.h"
#include "pm0.h"

/**
 * The main function for the virtual machine.
 *
 * @param input_file the input_file to read from (should be in PL/0 assembly)
 */
int pm0(FILE *input_file, int v_flag) {
  instruction code[MAX_CODE_LENGTH];
  int stack[MAX_STACK_HEIGHT];
  int activation_records[MAX_LEXI_LEVELS + 1]; // accounts for MAIN + 3 more levels

  int sp = 0, bp = 1, pc = 0;
  instruction *ir = code;
  int i = 0, i_cnt = 0;
  int ar = 0; // current activation record
  int sio_print = 0;
  int sio_scan = 0;

  /* some initialization */
  stack[0] = 0;
  stack[1] = 0;
  stack[2] = 0;

  for(i = 0; i <= MAX_LEXI_LEVELS; i++) {
    activation_records[i] = -1;
  }
  /* end initialization */

  i = 0;
  while(!feof(input_file))
    if(fscanf(input_file, "%d %d %d", &code[i].op, &code[i].l, &code[i].m) == 3)
      i++;
  i_cnt = i;
  /* done parsing the file */

  /* begin execution */
  if(v_flag) {
    printf("LINE   OP    L    M      PC   BP   SP    Stack\n");
    printf("--------------------------------------------------------------------\n");
    printf("Initial values:          %2d   %2d   %2d    (initialized to all zeroes)\n", pc, bp, sp);
    printf("--------------------------------------------------------------------\n");
  }

  while(pc < i_cnt && ar >= 0) {
    /* begin fetch */
    ir = &code[pc];
    if(v_flag) printf("%4d ", pc);
    pc++;
    /* end fetch */

    /* execute */
    switch(ir->op) {
      case 1:
        // lit
        stack[sp] = ir->m;
        sp++;
        break;
      case 2:
        // opr
        switch(ir->m) {
          case 0:
            // ret
            sp = bp - 1;
            bp = stack[sp + 1];
            pc = stack[sp + 2];

            activation_records[ar] = -1;
            ar--;
            break;
          case 1:
            // neg
            stack[sp - 1] = -stack[sp - 1];
            break;
          case 2:
            // add
            sp--;
            stack[sp - 1] =  stack[sp - 1] + stack[sp];
            break;
          case 3:
            // sub
            sp--;
            stack[sp - 1] = stack[sp - 1] - stack[sp];
            break;
          case 4:
            // mul
            sp--;
            stack[sp - 1] = stack[sp - 1] * stack[sp];
            break;
          case 5:
            // div
            sp--;
            stack[sp - 1] = stack[sp - 1] / stack[sp];
            break;
          case 6:
            // odd
            stack[sp - 1] = stack[sp - 1] % 2;
            break;
          case 7:
            // mod
            sp--;
            stack[sp - 1] = stack[sp - 1] % stack[sp];
            break;
          case 8:
            // eql
            sp--;
            stack[sp - 1] = stack[sp - 1] == stack[sp];
            break;
          case 9:
            // neq
            sp--;
            stack[sp - 1] = stack[sp - 1] != stack[sp];
            break;
          case 10:
            // lss
            sp--;
            stack[sp - 1] = stack[sp - 1] < stack[sp];
            break;
          case 11:
            // leq
            sp--;
            stack[sp - 1] = stack[sp - 1] <= stack[sp];
            break;
          case 12:
            // gtr
            sp--;
            stack[sp - 1] = stack[sp - 1] > stack[sp];
            break;
          case 13:
            // geq
            sp--;
            stack[sp - 1] = stack[sp - 1] >= stack[sp];
            break;
        }
        break;
      case 3:
        // lod
        stack[sp] = stack[base(stack, ir->l, bp) - 1 + ir->m];
        sp++;
        break;
      case 4:
        // sto
        sp--;
        stack[base(stack, ir->l, bp) - 1 + ir->m] = stack[sp];
        break;
      case 5:
        // cal
        stack[sp] = base(stack, ir->l, bp); // static link (SL)
        stack[sp + 1] = bp; // dynamic link (DL)
        stack[sp + 2] = pc; // return address (RA)
        bp = sp + 1;
        pc = ir->m;

        activation_records[ar] = sp;
        for(i = (ar - 1); i >= 0; i--) {
          activation_records[ar] -= activation_records[i];
        }
        ar++;
        activation_records[ar] = 3;
        break;
      case 6:
        // inc
        sp += ir->m;
        break;
      case 7:
        // jmp
        pc = ir->m;
        break;
      case 8:
        // jpc
        sp--;
        if(stack[sp] == 0)
          pc = ir->m;
        break;
      case 9:
        // sio
        sp--;
        sio_print = 1;
        break;
      case 10:
        // sio
        sio_scan = 1;
        break;
    }
    /* end execute */

    if(v_flag) {
      printf("%4s %4d %4d      %2d %4d %4d   ", get_op_code_symbol(ir->op), ir->l, ir->m, pc, bp, sp);
      print_stack(stack, sp, activation_records, ar);
      printf("\n");
    }

    if(sio_print) {
      if(v_flag) {
        printf("\n");
        printf("-----------\n");
      }

      printf("Output: %d\n", stack[sp]);

      if(v_flag) {
        printf("-----------\n");
        printf("\n");
      }
      sio_print = 0;
    }

    if(sio_scan) {
      if(v_flag) {
        printf("\n");
        printf("-----------\n");
      }

      printf("Input: ");
      scanf("%d", &stack[sp]);
      sp++;

      // hack to flush input buffer in case user was stupid
      int ch;
      while(!feof(stdin) && (ch = getchar()) != '\n' && ch != EOF);

      if(v_flag) {
        printf("-----------\n");
        printf("\n");
      }

      sio_scan = 0;
    }
  }
  /* end execution */

  return EXIT_SUCCESS;
}

/**
 * Return the string representation of an op code.
 *
 * @param op a PM/0 op code
 * @return a (const char *) string containing the string representation of an op code
 */
const char *get_op_code_symbol(int op) {
  switch(op) {
    case 1:
      // lit
      return "LIT";
      break;
    case 2:
      // opr
      return "OPR";
      break;
    case 3:
      // lod
      return "LOD";
      break;
    case 4:
      // sto
      return "STO";
      break;
    case 5:
      // cal
      return "CAL";
      break;
    case 6:
      // inc
      return "INC";
      break;
    case 7:
      // jmp
      return "JMP";
      break;
    case 8:
      // jpc
      return "JPC";
      break;
    case 9:
      // sio
      return "SIO";
      break;
    case 10:
      // sio
      return "SIO";
      break;
  }
  return "ERR";
}

/**
 * Return the string representation of an OPR instruction.
 *
 * @param op a PM/0 OPR instruction (instruction.m)
 * @return a (const char *) string containing the string representation of an OPR instruction
 */
const char *get_opr_symbol(int op) {
  switch(op) {
    case 0:
      // ret
      return "OPR_RET";
      break;
    case 1:
      // neg
      return "OPR_NEG";
      break;
    case 2:
      // add
      return "OPR_ADD";
      break;
    case 3:
      // sub
      return "OPR_SUB";
      break;
    case 4:
      // mul
      return "OPR_MUL";
      break;
    case 5:
      // div
      return "OPR_DIV";
      break;
    case 6:
      // odd
      return "OPR_ODD";
      break;
    case 7:
      // mod
      return "OPR_MOD";
      break;
    case 8:
      // eql
      return "OPR_EQL";
      break;
    case 9:
      // neq
      return "OPR_NEQ";
      break;
    case 10:
      // lss
      return "OPR_LSS";
      break;
    case 11:
      // leq
      return "OPR_LEQ";
      break;
    case 12:
      // gtr
      return "OPR_GTR";
      break;
    case 13:
      // geq
      return "OPR_GEQ";
      break;
  }
  return "ERR";
}

/**
 * Return the base of an activation record L levels down.
 *
 * @param stack the stack array
 * @param l the current instruction's L value
 * @param bp the current BP
 * @return the base of the activation record L levels down
 */
int base(int *stack, int l, int bp) {
  int base = bp;
  while(l > 0) {
    base = stack[base - 1];
    l--;
  }
  return base;
}

/**
 * Output a string representation of the stack.
 *
 * @param stack the stack array
 * @param sp the current SP
 * @param activation_records an array containing the size of each activation record
 * @param ar the current activation record index
 */
void print_stack(int *stack, int sp, int *activation_records, int ar) {
  int i, j, s = 0;
  if(ar != 0) {
    for(i = 0; i <= ar; i++) {
      for(j = 0; j < activation_records[i]; j++) {
        printf("%2d", stack[s++]);
        if(j + 1 < activation_records[i])
          printf(" ");
      }
      if(i + 1 <= ar)
        printf("  | ");
    }
  }

  if(s < sp && ar != 0)
    printf(" ");

  while(s < sp) {
    printf("%2d", stack[s++]);

    if(s < sp)
      printf(" ");
  }
}
