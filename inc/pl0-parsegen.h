/*
 * PL/0 Compiler
 * Written by Adam Dunson
 * Filename: pl0-parsegen.h
 *
 * Code parser/generator header.
 */

#ifndef PL0_PARSEGEN_H
#define PL0_PARSEGEN_H

#include <stdio.h>
#include "pl0-tokens.h"

// This should match the number of things in the parse_errors[] array (see
// pl0-parsegen.c).
#define NUM_PARSE_ERRORS 30

extern const char *parse_errors[];

int pl0_parse(FILE *input_file, int a_flag);

int block(FILE *input_file, token_type *token);
int statement(FILE *input_file, token_type *token);
int condition(FILE *input_file, token_type *token);
int expression(FILE *input_file, token_type *token);
int term(FILE *input_file, token_type *token);
int factor(FILE *input_file, token_type *token);

token_type get_token(FILE *input_file);
symbol *get_symbol(FILE *input_file, int is_new);
int get_number(FILE *input_file);
const char *get_parse_error(int e);
int emit(int op, int l, int m);
void proc_cleanup();

#endif
