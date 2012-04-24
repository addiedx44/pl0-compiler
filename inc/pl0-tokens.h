/*
 * PL/0 Compiler
 * Written by Adam Dunson
 * Filename: pl0-tokens.h
 *
 * Token header. Here be the token_type enum.
 */

#ifndef PL0_TOKENS_H
#define PL0_TOKENS_H

#define NUM_RESERVED_WORDS 13
#define NUM_SPECIAL_SYMBOLS 13
#define NUM_INVISIBLE_CHARS 4

typedef enum {
  nulsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym,
  oddsym, eqsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym,
  rparentsym, commasym, semicolonsym, periodsym, becomessym, beginsym, endsym,
  ifsym, thensym, whilesym, dosym, callsym, constsym, intsym, procsym, outsym,
  insym, elsesym
} token_type;

extern const char *token_symbols[];
extern const char *reserved_words[];
extern const char special_symbols[];
extern const char invisible_characters[];

token_type get_reserved_word_sym(char *str);
token_type get_special_symbol_sym (char *str);
const char *get_token_symbol(token_type t);
int is_reserved_word(char *str);
int is_letter(char c);
int is_digit(char c);
int is_special_symbol(char c);
int is_invisible_character(char c);
int is_relation(token_type token);

#endif
