#ifndef BACKEND_H
#define BACKEND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include "avl_tree.h"
#include "string.h"

#define LANGUAGE_EXISTS_ERROR 255
#define TITLE_EXISTS_ERROR 254
#define WORD_EXISTS_ERROR 253
#define FILE_OPEN_ERROR 252
#define LANGUAGE_NOT_CHOSEN 251
#define NULL_TITLE_CHANGE_ATTEMPT 250


typedef struct word {
    String m_word, m_definition;
    struct word * prev_word, * next_word;
} Word;

typedef struct title {
    String title_name;
    Word * first_word_ptr, * last_word_ptr;
    AVL word_tree;
    int word_count;
} Title;

typedef struct language {
    String language_name;
    AVL title_tree;
    int flag_modified, word_count;
} Language;

typedef struct dictionary {
    AVL language_tree;
} Dictionary;


// main functions
void initialize(char * base_dir);
void finish(void);
void change_language(char * lang);
void open_file(char * lang);
void close_file(void);
Language * create_language(char *, int);
int add_language(char * lang);
void delete_language(char * lang);
Language * find_language(char * lang);
Title * create_title(char *);
int add_title(char * title, int flag_open);
int delete_title(char * title);
Title * find_title(char * title);
void choose_title(char * title);
int change_title_name(char * title, char * new_title);
void * write_title(void *, void *);
Word * create_word(char *, char *);
int add_word(char * word, char * def, int flag_open);
void delete_word(char * word);
Word * find_word(char * word);
int change_word(char * word, char * new_title, char * new_word, char * new_def);
void * write_word(void *, void *);
// AVL tree functions
int avl_compare_language(const void *, const void *);
int avl_compare_title(const void *, const void *);
int avl_compare_word(const void *, const void *);
void avl_free_dictionary(void);
void avl_free_language(void *);
void avl_free_title(void *);
void avl_free_word(void *);
void * avl_print_language(void *, void *);
void * avl_print_title(void *, void *);
void * avl_print_word(void *, void *);
// wrapper stuff
Dictionary * get_m_dictionary(void);
Language * get_m_active_language(void);
Title * get_m_active_title(void);
// github synchronization
void run_exec(String * program_name);
// debugging
void traverse(void);
// misc
char * int_to_str(int num);
void str_reverse(char * str, int count);


#endif // BACKEND_H