/*
 * PL/0 Compiler
 * Written by Adam Dunson
 * Filename: pl0-parsegen.c
 *
 * This is where the majority of the parsing and code generation gets done.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pl0-compiler.h"
#include "pl0-parsegen.h"
#include "pl0-tokens.h"
#include "pm0.h"

const char *parse_errors[NUM_PARSE_ERRORS] = {
  /* 0. */ "No errors, program is syntactically correct.",
  /* 1. */ "Use = instead of :=.",
  /* 2. */ "= must be followed by a number.",
  /* 3. */ "Identifier must be followed by =.",
  /* 4. */ "const, int, procedure must be followed by identifier.",
  /* 5. */ "Semicolon or comma missing.",

  /* XXX ??? */
  /* 6. */ "Incorrect symbol after procedure declaration.",
  /* 7. */ "Statement expected.",
  /* 8. */ "Incorrect symbol after statement part in block.",
  /* XXX ??? */

  /* 9. */ "Period expected.",
  /* 10. */ "Semicolon between statements missing.",
  /* 11. */ "Undeclared identifier.",
  /* 12. */ "Assignment to constant or procedure is not allowed.",
  /* 13. */ "Assignment operator expected.",
  /* 14. */ "call must be followed by an identifier.",
  /* 15. */ "Call of a constant or variable is meaningless.",
  /* 16. */ "then expected.",

  /* XXX ??? */
  /* 17. */ "Semicolon or end expected.",
  /* XXX ??? */

  /* 18. */ "do expected.",

  /* XXX ??? */
  /* 19. */ "Incorrect symbol following statement.",
  /* XXX ??? */

  /* 20. */ "Relational operator expected.",
  /* 21. */ "Expression must not contain a procedure identifier.",
  /* 22. */ "Right parenthesis missing.",
  /* 23. */ "The preceding factor cannot begin with this symbol.",

  /* XXX ??? */
  /* 24. */ "An expression cannot begin with this symbol.",
  /* XXX ??? */

  /* 25. */ "This number is too large.",

  /* extra errors! */
  /* 26. */ "out must be followed by an expression.",
  /* 27. */ "in must be followed by an identifier.",
  /* 28. */ "Cannot reuse this symbol here.",
  /* 29. */ "Cannot redefine constants."
};

int curr_m[MAX_LEXI_LEVELS + 1] = {0};
int curr_l = 0;

/**
 * This is the main function for parsing/code generation.
 *
 * @param input_file the raw token file to read from (all numbers and spaces)
 */
int pl0_parse(FILE *input_file, int a_flag) {
  token_type token = nulsym;
  int error_code = 0;

  token = get_token(input_file);

  error_code = block(input_file, &token);

  if(!error_code) {
    if(token != periodsym) {
      error_code = 9;
    } else {
      // return 0; in main()
      error_code = emit(OPR, 0, OPR_RET);
      if(error_code)
        return error_code;
    }
  }

  if(!error_code && a_flag) {
    printf("%s\n\n", get_parse_error(error_code));

    print_code(stdout);
    print_code_pretty(stdout);
    printf("\n");
  }

  return error_code;
}

/**
 * The block function.
 *
 * block ::= const-declaration  var-declaration  statement.
 */
