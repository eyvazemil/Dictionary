#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib-object.h>
#include "wrapper.h"

typedef struct ui_grid {
    GtkWidget * _grid;
    int width, height;
} UI_Grid;

typedef struct ui_search {
    GtkWidget * search_entry;
} UI_Search;

typedef struct ui_languages {
    GtkWidget * language_entry;
    GtkWidget * chosen_language;
    GtkWidget * languages_list;
} UI_Languages;

typedef struct ui_titles {
    GtkWidget * title_entry;
    GtkWidget * chosen_title;
    GtkWidget * titles_list;
} UI_Titles;

typedef struct ui_words {
    GtkWidget * word_entry, * definition_entry, * scrolled_window;
    GtkWidget * words_list;
} UI_Words;

typedef struct ui_window {
    GtkWidget * _window;
    GtkWidget * search_entry;
    UI_Grid window_grid;
    UI_Search window_search;
    UI_Languages window_languages;
    UI_Titles window_titles;
    UI_Words window_words;
    int text_size;
} UI_Window;

typedef struct ui_change_parameter {
    GtkWidget ** widgets_array;
    int array_size, dialog_response;
} UI_Change_Parameter;

// main window and grid
void main_window(char **);
GtkWidget * set_grid_cells(int width, int height);
void populate_main_window(void);
// language list
void languages_list(void);
void update_languages_list(void);
void * update_languages_list_helper(void *);
void set_chosen_language(char *);
void empty_language_entry(void);
void empty_languages_list(void);
// message dialogs
void message_simple(char *);
int message_with_response(char *);
void message_change(GtkWidget *, UI_Change_Parameter *, void (*func)(UI_Change_Parameter *), char *);
// title list
void titles_list(void);
void update_titles_list(void);
void * update_titles_list_helper(void *);
void set_chosen_title(char *);
void empty_title_entry(void);
void empty_titles_list(void);
void dialog_change_title_callback(UI_Change_Parameter *);
// word list
void words_list(void);
void update_words_list(void);
void * update_words_list_helper(void *);
void empty_word_entry(void);
void empty_words_list(void);
void dialog_change_word_callback(UI_Change_Parameter *);
// search
void search_entry(void);
// language callbacks
void button_add_language_callback(GtkWidget *, void *);
void button_remove_language_callback(GtkWidget *, void *);
void button_language_callback(GtkWidget *, void *);
// title callbacks
void button_add_title_callback(GtkWidget *, void *);
void button_remove_title_callback(GtkWidget *, void *);
void button_change_title_callback(GtkWidget *, void *);
void button_title_callback(GtkWidget *, void *);
// word callbacks
void button_add_word_callback(GtkWidget *, void *);
void button_remove_word_callback(GtkWidget *, void *);
void button_change_word_callback(GtkWidget *, void *);
// search callback
void button_search_callback(GtkWidget *, void *);
// image stuff
GdkPixbuf * create_pixbuf(const char *);
void set_button_image(GtkWidget *, const char *);
// misc
void destroy_all_children(GtkWidget *);
void set_font(GtkWidget *);

#endif // MAIN_WINDOW_H