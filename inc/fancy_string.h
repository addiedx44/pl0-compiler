/*
 * PL/0 Compiler
 * Written by Adam Dunson
 * Filename: fancy_string.h
 *
 * Custom fancy_string struct's header.
 */

#ifndef FANCY_STRING_H
#define FANCY_STRING_H

struct fancy_string {
  char *value;
  unsigned int size;
};
typedef struct fancy_string fancy_string;

fancy_string *fancy_initialize(fancy_string *str);
fancy_string *fancy_copy(fancy_string *dst, char *src);
fancy_string *fancy_append(fancy_string *dst, char *src);
fancy_string *fancy_push(fancy_string *dst, char src);
char fancy_pop(fancy_string *dst);
void fancy_free(fancy_string *str);

#endif