int block(FILE *input_file, token_type *token) {
  int error_code = 0;
  symbol *symbol;

  // sl, dl, ra
  curr_m[curr_l] += 3;
  error_code = emit(INC, 0, 3);
  if(error_code)
    return error_code;

  // const
  if(*token == constsym) {
    do {
      // identsym
      *token = get_token(input_file);
      if(*token != identsym)
        return 4;

      symbol = get_symbol(input_file, 1);

      // eqsym
      *token = get_token(input_file);
      if(*token != eqsym) {
        if(*token == becomessym)
          return 1;
        else
          return 3;
      }

      // number
      *token = get_token(input_file);
      if(*token != numbersym)
        return 2;

      if(!symbol->kind) {
        symbol->kind = 1;
        symbol->val = get_number(input_file);
      } else {
        return 29;
      }

      *token = get_token(input_file);
    } while(*token == commasym);
    if(*token != semicolonsym)
      return 5;

    *token = get_token(input_file);
  }

  // int
  if(*token == intsym) {
    int num_vars = 0;

    do {
      // identsym
      *token = get_token(input_file);
      if(*token != identsym)
        return 4;

      symbol = get_symbol(input_file, 1);
      if(!symbol->kind) {
        symbol->kind = 2;
        symbol->level = curr_l;
        symbol->addr = curr_m[curr_l]++;
        num_vars++;
      } else {
        return 28;
      }

      *token = get_token(input_file);
    } while(*token == commasym);
    if(*token != semicolonsym)
      return 5;

    error_code = emit(INC, 0, num_vars);
    if(error_code)
      return error_code;

    *token = get_token(input_file);
  }

  // proc
  while(*token == procsym) {
    // identsym
    *token = get_token(input_file);
    if(*token != identsym)
      return 4;

    // add identifier to symbol_table
    symbol = get_symbol(input_file, 1);
    if(!symbol->kind) {
      symbol->kind = 3;
      symbol->level = curr_l;
      symbol->addr = cx+1; /*curr_m[curr_l]++;*/
      curr_l++;
    } else {
      return 28;
    }

    // semicolonsym
    *token = get_token(input_file);
    if(*token != semicolonsym)
      return 5;

    *token = get_token(input_file);

    // JMP over the proc code
    int c1 = cx;
    error_code = emit(JMP, 0, 0);
    if(error_code)
      return error_code;

    // recurse block again
    error_code = block(input_file, token);
    if(error_code)
      return error_code;

    // return
    error_code = emit(OPR, 0, OPR_RET);
    if(error_code)
      return error_code;

    // set the address for the JMP
    code[c1].m = cx;

    // some cleanup, uses the curr_l global
    // clears all symbols at curr_l and then decrement curr_l
    proc_cleanup();

    // semicolonsym
    if(*token != semicolonsym)
      return 5;

    *token = get_token(input_file);
  }

  error_code = statement(input_file, token);
  if(error_code)
    return error_code;

  return error_code;
}

/**
 * The statement function.
 *
 * statement   ::= [ ident ":=" expression
 *              | "call" ident
 *              | "begin" statement { ";" statement } "end"
 *              | "if" condition "then" statement ["else" statement]
 *              | "while" condition "do" statement
 *              | "read" ident
 *              | "write" expression
 *              | e ] .
 */
