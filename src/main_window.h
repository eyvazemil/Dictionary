#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib-object.h>
#include <pthread.h>
#include "backend.h"

#define GRID_WIDTH 26
#define GRID_HEIGHT 14
#define TEXT_SIZE 11

typedef struct strings {
    char ** strings_to_free;
    size_t len, count;
} Strings;

// main window and grid
void main_window(void);
GtkWidget * set_grid_cells(int width, int height);
void populate_main_window(GtkWidget *);
// language list
void language_choice_list(GtkWidget *);
void update_language_list(void);
// message dialogs
void message_simple(char *);
int message_with_response(char *);
// word list
void words_list(GtkWidget *);
void update_word_list(char *);
void words_list_empty(void);
void front_add_word(char *, char *, int);
void search_entry(GtkWidget *);
// set spinner
void spinner_create(GtkWidget *);
// language callbacks
void button_add_language_callback(GtkWidget *, void *);
void button_remove_language_callback(GtkWidget *, void *);
void button_language_callback(GtkWidget *, void *);
// word callbacks
void button_add_word_callback(GtkWidget *, void *);
void button_remove_word_callback(GtkWidget *, void *);
void button_change_word_callback(GtkWidget *, void *);
// search callback
void button_search_callback(GtkWidget *, void *);
// image stuff
GdkPixbuf * create_pixbuf(const char *);
void set_button_image(GtkWidget *, const char *);
// checking
int string_language_valid(char *);
int string_word_valid(char *);
// memory freeing
void memory_free(void);

#endif // MAIN_WINDOW_H