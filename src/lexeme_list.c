/*
 * PL/0 Compiler
 * Written by Adam Dunson
 * Filename: lexeme_list.c
 *
 * Custom lexeme_list struct for storing the lexeme list. Pretty much only used
 * during the lexical analyzer stage.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexeme_list.h"
#include "fancy_string.h"
#include "pl0-compiler.h"
#include "pl0-tokens.h"

/**
 * Add lexemes to the lexeme_list.
 *
 * @param lexemes pointer to a lexeme_list
 * @param lex a lexeme string
 * @param t a token_type
 * @return a pointer to the first element in a lexeme_list
 */
lexeme_list *add_lexeme(lexeme_list *lexemes, char *lex, token_type t) {
  lexeme_list *pos = lexemes;

  // lexemes is empty
  if(!lexemes) {
    lexemes = (lexeme_list *)malloc(sizeof(lexeme_list));
    if(!lexemes) return NULL;
    pos = lexemes;
  } else {
    while(pos->next) pos = pos->next;
    pos->next = (lexeme_list *)malloc(sizeof(lexeme_list));
    if(!pos->next) {
      pos->next = NULL;
      return lexemes;
    }
    pos = pos->next;
  }

  pos->lex = (char *)malloc((strlen(lex) + 1) * sizeof(char));
  strcpy(pos->lex, lex);
  pos->t = t;
  pos->next = NULL;

  return lexemes;
}

/**
 * Frees up the memory used by a lexeme_list.
 *
 * @param lexemes a pointer to a lexeme_list
 */
void destroy_lexemes(lexeme_list *lexemes) {
  lexeme_list *del;
  while(lexemes) {
    del = lexemes;
    lexemes = lexemes->next;
    free(del->lex);
    free(del);
  }
}

/**
 * Prints out the lexemes in a table.
 *
 * Outputs in the order the lexemes were created (it's a linked list, friend).
 *
 * Outputs two columns: lexeme and token type
 */
void print_lexeme_table(lexeme_list *lexemes) {
  lexeme_list *pos = lexemes;

  printf("Lexeme Table:\n");
  printf("----------------------\n");
  printf("lexeme      token type\n");
  printf("----------------------\n");
  while(pos) {
    printf("%-11s %d\n", pos->lex, pos->t);
    pos = pos->next;
  }
}

/**
 * Prints out the list of lexemes.
 *
 * Each token is separated by a vertical bar '|'.
 *
 * If the token_type is either 2 or 3, need to print the lexeme (separated by a
 * space).
 *
 * Example: 
 * 29 | 2 x | 17 | 2 y | 18 | 21 | 2 y | 20 | 3 3 | 18 | 2 x | 20 | 2 y | 4 | 3
 * 56 | 18 | 22 | 19
 */
void print_lexeme_list(lexeme_list *lexemes) {
  lexeme_list *pos = lexemes;

  printf("Lexeme List:\n");
  printf("------------\n");
  while(pos) {
    printf("%d", pos->t);

    if(pos->t == identsym || pos->t == numbersym) {
      if(pos->next) printf(" ");
      printf("%s", pos->lex);
    }
    if(pos->next) printf(" | ");

    pos = pos->next;
  }
  printf("\n");
}

/**
 * Prints out the internal list of lexemes to a file.
 *
 * If the token_type is either 2 or 3, need to print the symbol hash or else
 * the lexeme (separated by a space).
 *
 * Example: 
 * 29 2 1 17 2 2 18 21 2 2 20 3 3 18 2 1 20 2 2 4 3 56 18 22 19
 */
void print_internal_lexeme_list(FILE *output_file, lexeme_list *lexemes) {
  if(!output_file) output_file = stdout;
  lexeme_list *pos = lexemes;

  while(pos) {
    fprintf(output_file, "%d", pos->t);

    if(pos->t == identsym) {
      //fprintf(output_file, " %d", symbol_hash(pos->lex));
      fprintf(output_file, " %s", pos->lex);
    } else if(pos->t == numbersym) {
      fprintf(output_file, " %s", pos->lex);
    }
    if(pos->next) fprintf(output_file, " ");

    pos = pos->next;
  }
  fprintf(output_file, "\n");
}

/**
 * Prints out the symbolic internal list of lexemes to a file.
 *
 * If the token_type is either 2 or 3, need to print the symbol hash or else
 * the lexeme (separated by a space).
 *
 * Example: 
 * intsym identsym.20 commasym identsym.26 semicolonsym beginsym identsym.26
 * becomessym numbersym.3 semicolonsym identsym.20 becomessym identsym.26
 * plussym numbersym.56 semicolonsym endsym periodsym
 */
void print_symbolic_internal_lexeme_list(FILE *output_file, lexeme_list *lexemes) {
  if(!output_file) output_file = stdout;
  lexeme_list *pos = lexemes;

  while(pos) {
    fprintf(output_file, "%s", get_token_symbol(pos->t));

    if(pos->t == identsym) {
      //fprintf(output_file, ".%d", symbol_hash(pos->lex));
      fprintf(output_file, ".%s", pos->lex);
    } else if(pos->t == numbersym) {
      fprintf(output_file, ".%s", pos->lex);
    }
    if(pos->next) fprintf(output_file, " ");

    pos = pos->next;
  }
  fprintf(output_file, "\n");
}

/**
 * Returns the list of lexemes as a character array.
 *
 * Each token is separated by a space ' '.
 *
 * If the token_type is either 2 or 3, need to print the lexeme (separated by a
 * space).
 *
 * Example: 
 * 29 2 x 17 2 y 18 21 2 y 20 3 3 18 2 x 20 2 y 4 3 56 18 22 19
 */
char *get_lexeme_list_str(lexeme_list *lexemes) {
  lexeme_list *pos = lexemes;
  char tmp_t[3];
  fancy_string *lexeme_list_str = NULL;

  while(pos) {
    sprintf(tmp_t, "%d", pos->t);
    lexeme_list_str = fancy_append(lexeme_list_str, tmp_t);

    if(pos->t == 2 || pos->t == 3) {
      if(pos->next)
        lexeme_list_str = fancy_push(lexeme_list_str, ' ');
      lexeme_list_str = fancy_append(lexeme_list_str, pos->lex);
    }
    if(pos->next) {
      lexeme_list_str = fancy_push(lexeme_list_str, ' ');
    }

    pos = pos->next;
  }

  char *return_str = lexeme_list_str->value;

  free(lexeme_list_str);

  return return_str;
}