int statement(FILE *input_file, token_type *token) {
  symbol *symbol;
  //int number;
  int error_code = 0;

  if(*token == identsym) {
    symbol = get_symbol(input_file, 0);

    if(!symbol->kind) {
      return 11;
    }
    if(symbol->kind != 2) {
      return 12;
    }

    *token = get_token(input_file);

    // becomessym
    if(*token != becomessym)
      return 13;

    *token = get_token(input_file);
    error_code = expression(input_file, token);
    if(error_code)
      return error_code;

    error_code = emit(STO, abs(symbol->level - curr_l), symbol->addr);
    if(error_code)
      return error_code;
  }

  // callsym
  else if(*token == callsym) {
    *token = get_token(input_file);
    if(*token != identsym)
      return 14;

    symbol = get_symbol(input_file, 0);

    if(!symbol) {
      return 14;
    } else if(!symbol->kind) {
      return 11;
    } else if(symbol->kind != 3) {
      return 15;
    }

    error_code = emit(CAL, abs(symbol->level - curr_l), symbol->addr);
    if(error_code)
      return error_code;

    *token = get_token(input_file);
  }

  // beginsym
  else if(*token == beginsym) {
    *token = get_token(input_file);
    error_code = statement(input_file, token);
    if(error_code) return error_code;

    while(*token == semicolonsym) {
      *token = get_token(input_file);
      error_code = statement(input_file, token);
      if(error_code) return error_code;
    }

    // XXX not sure if this is right, but let's roll with it
    if(*token != endsym) {
      if(*token == periodsym || *token == nulsym)
        return 17;
      return 10;
    }

    *token = get_token(input_file);
  }

  // ifsym
  else if(*token == ifsym) {
    *token = get_token(input_file);

    // condition
    error_code = condition(input_file, token);
    if(error_code)
      return error_code;

    if(*token != thensym)
      return 16; // then expected

    *token = get_token(input_file);

    int c1 = cx;
    error_code = emit(JPC, 0, 0);
    if(error_code)
      return error_code;

    error_code = statement(input_file, token);
    if(error_code)
      return error_code;

    // this is for jumping over the else
    int c2 = cx;
    error_code = emit(JMP, 0, 0);
    if(error_code)
      return error_code;

    code[c1].m = cx;

    // token is either semicolonsym or elsesym at this point
    if(*token == elsesym) {
      *token = get_token(input_file);
      error_code = statement(input_file, token);
      if(error_code) return error_code;
    }

    code[c2].m = cx;

    return error_code;
  }

  // whilesym
  else if(*token == whilesym) {
    int cx1 = cx;

    *token = get_token(input_file);

    // condition
    error_code = condition(input_file, token);
    if(error_code)
      return error_code;

    int cx2 = cx;

    error_code = emit(JPC, 0, 0);
    if(error_code)
      return error_code;

    if(*token != dosym)
      return 18; // do expected

    *token = get_token(input_file);

    error_code = statement(input_file, token);
    if(error_code)
      return error_code;

    error_code = emit(JMP, 0, cx1);
    if(error_code)
      return error_code;
    code[cx2].m = cx;

    return error_code;
  }

  else if(*token == outsym) {
    *token = get_token(input_file);

    error_code = expression(input_file, token);
    if(error_code)
      return error_code;

    error_code = emit(SIO_OUT, 0, 1);
    if(error_code)
      return error_code;
  }

  else if(*token == insym) {
    *token = get_token(input_file);

    if(*token == identsym) {
      symbol = get_symbol(input_file, 0);

      if(!symbol->kind) {
        return 11;
      }
      else if(symbol->kind == 2) {
        error_code = emit(SIO_IN, 0, 2);
        if(error_code)
          return error_code;

        error_code = emit(STO, abs(symbol->level - curr_l), symbol->addr);
        if(error_code)
          return error_code;
      } else {
        return 27;
      }
    }
    else {
      return 27;
    }

    *token = get_token(input_file);
  }

  /* XXX statements can be the empty string so wtf
     else {
  // statement expected
  return 7;
  }
  */

  return error_code;
}

/**
 * The condition function.
 *
 * condition ::= "odd" expression
 *            | expression  rel-op  expression.
 *
 */
int condition(FILE *input_file, token_type *token) {
  int error_code = 0;
  token_type relop = nulsym;

  // oddsym
  if(*token == oddsym) {
    relop = *token;
    *token = get_token(input_file);
  }

  // relation symbols
  else {
    error_code = expression(input_file, token);
    if(error_code)
      return error_code;

    if(!is_relation(*token)) {
      // relational operator expected
      return 20;
    }

    relop = *token;

    *token = get_token(input_file);
  }

  error_code = expression(input_file, token);
  if(error_code)
    return error_code;

  switch(relop) {
    case oddsym: // odd
      error_code = emit(OPR, 0, OPR_ODD);
      break;
    case eqsym:  // =
      error_code = emit(OPR, 0, OPR_EQL);
      break;
    case neqsym: // <>
      error_code = emit(OPR, 0, OPR_NEQ);
      break;
    case lessym: // <
      error_code = emit(OPR, 0, OPR_LSS);
      break;
    case leqsym: // <=
      error_code = emit(OPR, 0, OPR_LEQ);
      break;
    case gtrsym: // >
      error_code = emit(OPR, 0, OPR_GTR);
      break;
    case geqsym: // >=
      error_code = emit(OPR, 0, OPR_GEQ);
      break;
    default:
      break;
  }

  return error_code;
}

