/*
 * PL/0 Compiler
 * Written by Adam Dunson
 * Filename: pl0-compiler.c
 *
 * Main compiler driver code in here. Also some nifty globals and the
 * symbol_hash() function.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pl0-compiler.h"
#include "pl0-lex.h"
#include "pl0-parsegen.h"
#include "pl0-tokens.h"
#include "pm0.h"
#include "lexeme_list.h"
#include "fancy_string.h"

// gcc is stupid and wants me to add extra curly braces here
const symbol EMPTY_SYMBOL = {0};
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE] = {{0}};
instruction code[MAX_CODE_LENGTH] = {{0}};
int cx = 0;

int main(int argc, char **argv) {
  FILE *input_file = NULL;
  int l_flag = 0, a_flag = 0, v_flag = 0; // output flags

  if(argc > 1) {
    int i;
    for(i = 1; i < argc; i++) {
      // last arg must be the input_file
      if((i+1) == argc) {
        input_file = fopen(argv[i], "r");
        if(!input_file) {
          printf("File %s not found.\n", argv[1]);
          exit(EXIT_FAILURE);
        }
      } else {
        if(argv[i][0] == '-') {
          int j;
          for(j = 1; j < strlen(argv[i]); j++) {
            if(argv[i][j] == 'l') l_flag = 1;
            else if(argv[i][j] == 'a') a_flag = 1;
            else if(argv[i][j] == 'v') v_flag = 1;
            else {
              printf("Unknown option: %s\n", argv[i]);
              exit(EXIT_FAILURE);
            }
          }
        }
      }
    }
  } else {
    printf("Usage: pl0-compiler [-l] [-a] [-v] /path/to/input_file\n");
    exit(EXIT_FAILURE);
  }

  FILE *lexeme_file = tmpfile();

  pl0_lex(input_file, lexeme_file, l_flag);
  fclose(input_file);

  rewind(lexeme_file);

  int error_code = pl0_parse(lexeme_file, a_flag);
  fclose(lexeme_file);

  if(error_code == 0) {
    FILE *code_file = tmpfile();
    print_code(code_file);
    rewind(code_file);

    if(v_flag) {
      printf("Running in PM/0\n");
      printf("===============\n\n");
    }

    error_code = pm0(code_file, v_flag);

    if(v_flag) {
      printf("\n===============\n\n");
    }
    if(!error_code) {
      if(v_flag) {
        printf("Finished without error.\n");
      }
    } else {
      printf("Error number %d.\n", error_code);
    }

    fclose(code_file);
  } else {
    printf("Error number %d, %s\n", error_code, get_parse_error(error_code));
  }

  return EXIT_SUCCESS;
}

/**
 * Prints the raw generated code to a file.
 *
 * The file can be stdout.
 *
 * @param output_file the output file pointer
 */
void print_code(FILE *output_file) {
  int i;
  if(output_file == stdout) {
    printf("\n");
    printf("Generated Code (Raw)\n");
    printf("====================\n");
    printf("op  l  m\n");
    printf("--------\n");
  }
  for(i = 0; i < cx; i++) {
    fprintf(output_file, "%2d %2d %2d\n", code[i].op, code[i].l, code[i].m);
  }
  if(output_file == stdout) {
    printf("\n");
  }
}

/**
 * Prints the pretty generated code to a file.
 *
 * The file can be stdout.
 *
 * @param output_file the output file pointer
 */
void print_code_pretty(FILE *output_file) {
  int i;
  printf("\n");
  printf("Generated Code (Pretty)\n");
  printf("=======================\n");
  printf("  # | op   l       m\n");
  printf("--------------------\n");
  for(i = 0; i < cx; i++) {
    fprintf(output_file, "%3d | %s %2d ", i, get_op_code_symbol(code[i].op), code[i].l);
    if(code[i].op == OPR)
      fprintf(output_file, "%s\n", get_opr_symbol(code[i].m));
    else
      fprintf(output_file, "%7d\n", code[i].m);
  }
  printf("\n");
}

/**
 * Creates a hash for the given string.
 *
 * O(1) for no collisions, O(n) for collisions :D
 *
 * 1. Adds the first letter to the last letter in the string (or the first
 *    letter * 2).
 * 2. Multiplies that value by extra which is currently the value of
 *    'a' + 'd' + 'a' + 'm'.
 * 3. Modulo that value by MAX_SYMBOL_TABLE_SIZE (defined in pl0-compiler.c)
 *
 * @param value the string (char array) to be hashed
 * @return the integer value of the hash
 */
int symbol_hash(char *value, int level) {
  int hash = -1;
  int extra = 'a' + 'd' + 'a' + 'm';

  if(strlen(value) && level >= 0) {
    hash = abs(((value[0] + value[strlen(value) - 1]) * extra) % MAX_SYMBOL_TABLE_SIZE);
    if(DEBUG) printf("DEBUG: initial hash: %d\n", hash);

    // handle collisions where symbol_table[hash].name already exists
    int tmp = hash;
    while(strlen(symbol_table[hash].name) && (strcmp(value, symbol_table[hash].name) != 0 || (symbol_table[hash].kind != 1 && symbol_table[hash].level != level))) {
      hash = (hash + 1) % MAX_SYMBOL_TABLE_SIZE;
      if(tmp == hash)
        return -1;
    }
  }

  if(DEBUG) printf("DEBUG: final hash: %d\n", hash);
  return hash;
}
