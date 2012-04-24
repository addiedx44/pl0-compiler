/*
 * PL/0 Compiler
 * Written by Adam Dunson
 * Filename: pl0-lex.c
 *
 * Lexical analyzer. This is where the token file gets written.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pl0-compiler.h"
#include "pl0-lex.h"
#include "pl0-tokens.h"
#include "fancy_string.h"
#include "lexeme_list.h"

/**
 * The main function for lexical analysis.
 *
 * @param input_file the PL/0 code
 * @param output_file the token file to be written
 * @return the error_code
 */
int pl0_lex(FILE *input_file, FILE *output_file, int l_flag) {
  int c = 0;
  int end_comment = 0;
  int error_code = 0; // maybe we'll use this in the future?
  fancy_string *lex_tmp = NULL;
  token_type t_tmp = nulsym;
  lexeme_list *lexemes = NULL;

  while(!feof(input_file)) {
    c = getc(input_file);

    /* reserved words, identfiers */
    /******************************/
    if(is_letter(c)) {
      lex_tmp = fancy_push(lex_tmp, (char)c);
      while((is_letter(c) || is_digit(c)) && !feof(input_file)) {
        c = getc(input_file);
        if(is_letter(c)) {
          lex_tmp = fancy_push(lex_tmp, (char)c);
        } else if(is_digit(c)) {
          t_tmp = identsym;
          lex_tmp = fancy_push(lex_tmp, (char)c);
        }
      }
      ungetc(c, input_file);

      // if we still don't know what t_tmp is...
      if(t_tmp != identsym) {
        // figure it out
        t_tmp = get_reserved_word_sym(lex_tmp->value);
      }

      if(t_tmp == identsym) {
        if(lex_tmp->size > 11) {
          fprintf(stderr, "SCANNER ERROR: Name too long: %s\n", lex_tmp->value);
          exit(EXIT_FAILURE);
        }
      }

      /*
      // add identifier to symbol_table
      int idx = symbol_hash(lex_tmp->value);
      if(idx != -1) {
      if(!strlen(symbol_table[idx].name)) {
      strcpy(symbol_table[idx].name, lex_tmp->value);
      }
      } else {
      fprintf(stderr, "SCANNER ERROR: Too many identifiers, max is %d.\n", MAX_SYMBOL_TABLE_SIZE);
      exit(EXIT_FAILURE);
      }
      }
      */

      lexemes = add_lexeme(lexemes, lex_tmp->value, t_tmp);
  }

  /* numbers */
  /***********/
  else if(is_digit(c)) {
    t_tmp = numbersym;
    lex_tmp = fancy_push(lex_tmp, (char)c);
    while(is_digit(c) && !feof(input_file)) {
      c = getc(input_file);
      if(is_digit(c)) {
        lex_tmp = fancy_push(lex_tmp, (char)c);
      }
    }

    // found variable that starts with a number
    if(is_letter(c)) {
      // get the rest of the bad identifier
      lex_tmp = fancy_push(lex_tmp, (char)c);
      while(is_letter(c) && !feof(input_file)) {
        c = getc(input_file);
        if(is_letter(c))
          lex_tmp = fancy_push(lex_tmp, (char)c);
      }
      fprintf(stderr, "SCANNER ERROR: Variable does not start with letter: %s.\n", lex_tmp->value);
      exit(EXIT_FAILURE);
    }
    ungetc(c, input_file);

    // number has too many digits
    if(lex_tmp->size > 5) {
      fprintf(stderr, "SCANNER ERROR: Number too long: %s\n", lex_tmp->value);
      exit(EXIT_FAILURE);
    }

    lexemes = add_lexeme(lexemes, lex_tmp->value, t_tmp);
  }

  /* special symbols */
  /*******************/
  else if(is_special_symbol(c)) {
    // check for comment...
    if(c == '/') {
      c = getc(input_file);
      // got a comment here...
      if(c == '*') {
        while(!end_comment && !feof(input_file)) {
          c = getc(input_file);
          if(c == '*') {
            c = getc(input_file);
            if(c == '/')
              end_comment = 1;
          }
        }

        // if we got EOF before ending the comment, break
        if(feof(input_file) && !end_comment) {
          fprintf(stderr, "SCANNER ERROR: Reached EOF before end of comment.\n");
          exit(EXIT_FAILURE);
        }
      } else {
        ungetc(c, input_file);
        c = '/';
      }
    }

    // if we didn't run into a comment
    if(!end_comment) {
      lex_tmp = fancy_push(lex_tmp, (char)c);
      if(is_special_symbol(c)) {
        c = getc(input_file);
        if(is_special_symbol(c))
          lex_tmp = fancy_push(lex_tmp, (char)c);
        else
          ungetc(c, input_file);
      } else {
        ungetc(c, input_file);
      }

      // check to make sure t_tmp gets set
      while(lex_tmp->size > 0 && t_tmp == nulsym) {
        t_tmp = get_special_symbol_sym(lex_tmp->value);
        if(t_tmp == nulsym)
          ungetc(fancy_pop(lex_tmp), input_file);
      }

      if(t_tmp != nulsym)
        lexemes = add_lexeme(lexemes, lex_tmp->value, t_tmp);
      else if(!(lex_tmp->size <= 1 && is_invisible_character(lex_tmp->value[0]))) {
        fprintf(stderr, "SCANNER ERROR: Invalid symbol: ");
        int i;
        for(i = 0; i < lex_tmp->size; i++)
          fprintf(stderr, "%d ", lex_tmp->value[i]);
        fprintf(stderr, "(\"%s\")\n", lex_tmp->value);
        exit(EXIT_FAILURE);
      }
    }
  } else if(!is_invisible_character(c)) {
    fprintf(stderr, "SCANNER ERROR: Invalid symbol: %d ('%c')\n", c, c);
    exit(EXIT_FAILURE);
  }

  // free the temporary lex string, reset t_tmp
  fancy_free(lex_tmp);
  lex_tmp = NULL;
  t_tmp = nulsym;
  end_comment = 0;
}

if(l_flag) {
  printf("Token File (Raw)\n");
  printf("================\n");
  print_internal_lexeme_list(stdout, lexemes);
  printf("\n");

  printf("Token File (Symbolic)\n");
  printf("=====================\n");
  print_symbolic_internal_lexeme_list(stdout, lexemes);
  printf("\n");
}

print_internal_lexeme_list(output_file, lexemes);

destroy_lexemes(lexemes);

return error_code;
}
