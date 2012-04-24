/*
 * PL/0 Compiler
 * Written by Adam Dunson
 * Filename: pl0-compiler.h
 *
 * Header file for the compiler driver.
 */

#ifndef PL0_COMPILER_H
#define PL0_COMPILER_H

#define DEBUG 0 // 0 = off, 1 = on

#define MAX_SYMBOL_TABLE_SIZE 100 // What is a reasonable amount here..?
#define MAX_CODE_LENGTH 1000 // 1000? I would certainly hope not

/*
 * For constants, you must store kind, name and value.
 * For variables, you must store kind, name, L and M.
 * For procedures, you must store kind, name, L and M.
 */

struct symbol {
  int kind;      // const = 1, var = 2, proc = 3
  char name[12]; // name up to 11 chars
  int val;       // number (ASCII value) 
  int level;     // L level
  int addr;      // M address
};
typedef struct symbol symbol;

typedef struct {
  int op;
  int l;
  int m;
} instruction;

extern const symbol EMPTY_SYMBOL;
extern symbol symbol_table[];
extern instruction code[];
extern int cx;

void print_code(FILE *output_file);
void print_code_pretty(FILE *output_file);
int symbol_hash(char *value, int level);

#endif
