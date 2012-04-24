/*
 * PL/0 Compiler
 * Written by Adam Dunson
 * Filename: pl0-tokens.c
 *
 * Defines some useful globals and functions for dealing with tokens and
 * reserved words.
 */

#include <stdio.h>
#include <string.h>
#include "pl0-tokens.h"

const char *token_symbols[] = {
  "", "nulsym", "identsym", "numbersym", "plussym", "minussym", "multsym",
  "slashsym", "oddsym", "eqsym", "neqsym", "lessym", "leqsym", "gtrsym",
  "geqsym", "lparentsym", "rparentsym", "commasym", "semicolonsym",
  "periodsym", "becomessym", "beginsym", "endsym", "ifsym", "thensym",
  "whilesym", "dosym", "callsym", "constsym", "intsym", "procsym", "outsym",
  "insym", "elsesym"
};
const char *reserved_words[] = {
  "begin", "end", "if", "then", "while", "do", "call", "const", "int",
  "procedure", "out", "in", "else"
};
const char special_symbols[] = {
  '+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':'
};
const char invisible_characters[] = { 32, '\t', '\n', '\r' };


/**
 * Returns the string version of a numeric token.
 *
 * @param t the token_type
 */
const char *get_token_symbol(token_type t) {
  return token_symbols[(int)t];
}

/**
 * Returns the token_type for a given reserved word.
 *
 * TODO Make this function not so terrible? Maybe create an array of structs
 * that you can iterate through for these values.
 *
 * If the str is not a known reserved word, return identsym (this assumes that
 * str matches the grammar for identifiers beforehand).
 *
 * @param str string containing the reserved word/identifier
 * @return a token_type corresponding to the reserved word in str
 */
token_type get_reserved_word_sym(char *str) {
  if(strcmp(str, "begin")==0) {
    return beginsym;
  } else if(strcmp(str, "end")==0) {
    return endsym;
  } else if(strcmp(str, "if")==0) {
    return ifsym;
  } else if(strcmp(str, "then")==0) {
    return thensym;
  } else if(strcmp(str, "while")==0) {
    return whilesym;
  } else if(strcmp(str, "do")==0) {
    return dosym;
  } else if(strcmp(str, "call")==0) {
    return callsym;
  } else if(strcmp(str, "const")==0) {
    return constsym;
  } else if(strcmp(str, "int")==0) {
    return intsym;
  } else if(strcmp(str, "procedure")==0) {
    return procsym;
  } else if(strcmp(str, "out")==0) {
    return outsym;
  } else if(strcmp(str, "in")==0) {
    return insym;
  } else if(strcmp(str, "else")==0) {
    return elsesym;
  } else if(strcmp(str, "odd")==0) { // this guy is a special symbol...
    return oddsym;
  } else {
    return identsym;
  }
}

/**
 * Returns the token_type for a given special symbol.
 *
 * TODO Make this function not so terrible. Maybe create an array of structs
 * that you can iterate through for these values.
 *
 * If the str is not a known special symbol, just return nulsym because I don't
 * know what we're supposed to be using it for :).
 *
 * @param str string containing the special symbol
 * @return a token_type corresponding to the special symbol in str
 */
token_type get_special_symbol_sym (char *str) {
  if(strcmp(str, "+")==0) {
    return plussym;
  } else if(strcmp(str, "-")==0) {
    return minussym;
  } else if(strcmp(str, "*")==0) {
    return multsym;
  } else if(strcmp(str, "/")==0) {
    return slashsym;
  } else if(strcmp(str, "=")==0) {
    return eqsym;
  } else if(strcmp(str, "<>")==0) {
    return neqsym;
  } else if(strcmp(str, "<")==0) {
    return lessym;
  } else if(strcmp(str, "<=")==0) {
    return leqsym;
  } else if(strcmp(str, ">")==0) {
    return gtrsym;
  } else if(strcmp(str, ">=")==0) {
    return geqsym;
  } else if(strcmp(str, "(")==0) {
    return lparentsym;
  } else if(strcmp(str, ")")==0) {
    return rparentsym;
  } else if(strcmp(str, ",")==0) {
    return commasym;
  } else if(strcmp(str, ";")==0) {
    return semicolonsym;
  } else if(strcmp(str, ".")==0) {
    return periodsym;
  } else if(strcmp(str, ":=")==0) {
    return becomessym;
  } else {
    return nulsym;
  }
}

/**
 * Checks whether or not a given string is in the reserved_words global array.
 *
 * @param str the string to check
 * @return 1 if str exists in the reserved_words global array, else 0
 */
int is_reserved_word(char *str) {
  int i;
  for(i = 0; i < NUM_RESERVED_WORDS; i++) {
    if(strcmp(str, reserved_words[i])==0) {
      return 1;
    }
  }
  return 0;
}

/**
 * Checks whether or not a given character is a letter (A-Z, a-z).
 *
 * @param c the character to check
 * @return 1 if c is a letter, else 0
 */
int is_letter(char c) {
  if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
    return 1;
  return 0;
}

/**
 * Checks whether or not a given character is a digit (0-9).
 *
 * @param c the character to check
 * @return 1 if c is a digit, else 0
 */
int is_digit(char c) {
  if(c >= '0' && c <= '9')
    return 1;
  return 0;
}

/**
 * Checks whether or not a given character is in the special_symbols global
 * array.
 *
 * @param c the character to check
 * @return 1 if c exists in the special_symbols global array, else 0
 */
int is_special_symbol(char c) {
  int i;
  for(i = 0; i < NUM_SPECIAL_SYMBOLS; i++)
    if(c == special_symbols[i]) {
      return 1;
    }
  return 0;
}

/**
 * Checks whether or not a given character is in the invisible_characters
 * global array.
 *
 * @param c the character to check
 * @return 1 if c exists in the invisible_characters global array, else 0
 */
int is_invisible_character(char c) {
  int i;
  for(i = 0; i < NUM_INVISIBLE_CHARS; i++) {
    if(c == invisible_characters[i]) {
      return 1;
    }
  }
  if(c == EOF)
    return 1;
  return 0;
}

/**
 * Checks whether or not a given token is a relation symbol.
 *
 * @param token the token we're dealing with
 * @return 1 if token is a relation symbol, else 0
 */
int is_relation(token_type token) {
  switch(token) {
    case eqsym:  // =
    case neqsym: // <>
    case lessym: // <
    case leqsym: // <=
    case gtrsym: // >
    case geqsym: // >=
      return 1;
      break;
    default:
      return 0;
  }
}
