#ifndef WRAPPER_H
#define WRAPPER_H

#include "backend.h"

typedef struct wrapper_language_title {
    char * name;
    int word_count;
} Wrapper_Language_Title;

typedef struct wrapper_word {
    char * word, * definition;
} Wrapper_Word;

void * get_language(void * (*func_para)(void *), void * key);
void * get_title(void * (*func_para)(void *), void * key);
void * get_word(void * (*func_para)(void *), void * key);
void get_languages(void * (*func)(void *));
void get_titles(void * (*func)(void *));
void get_words(void * (*func)(void *));
char * get_active_language(void);
char * get_active_title(void);
// debug
void * print_language(void *);
void * print_title(void *);
void * print_word(void *);

#endif // WRAPPER_H