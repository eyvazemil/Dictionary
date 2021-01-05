#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct string {
    char * str;
    long len;
} String;

void _str_init_(String *);
void str_free(String *);
void str_append(String *, char *, int, int);
void str_append_char(String *, char);