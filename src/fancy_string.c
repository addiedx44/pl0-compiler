/*
 * PL/0 Compiler
 * Written by Adam Dunson
 * Filename: fancy_string.c
 *
 * Custom fancy_string struct for variable length char arrays. Most stuff is at
 * least O(n) due to my realloc()'ing arrays all over the place.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fancy_string.h"

/**
 * Initializes a fancy_string.
 *
 * This allocates space for the empty string in str->value and stores 0 at
 * index 0. This helps with appending stuff down below.
 *
 * @param str the fancy_string to be initialized
 * @return  the fancy_string pointer
 */
fancy_string *fancy_initialize(fancy_string *str) {
  str = (fancy_string *)malloc(sizeof(fancy_string));
  str->value = (char *)malloc(sizeof(char));
  str->value[0] = 0;
  str->size = 0;
  return str;
}

/**
 * Copies the contents of src into dst after reallocating dst.
 *
 * Will also update dst->size.
 *
 * @param dst the fancy_string destination
 * @param src the c-string to be copied
 * @return  pointer to the location of the fancy_string
 */
fancy_string *fancy_copy(fancy_string *dst, char *src) {
  if(!dst) {
    dst = fancy_initialize(dst);
    if(!dst)
      return NULL;
  }

  dst->size = strlen(src);
  dst->value = (char *)realloc(dst->value, (dst->size + 1) * sizeof(char));
  strcpy(dst->value, src);

  return dst;
}

/**
 * Appends a string onto the end of the fancy_string.
 *
 * Will also update dst->size.
 *
 * @param dst the fancy_string
 * @param src the string to be appended
 * @return  pointer to the location of the fancy_string
 */
fancy_string *fancy_append(fancy_string *dst, char *src) {
  if(!dst) {
    dst = fancy_initialize(dst);
    if(!dst)
      return NULL;
  }

  dst->size += strlen(src);

  char tmp[dst->size + 1];
  sprintf(tmp, "%s%s", dst->value, src);

  dst->value = (char *)realloc(dst->value, (dst->size + 1) * sizeof(char));
  dst->value[0] = 0;

  strcpy(dst->value, tmp);
  return dst;
}

/**
 * Pushes a single character onto the end of the string.
 *
 * This simply converts the character to a string and calls fancy_append()
 * on that string.
 *
 * @param dst the pointer to a fancy_string
 * @param src the char to be pushed
 * @return  pointer to the location of the fancy_string
 */
fancy_string *fancy_push(fancy_string *dst, char src) {
  if(!dst) {
    dst = fancy_initialize(dst);
    if(!dst)
      return NULL;
  }

  char tmp[2];
  sprintf(tmp, "%c", src);

  return fancy_append(dst, tmp);
}

/**
 * Pops a single character off of the end of the string.
 *
 * @param dst the pointer to a fancy_string
 * @param src the char to be pushed
 * @return  pointer to the location of the fancy_string
 */
char fancy_pop(fancy_string *dst) {
  if(!dst) {
    return -1;
  }

  dst->size--;
  char c = dst->value[dst->size];
  dst->value[dst->size] = 0;
  dst->value = (char *)realloc(dst->value, (dst->size + 1) * sizeof(char));

  return c;
}

/**
 * Frees up the memory used by a fancy_string
 *
 * I love this function's name :D
 *
 * @param str the fancy_string to be freed
 */
void fancy_free(fancy_string *str) {
  if(str) {
    free(str->value);
    free(str);
  }
}
