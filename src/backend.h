#ifndef BACKEND_H
#define BACKEND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#define LANGUAGE_EXISTS_ERROR 255
#define WORD_EXISTS_ERROR 254
#define FILE_OPEN_ERROR 253
#define LANGUAGES_DIR_NAME "./Languages/"

typedef struct language {
    char * _word, * _definition;
    struct language * prev_word, * next_word;
} Language;

typedef struct dictionary {
    char * language_name;
    int flag_opened;
    Language * this_language;
    struct dictionary * prev_language, * next_language;
} Dictionary;


void initialize(void);
void finish(void);
void open_file(char *);
void close_file(char *);
int add_language(char *, int *);
void delete_language(char *, int *);
Dictionary * find_language(char *, int *);
Language * find_word(char *, char *, int *);
int add_word(char *, char *, char *, int *);
void delete_word(char *, char *, int *);
int change_word(char *, char *, char *, char *, int *);
void change_definition(char *, char *, char *, int *);
// wrapper stuff
Dictionary * get_m_dictionary(void);
// debugging
void traverse(void);


#endif // BACKEND_H