/**
 * The expression function.
 *
 * expression ::= [ "+"|"-"] term { ("+"|"-") term}.
 */
int expression(FILE *input_file, token_type *token) {
  int error_code = 0;
  int addop = nulsym;

  if(*token == plussym || *token == minussym) {
    addop = *token;
    *token = get_token(input_file);

    error_code = term(input_file, token);
    if(error_code)
      return error_code;

    if(addop == minussym)
      error_code = emit(OPR, 0, OPR_NEG); // negate
    if(error_code)
      return error_code;
  }

  else {
    error_code = term(input_file, token);
    if(error_code)
      return error_code;
  }

  while(*token == plussym || *token == minussym) {
    addop = *token;
    *token = get_token(input_file);

    error_code = term(input_file, token);
    if(error_code)
      return error_code;

    if(addop == plussym)
      error_code = emit(OPR, 0, OPR_ADD); // addition
    else
      error_code = emit(OPR, 0, OPR_SUB); // subtraction

    if(error_code)
      return error_code;
  }

  return error_code;
}

/**
 * The term function.
 *
 * term ::= factor {("*"|"/") factor}.
 */
int term(FILE *input_file, token_type *token) {
  int error_code = 0;
  int mulop = nulsym;

  error_code = factor(input_file, token);
  if(error_code)
    return error_code;

  while(*token == multsym || *token == slashsym) {
    mulop = *token;
    *token = get_token(input_file);

    error_code = factor(input_file, token);
    if(error_code)
      return error_code;

    if(mulop == multsym)
      error_code = emit(OPR, 0, OPR_MUL); // multiplication
    else
      error_code = emit(OPR, 0, OPR_DIV); // division

    if(error_code)
      return error_code;
  }

  return error_code;
}

/**
 * The factor function.
 *
 * factor ::= ident | number | "(" expression ")".
 */
int factor(FILE *input_file, token_type *token) {
  int error_code = 0;
  symbol *symbol;
  int number;

  // identsym
  if(*token == identsym) {
    symbol = get_symbol(input_file, 0);

    if(!symbol->kind) {
      return 11;
    }

    if(symbol->kind == 1) {
      error_code = emit(LIT, 0, symbol->val);
    } else if(symbol->kind == 2) {
      error_code = emit(LOD, abs(symbol->level - curr_l), symbol->addr);
    } else {
      return 21;
    }

    if(error_code)
      return error_code;

    *token = get_token(input_file);
  }

  // is number?
  else if(*token == numbersym) {
    number = get_number(input_file);

    error_code = emit(LIT, 0, number);
    if(error_code)
      return error_code;

    *token = get_token(input_file);

    if(*token == nulsym) {
      return 17;
    }
  }

  // (...)
  else if(*token == lparentsym) {
    *token = get_token(input_file);

    error_code = expression(input_file, token);
    if(error_code)
      return error_code;

    if(*token != rparentsym) {
      return 22;
    }

    *token = get_token(input_file);
  }

  else if(*token == nulsym) {
    return 17;
  }

  // bad start symbol/not a valid factor
  else {
    return 23;
  }

  return error_code;
}

/**
 * Gets (and returns) a token from the token file.
 *
 * @param input_file the token file
 * @return the token_type scanned from the file
 */
token_type get_token(FILE *input_file) {
  int tmp;
  if(fscanf(input_file, "%d", &tmp) == 1) {
    if(tmp >= 1 && tmp <= 34) {
      if(DEBUG) printf("DEBUG: get_token: %d %s\n", (token_type)tmp, get_token_symbol((token_type)tmp));
      return (token_type)tmp;
    }
  }
  return nulsym;
}

/**
 * Gets (and returns) a symbol from the token file.
 *
 * The idx is the hash of the symbol. See symbol_hash in pl0-compiler.c.
 *
 * @param input_file the token file
 * @param is_new  specifies whether or not we're getting a new symbol
 * @return a pointer to the symbol table after creating a new symbol
 */
