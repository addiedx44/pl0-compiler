/*
 * PL/0 Compiler
 * Written by Adam Dunson
 * Filename: lexeme_list.h
 *
 * Custom lexeme_list struct's header.
 */

#ifndef LEXEME_LIST_H
#define LEXEME_LIST_H

#include <stdio.h>
#include "pl0-tokens.h"

typedef struct lexeme_list {
  char *lex;
  token_type t;
  struct lexeme_list *next;
} lexeme_list;

lexeme_list *add_lexeme(lexeme_list *lexemes, char *lex, token_type t);
void destroy_lexemes(lexeme_list *lexemes);
void print_lexeme_table(lexeme_list *lexemes);
void print_lexeme_list(lexeme_list *lexemes);
void print_internal_lexeme_list(FILE *output_file, lexeme_list *lexemes);
void print_symbolic_internal_lexeme_list(FILE *output_file, lexeme_list *lexemes);
char *get_lexeme_list_str(lexeme_list *lexemes);

#endif