symbol *get_symbol(FILE *input_file, int is_new) {
  char symbol_name[12];
  int idx = -1;

  if(fscanf(input_file, "%11s", symbol_name) == 1 && strlen(symbol_name) > 0) {
    if(DEBUG) printf("DEBUG: symbol_name = %s\n", symbol_name);
    int tmp_l = curr_l;
    do {
      idx = symbol_hash(symbol_name, tmp_l--);
    } while(!is_new && (idx < 0 || idx >= MAX_SYMBOL_TABLE_SIZE || !symbol_table[idx].kind) && tmp_l >= 0);

    if(DEBUG) printf("DEBUG: idx = %d\n", idx);

    if(idx >= 0 && idx < MAX_SYMBOL_TABLE_SIZE) {
      if(is_new) {
        strcpy(symbol_table[idx].name, symbol_name);
        if(DEBUG) printf("DEBUG: BRAND NEW SYMBOL!!!!!!!!!!!\n");
      }
      if(DEBUG) printf("DEBUG: ********************************\n");
      if(DEBUG) printf("DEBUG: symbol_table[%d].kind = %d\n", idx, symbol_table[idx].kind);
      if(DEBUG) printf("DEBUG: symbol_table[%d].name = %s\n", idx, symbol_table[idx].name);
      if(DEBUG) printf("DEBUG: symbol_table[%d].val = %d\n", idx, symbol_table[idx].val);
      if(DEBUG) printf("DEBUG: symbol_table[%d].level = %d\n", idx, symbol_table[idx].level);
      if(DEBUG) printf("DEBUG: symbol_table[%d].addr = %d\n", idx, symbol_table[idx].addr);
      if(DEBUG) printf("DEBUG: ********************************\n");
      return &symbol_table[idx];
    }
  }

  return NULL;
}

/**
 * Gets a number from the token file.
 *
 * This is for number literals.
 *
 * @param input_file the token file
 * @return the number read from the token file
 */
int get_number(FILE *input_file) {
  int n = 0;
  fscanf(input_file, "%d", &n);
  if(DEBUG) printf("DEBUG: n = %d\n", n);
  return n;
}

/**
 * Converts an error number into an error string.
 *
 * @param e the error number
 * @return the error string that e corresponds to
 */
const char *get_parse_error(int e) {
  if(e < 0 || e > NUM_PARSE_ERRORS)
    return "Invalid error code.";
  return parse_errors[e];
}

/**
 * Emits code into the global code array.
 *
 * cx and code[] are created in pl0-compiler.c
 *
 * @param op the op code
 * @param l the l value (lexical level)
 * @param m an address, value, OPR code, etc.
 * @return 0 on success, 25 on failure
 */
int emit(int op, int l, int m) {
  if(cx >= MAX_CODE_LENGTH)
    return 25;
  else {
    if(DEBUG) {
      printf("DEBUG: cx = %d, op = %d (%s), l = %d, m = %d", cx, op, get_op_code_symbol(op), l, m);
      if(op == OPR)
        printf(" (%s)", get_opr_symbol(m));
      printf("\n");
    }
    code[cx].op = op;
    code[cx].l = l;
    code[cx].m = m;
    cx++;
  }
  return 0;
}

/**
 * Cleans up after declaring a procedure.
 *
 * Need to remove all symbols related to a procedure to avoid hashing issues
 * with symbols at the same lex level, but inside different procedures.
 *
 * Uses the curr_l, symbol_table, EMPTY_SYMBOL globals as well as MAX_SYMBOL_TABLE_SIZE
 * constant.
 */
void proc_cleanup() {
  int i;
  for(i = 0; i < MAX_SYMBOL_TABLE_SIZE; i++) {
    if(symbol_table[i].level == curr_l) {
      if(DEBUG) printf("Clearing symbol \"%s\" at level %d\n", symbol_table[i].name, symbol_table[i].level);
      symbol_table[i] = EMPTY_SYMBOL; // nuked
    }
  }
  // finally, decrement curr_l
  curr_l--;
}
