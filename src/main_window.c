#include "main_window.h"

GtkWidget * window;
GtkWidget * list_languages, * list_words;
GtkWidget * scrolled_window_languages, * scrolled_window_words;
GtkWidget * entry_add_word, * entry_add_definition;
GtkWidget * label_chosen_language;
GtkWidget * spinner;
// active language
char * active_language = NULL;
// initialize struct
Strings _strings;

// main window and grid
void main_window(void) {
    GdkPixbuf * window_icon;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // initialize strings structure
    _strings.strings_to_free = NULL;
    _strings.len = 1;
    _strings.count = 0;
    // setting up window
    gtk_window_set_title((GtkWindow *) window, "Dictionary");
    gtk_window_set_default_size((GtkWindow *) window, 800, 600);
    gtk_window_set_position((GtkWindow *) window, GTK_WIN_POS_CENTER);
    gtk_container_set_border_width((GtkContainer *) window, 5);
    window_icon = create_pixbuf("./Resources/dictionary_icon.png");
    gtk_window_set_icon((GtkWindow *) window, window_icon);
    gtk_window_maximize((GtkWindow *) window);
    // setting up window widgets
    populate_main_window(window);
    // establishing connections
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    // showing window
    gtk_widget_show_all(window);
}

GtkWidget * set_grid_cells(int width, int height) {
    GtkWidget * grid = gtk_grid_new();
    // set cells
    gtk_grid_set_row_homogeneous((GtkGrid *) grid, TRUE);
    gtk_grid_set_column_homogeneous((GtkGrid *) grid, TRUE);
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            GtkWidget * cell = gtk_label_new("");
            gtk_grid_attach((GtkGrid *) grid, cell, j, i, 1, 1);
        }
    }
    return grid;
}

void populate_main_window(GtkWidget * window) {
    // set ui layout as a grid
    GtkWidget * grid = set_grid_cells(GRID_WIDTH, GRID_HEIGHT);
    // search entry
    search_entry(grid);
    // language list
    language_choice_list(grid);
    // words list
    words_list(grid);
    // set spinner
    spinner_create(grid);
    // set grid to the window
    gtk_container_add((GtkContainer *) window, grid);
}

// language list
void language_choice_list(GtkWidget * grid) {
    GtkWidget * entry;
    GtkWidget * button_add_language;
    GtkWidget * frame_chosen_language, * scrolled_window_chosen_language;
    // setting up an enrtry to add a new language
    entry = gtk_entry_new();
    gtk_grid_attach((GtkGrid *) grid, entry, 0, 0, 4, 1);
    // setting up button to add a language
    button_add_language = gtk_button_new();
    set_button_image(button_add_language, "./Resources/add_language.png");
    gtk_grid_attach((GtkGrid *) grid, button_add_language, 4, 0, 1, 1);
    g_signal_connect(button_add_language, "clicked", G_CALLBACK(button_add_language_callback), (void *) entry);
    // setting up a scrollable window for language list
    scrolled_window_languages = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow *) scrolled_window_languages, GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_grid_attach((GtkGrid *) grid, scrolled_window_languages, 0, 1, 5, GRID_HEIGHT - 1);
    gtk_widget_set_margin_top(scrolled_window_languages, 5);
    gtk_widget_set_margin_bottom(scrolled_window_languages, 5);
    gtk_widget_set_margin_start(scrolled_window_languages, 1);
    // setting up a language list
    list_languages = gtk_list_box_new();
    gtk_list_box_set_selection_mode((GtkListBox * ) list_languages, GTK_SELECTION_NONE);
    gtk_container_add((GtkContainer *) scrolled_window_languages, list_languages);
    // set chosen language label
    frame_chosen_language = gtk_frame_new("Language:");
    gtk_widget_set_margin_bottom(frame_chosen_language, 5);
    gtk_widget_set_margin_start(frame_chosen_language, 10);
    label_chosen_language = gtk_label_new("");
    scrolled_window_chosen_language = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow *) scrolled_window_chosen_language, GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    gtk_container_add((GtkContainer *) scrolled_window_chosen_language, label_chosen_language);
    gtk_container_add((GtkContainer *) frame_chosen_language, scrolled_window_chosen_language);
    gtk_grid_attach((GtkGrid *) grid, frame_chosen_language, 5, 0, 6, 1);
    // initializing list with languages
    update_language_list();
}

void update_language_list(void) {
    // create a language widget with button
    GtkWidget * one_row, * button_delete_language, * button_language;
    char * lang;
    // backend language iterating and adding to the list
    Dictionary * tmp = get_m_dictionary();
    while(tmp) {
        // create a lang name
        lang = (char *) malloc(strlen(tmp->language_name) + 1);
        memset(lang, '\0', strlen(tmp->language_name) + 1);
        strncpy(lang, tmp->language_name, strlen(tmp->language_name));
        // create a row
        one_row = set_grid_cells(5, 1);
        button_delete_language = gtk_button_new();
        set_button_image(button_delete_language, "./Resources/remove.png");
        g_signal_connect(button_delete_language, "clicked", G_CALLBACK(button_remove_language_callback), (void *) lang);
        button_language = gtk_button_new_with_label(tmp->language_name);
        g_signal_connect(button_language, "clicked", G_CALLBACK(button_language_callback), (void *) lang);
        gtk_grid_attach((GtkGrid *) one_row, button_language, 0, 0, 4, 1);
        gtk_grid_attach((GtkGrid *) one_row, button_delete_language, 4, 0, 1, 1);
        gtk_list_box_insert((GtkListBox *) list_languages, one_row, -1); // insert to the end
        tmp = tmp->next_language; // traverse further
    }
}

// message dialogs
void message_simple(char * message) {
    GtkWidget * dialog;
    dialog = gtk_message_dialog_new((GtkWindow *) window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, NULL);
    gtk_message_dialog_set_markup((GtkMessageDialog *) dialog, message);
    g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);
    gtk_widget_show_all(dialog);
}

int message_with_response(char * message) {
    int dialog_response;
    GtkWidget * dialog;
    dialog = gtk_message_dialog_new((GtkWindow *) window, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, NULL);
    gtk_dialog_add_buttons((GtkDialog *) dialog, "YES", 1, "NO", 0, NULL);
    gtk_message_dialog_set_markup((GtkMessageDialog *) dialog, message);
    dialog_response = gtk_dialog_run((GtkDialog *) dialog);
    gtk_widget_destroy(dialog);
    return dialog_response;
}

// word list
void words_list(GtkWidget * grid) {
    // create Pango font description
    PangoFontDescription * desc = pango_font_description_new();
    pango_font_description_set_size(desc, TEXT_SIZE * PANGO_SCALE);
    // widgets
    GtkWidget * button_add_word, * frame_word_str, * frame_definition_str;
    // create new word entries and add them to the grid
    entry_add_word = gtk_entry_new();
    //gtk_widget_set_margin_start(entry_add_word, 10);
    entry_add_definition = gtk_entry_new();
    // add Pango font to the entries
    gtk_widget_override_font(entry_add_word, desc);
    gtk_widget_override_font(entry_add_definition, desc);
    // free Pango font description
    pango_font_description_free(desc);
    //gtk_widget_set_margin_start(entry_add_definition, 10);
    button_add_word = gtk_button_new();
    set_button_image(button_add_word, "./Resources/add_language.png");
    // set frames for new word and definition entries
    frame_word_str = gtk_frame_new("Word:");
    frame_definition_str = gtk_frame_new("Definition:");
    gtk_widget_set_margin_start(frame_word_str, 10);
    gtk_widget_set_margin_start(frame_definition_str, 2);
    gtk_grid_attach((GtkGrid *) grid, frame_word_str, 5, 1, 6, 1);
    gtk_grid_attach((GtkGrid *) grid, frame_definition_str, 11, 1, 13, 1);
    // add entries in corresponding frames
    gtk_container_add((GtkContainer *) frame_word_str, entry_add_word);
    gtk_container_add((GtkContainer *) frame_definition_str, entry_add_definition);
    gtk_grid_attach((GtkGrid *) grid, button_add_word, 24, 1, 2, 1);
    g_signal_connect(button_add_word, "clicked", G_CALLBACK(button_add_word_callback), NULL);
    // setting up a scrollable window for words list
    scrolled_window_words = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow *) scrolled_window_words, GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_grid_attach((GtkGrid *) grid, scrolled_window_words, 5, 2, GRID_WIDTH - 5, GRID_HEIGHT - 2);
    gtk_widget_set_margin_top(scrolled_window_words, 5);
    gtk_widget_set_margin_bottom(scrolled_window_words, 5);
    gtk_widget_set_margin_start(scrolled_window_words, 10);
    gtk_widget_set_margin_end(scrolled_window_words, 1);
    // setting up a list
    list_words = gtk_list_box_new();
    gtk_list_box_set_selection_mode((GtkListBox * ) list_words, GTK_SELECTION_NONE);
    gtk_container_add((GtkContainer *) scrolled_window_words, list_words);
    gtk_container_set_focus_vadjustment((GtkContainer *) list_words, gtk_scrolled_window_get_vadjustment((GtkScrolledWindow *) scrolled_window_words));
}

void update_word_list(char * lang) {
    // create a word widget with buttons
    GtkWidget * one_row, * button_change_word, * button_delete_word;
    GtkWidget * scrolled_window_row_word, * scrolled_window_row_definition;
    GtkWidget * frame_word, * frame_definition, * label_word, * label_definition;
    char * word, * definition;
    // start the spinner
    gtk_spinner_start((GtkSpinner *) spinner); // it will appear only if file is very big
    // empty the word list
    words_list_empty();
    // backend words iterating and adding to the list
    Language * tmp = find_language(lang, NULL)->this_language;
    while(tmp) {
        // create a word and definition
        word = (char *) malloc(strlen(tmp->_word) + 1);
        memset(word, '\0', strlen(tmp->_word) + 1);
        strncpy(word, tmp->_word, strlen(tmp->_word));
        definition = (char *) malloc(strlen(tmp->_definition) + 1);
        memset(definition, '\0', strlen(tmp->_definition) + 1);
        strncpy(definition, tmp->_definition, strlen(tmp->_definition));
        // add word to the UI
        front_add_word(word, definition, -1);
        free(definition);
        tmp = tmp->next_word; // traverse further
    }
    gtk_widget_show_all(list_words);
    // stop the spinner
    gtk_spinner_stop((GtkSpinner *) spinner);
}

void words_list_empty(void) {
    // empty the word list
    GList * children, * iter;
    children = gtk_container_get_children((GtkContainer *) list_words);
    for(iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy((GtkWidget *) iter->data);
    g_list_free(children);
    // empty the structure with strings
    memory_free();
}

void front_add_word(char * new_word, char * new_definition, int pos_in_list) {
    // create Pango font description
    PangoFontDescription * desc = pango_font_description_new();
    pango_font_description_set_size(desc, TEXT_SIZE * PANGO_SCALE);
    // create layout
    GtkWidget * one_row, * button_change_word, * button_delete_word;
    GtkWidget * scrolled_window_row_word, * scrolled_window_row_definition;
    GtkWidget * frame_word, * frame_definition, * label_word, * label_definition;
    // add the word to the structure to free in the future
    if(_strings.count == _strings.len - 1) {
        size_t size = _strings.len *= 2;
        _strings.strings_to_free = (char **) realloc(_strings.strings_to_free, size * sizeof(char *));
    }
    _strings.strings_to_free[_strings.count] = new_word;
    (_strings.count)++;
    // create layout for new word
    one_row = set_grid_cells(20, 1);
    // creating frames for scrollable windows
    frame_word = gtk_frame_new(NULL);
    frame_definition = gtk_frame_new(NULL);
    gtk_grid_attach((GtkGrid *) one_row, frame_word, 0, 0, 6, 1);
    gtk_grid_attach((GtkGrid *) one_row, frame_definition, 6, 0, 12, 1);
    gtk_widget_set_margin_start(frame_definition, 2);
    // setting up scrollable window for a word and definition
    scrolled_window_row_word = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow *) scrolled_window_row_word, GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    scrolled_window_row_definition = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow *) scrolled_window_row_definition, GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    // attach scrollable windows into the frames
    gtk_container_add((GtkContainer *) frame_word, scrolled_window_row_word);
    gtk_container_add((GtkContainer *) frame_definition, scrolled_window_row_definition);
    // creating frames and labels for word and definition
    label_word = gtk_label_new(new_word);
    label_definition = gtk_label_new(new_definition);
    gtk_label_set_selectable((GtkLabel *) label_word, TRUE);
    gtk_label_set_selectable((GtkLabel *) label_definition, TRUE);
    // add Pango font to the labels
    gtk_widget_override_font(label_word, desc);
    gtk_widget_override_font(label_definition, desc);
    // add labels to their respective scrolled windows
    gtk_container_add((GtkContainer *) scrolled_window_row_word, label_word);
    gtk_container_add((GtkContainer *) scrolled_window_row_definition, label_definition);
    //free(new_definition);
    // setting up buttons
    button_delete_word = gtk_button_new();
    set_button_image(button_delete_word, "./Resources/remove.png");
    button_change_word = gtk_button_new();
    set_button_image(button_change_word, "./Resources/change.png");
    // connect signals
    g_signal_connect(button_delete_word, "clicked", G_CALLBACK(button_remove_word_callback), (void *) (_strings.count - 1));
    g_signal_connect(button_change_word, "clicked", G_CALLBACK(button_change_word_callback), (void *) (_strings.count - 1));
    // attach buttons to the grid
    gtk_grid_attach((GtkGrid *) one_row, button_change_word, 18, 0, 1, 1);
    gtk_grid_attach((GtkGrid *) one_row, button_delete_word, 19, 0, 1, 1);
    gtk_list_box_insert((GtkListBox *) list_words, one_row, pos_in_list); // insert into row with the given position
    gtk_widget_show_all(list_words); // show the refreshed list
    // free Pango font description
    pango_font_description_free(desc);
}

void search_entry(GtkWidget * grid) {
    // set entry and search button
    GtkWidget * entry_search, * button_search, * frame_search;
    entry_search = gtk_entry_new();
    button_search = gtk_button_new();
    set_button_image(button_search, "./Resources/search.png");
    frame_search = gtk_frame_new("Search:");
    gtk_widget_set_margin_end(frame_search, 2);
    gtk_grid_attach((GtkGrid *) grid, frame_search, 16, 0, 6, 1);
    gtk_grid_attach((GtkGrid *) grid, button_search, 22, 0, 1, 1);
    gtk_container_add((GtkContainer *) frame_search, entry_search);
    // connect signal
    g_signal_connect(button_search, "clicked", G_CALLBACK(button_search_callback), (void *) entry_search);
}

// set spinner
void spinner_create(GtkWidget * grid) {
    spinner = gtk_spinner_new();
    gtk_grid_attach((GtkGrid *) grid, spinner, 25, 0, 1, 1);
}

// language callbacks
void button_add_language_callback(GtkWidget * button_add_language, void * entry) {
    int pos_in_list = 0;
    int new_lang_len = strlen(gtk_entry_get_text((GtkEntry *) entry));
    char * new_lang = (char *) malloc(new_lang_len + 1);
    memset(new_lang, '\0', new_lang_len + 1);
    strncpy(new_lang, gtk_entry_get_text((GtkEntry *) entry), new_lang_len);
    if(!string_language_valid(new_lang)) {
        // not valid language name
        message_simple("Invalid language name.");
        free(new_lang);
    } else {
        if(add_language(new_lang, &pos_in_list) == LANGUAGE_EXISTS_ERROR) {
            // such language already exists
            char * message = (char *) malloc(strlen(new_lang) + 28);
            memset(message, '\0', strlen(new_lang) + 28);
            strcpy(message, "\"");
            strncat(message, new_lang, strlen(new_lang));
            strcat(message, "\" language already exists.");
            // create message dialog
            message_simple(message);
            // free the message
            free(message);
            free(new_lang);
        } else {
            // language successfully added
            GtkWidget * one_row, * button_delete_language, * button_language;
            one_row = set_grid_cells(5, 1);
            button_delete_language = gtk_button_new();
            set_button_image(button_delete_language, "./Resources/remove.png");
            g_signal_connect(button_delete_language, "clicked", G_CALLBACK(button_remove_language_callback), (void *) new_lang);
            button_language = gtk_button_new_with_label(new_lang);
            g_signal_connect(button_language, "clicked", G_CALLBACK(button_language_callback), (void *) new_lang);
            gtk_grid_attach((GtkGrid *) one_row, button_language, 0, 0, 4, 1);
            gtk_grid_attach((GtkGrid *) one_row, button_delete_language, 4, 0, 1, 1);
            gtk_list_box_insert((GtkListBox *) list_languages, one_row, pos_in_list); // insert to the given position
            gtk_widget_show_all(list_languages);
        }
    }
    gtk_entry_set_text((GtkEntry *) entry, ""); // empty the entry for new language name
}

void button_remove_language_callback(GtkWidget * button_delete_language, void * lang) {
    int dialog_response;
    char * dialog_message = (char *) malloc(strlen((char *) lang) + 37);
    memset(dialog_message, '\0', strlen((char *) lang) + 37);
    strcpy(dialog_message, "Would you like to delete language \"");
    strncat(dialog_message, (char *) lang, strlen(lang));
    strcat(dialog_message, "\"");
    // create a message dialog and get response from it
    dialog_response = message_with_response(dialog_message);
    free(dialog_message);
    // decide what to do based on response
    if(dialog_response) {
        // empty the word list
        words_list_empty();
        gtk_widget_show_all(list_words);
        if(active_language && !strcmp(active_language, (char *) lang)) {
            // set active language to NULL
            free(active_language);
            active_language = NULL;
            // unset chosen language
            gtk_label_set_text((GtkLabel *) label_chosen_language, "");
        }
        // delete this language
        int pos_in_list = 0;
        delete_language((char *) lang, &pos_in_list);
        // update the list
        GtkWidget * one_row = (GtkWidget *) gtk_list_box_get_row_at_index((GtkListBox *) list_languages, pos_in_list);
        gtk_widget_destroy(one_row);
        gtk_widget_show_all(list_languages);
        free((char *) lang); // free the language name string
    }
}

void button_language_callback(GtkWidget * button_language, void * lang) {
    // if some language is already open, close it
    if(active_language && !strcmp(active_language, (char *) lang)) return; // if opened chosen language is active
    if(active_language)
        close_file(active_language);
    // open new language and add its words to the list
    open_file(lang);
    // update UI
    update_word_list((char *) lang);
    // set active language to "lang"
    free(active_language);
    active_language = (char *) malloc(strlen(lang) + 1);
    memset(active_language, '\0', strlen(lang) + 1);
    strncpy(active_language, lang, strlen(lang));
    // set chosen language
    gtk_label_set_text((GtkLabel *) label_chosen_language, (char *) lang);
}

// word callbacks
void button_add_word_callback(GtkWidget * button, void * para) {
    // do all else
    int pos_in_list = 0;
    // initialize word and definition strings
    int new_word_len = strlen(gtk_entry_get_text((GtkEntry *) entry_add_word));
    int new_definition_len = strlen(gtk_entry_get_text((GtkEntry *) entry_add_definition));
    char * new_word = (char *) malloc(new_word_len + 1), * new_definition;
    memset(new_word, '\0', new_word_len + 1);
    strncpy(new_word, gtk_entry_get_text((GtkEntry *) entry_add_word), new_word_len);
    new_definition = (char *) malloc(new_definition_len + 1);
    memset(new_definition, '\0', new_definition_len + 1);
    strncpy(new_definition, gtk_entry_get_text((GtkEntry *) entry_add_definition), new_definition_len);
    // decide about the received word
    if(!string_word_valid(new_word) || !string_word_valid(new_definition)) {
        // word or definition is not valid
        message_simple("Incorrect word or definition.\nWord or definition should not contain \'-\', \'\\n\', and should not be empty.");
        free(new_word); // addition
    } else {
        if(active_language == NULL) {
            // no language chosen to add this word to
            message_simple("No language is chosen to add this word to.");
            free(new_word); // addition
        } else if(add_word(new_word, new_definition, active_language, &pos_in_list) == WORD_EXISTS_ERROR) {
            // set focus on the existing word
            GtkWidget * focus_child = (GtkWidget *) gtk_list_box_get_row_at_index((GtkListBox *) list_words, pos_in_list);
            gtk_container_set_focus_child((GtkContainer *) list_words, focus_child);
            // such language already exists
            char * message = (char *) malloc(strlen(new_word) + 24);
            memset(message, '\0', strlen(new_word) + 24);
            strcpy(message, "\"");
            strncat(message, new_word, strlen(new_word));
            strcat(message, "\" word already exists.");
            // create message dialog
            message_simple(message);
            // free message string
            free(message);
            free(new_word); // addition
        } else front_add_word(new_word, new_definition, -1);
    }
    free(new_definition);
    gtk_entry_set_text((GtkEntry *) entry_add_word, ""); // empty the entry for new word
    gtk_entry_set_text((GtkEntry *) entry_add_definition, ""); // empty the entry for new definition
}

void button_remove_word_callback(GtkWidget * button, void * id) {
    char * word = (_strings.strings_to_free)[(size_t) id];
    int dialog_response;
    char * dialog_message = (char *) malloc(strlen(word) + 33);
    memset(dialog_message, '\0', strlen(word) + 33);
    strcpy(dialog_message, "Would you like to delete word \"");
    strncat(dialog_message, word, strlen(word));
    strcat(dialog_message, "\"");
    // create a message dialog and get response from it
    dialog_response = message_with_response(dialog_message);
    free(dialog_message);
    // decide what to do based on response
    if(dialog_response) {
        // delete this language
        int pos_in_list = 0;
        delete_word(word, active_language, &pos_in_list);
        // update the list
        GtkWidget * one_row = (GtkWidget *) gtk_list_box_get_row_at_index((GtkListBox *) list_words, pos_in_list);
        gtk_widget_destroy(one_row);
        gtk_widget_show_all(list_words);
        // free the word and set the structure pointer to NULL
        free(word);
        (_strings.strings_to_free)[(size_t) id] = NULL;
    }
}

void button_change_word_callback(GtkWidget * button, void * id) {
    char * word = (_strings.strings_to_free)[(size_t) id];
    char * def_ptr = find_word(word, active_language, NULL)->_definition;
    char * definition = (char *) malloc(strlen(def_ptr) + 1);
    memset(definition, '\0', strlen(def_ptr) + 1);
    strncpy(definition, def_ptr, strlen(def_ptr));
    def_ptr = NULL; // for insurance reasons
    // create a dialog window
    int dialog_response;
    GtkWidget * dialog, * dialog_content_area, * dialog_label_word, * dialog_label_definition;
    GtkWidget * dialog_entry_word, * dialog_entry_definition, * dialog_grid;
    dialog = gtk_dialog_new_with_buttons("Change word", (GtkWindow *) window, GTK_DIALOG_MODAL, "Change", 1, "Cancel", 0, NULL);
    //gtk_dialog_add_buttons((GtkDialog *) dialog, "Change", 1, "Cancel", 0, NULL);
    // create a grid for dialog's content area
    dialog_grid = set_grid_cells(8, 4);
    // set dialogs content area
    dialog_content_area = gtk_dialog_get_content_area((GtkDialog *) dialog);
    gtk_container_add((GtkContainer *) dialog_content_area, dialog_grid);
    // populate grid
    dialog_label_word = gtk_label_new("Word:");
    gtk_label_set_xalign((GtkLabel *) dialog_label_word, 0);
    dialog_label_definition = gtk_label_new("Definition:");
    gtk_label_set_xalign((GtkLabel *) dialog_label_definition, 0);
    dialog_entry_word = gtk_entry_new();
    gtk_entry_set_text((GtkEntry *) dialog_entry_word, word);
    dialog_entry_definition = gtk_entry_new();
    gtk_entry_set_text((GtkEntry *) dialog_entry_definition, definition);
    free(definition); // as we don't need it anymore
    gtk_grid_attach((GtkGrid *) dialog_grid, dialog_label_word, 0, 0, 2, 1);
    gtk_grid_attach((GtkGrid *) dialog_grid, dialog_entry_word, 0, 1, 8, 1);
    gtk_grid_attach((GtkGrid *) dialog_grid, dialog_label_definition, 0, 2, 2, 1);
    gtk_grid_attach((GtkGrid *) dialog_grid, dialog_entry_definition, 0, 3, 8, 1);
    // show the dialog
    gtk_widget_show_all(dialog);
    // dialog run
    dialog_response = gtk_dialog_run((GtkDialog *) dialog);
    // get word and definition from the dialog entries
    char * new_word, * new_definition;
    new_word = (char *) malloc(strlen(gtk_entry_get_text((GtkEntry *) dialog_entry_word)) + 1);
    memset(new_word, '\0', strlen(gtk_entry_get_text((GtkEntry *) dialog_entry_word)) + 1);
    strncpy(new_word, gtk_entry_get_text((GtkEntry *) dialog_entry_word), strlen(gtk_entry_get_text((GtkEntry *) dialog_entry_word)));
    new_definition = (char *) malloc(strlen(gtk_entry_get_text((GtkEntry *) dialog_entry_definition)) + 1);
    memset(new_definition, '\0', strlen(gtk_entry_get_text((GtkEntry *) dialog_entry_definition)) + 1);
    strncpy(new_definition, gtk_entry_get_text((GtkEntry *) dialog_entry_definition), strlen(gtk_entry_get_text((GtkEntry *) dialog_entry_definition)));
    // destroy the dialog, as we don't need it anymore
    gtk_widget_destroy(dialog);
    // decide, based on the dialog response
    int pos_in_list = 0;
    if(!dialog_response) {
        // cancel was pressed
        free(new_word);
        free(new_definition);
        return;
    }
    if(!string_word_valid(new_word) || !string_word_valid(new_definition)) {
        // word or definition is not valid
        message_simple("Incorrect word or definition.\nWord or definition should not contain \'-\', \'\\n\', and should not be empty.");
        free(new_word);
    } else {
        if(strcmp(word, new_word) != 0 && change_word(new_word, new_definition, word, active_language, &pos_in_list) == WORD_EXISTS_ERROR) {
            // set focus on the existing word
            GtkWidget * focus_child = (GtkWidget *) gtk_list_box_get_row_at_index((GtkListBox *) list_words, pos_in_list);
            gtk_container_set_focus_child((GtkContainer *) list_words, focus_child);
            // such language already exists
            char * message = (char *) malloc(strlen(new_word) + 24);
            memset(message, '\0', strlen(new_word) + 24);
            strcpy(message, "\"");
            strncat(message, new_word, strlen(new_word));
            strcat(message, "\" word already exists.");
            // create message dialog
            message_simple(message);
            // free message string
            free(message);
            free(new_word);
        } else {
            // if new word is equal to the previous one, but the definition has changed, update UI
            if(!strcmp(word, new_word))
                change_definition(new_definition, new_word, active_language, &pos_in_list);
            // remove old word
            GtkWidget * remove_row = (GtkWidget *) gtk_list_box_get_row_at_index((GtkListBox *) list_words, pos_in_list);
            gtk_widget_destroy(remove_row);
            // add new one to the position of old
            front_add_word(new_word, new_definition, pos_in_list);
            // free previous word
            free((_strings.strings_to_free)[(size_t) id]);
            (_strings.strings_to_free)[(size_t) id] = NULL;
        }
    }
    free(new_definition);
}

// search callback
void button_search_callback(GtkWidget * button_search, void * entry) {
    // if active language is NULL, return with error message
    if(active_language == NULL) {
        message_simple("No language is chosen.");
        gtk_entry_set_text((GtkEntry *) entry, "");
        return;
    }
    // else go on
    int search_word_len = strlen(gtk_entry_get_text((GtkEntry *) entry));
    char * search_word = (char *) malloc(search_word_len + 1);
    memset(search_word, '\0', search_word_len + 1);
    strncpy(search_word, gtk_entry_get_text((GtkEntry *) entry), search_word_len);
    // find if such word exists
    int pos_in_list = 0;
    if(find_word(search_word, active_language, &pos_in_list) == NULL)
        message_simple("Such word does not exist.");
    else {
        // set focus on the found word
        GtkWidget * focus_child = (GtkWidget *) gtk_list_box_get_row_at_index((GtkListBox *) list_words, pos_in_list);
        gtk_container_set_focus_child((GtkContainer *) list_words, focus_child);
    }
    // free memory and empty the search entry
    free(search_word);
    gtk_entry_set_text((GtkEntry *) entry, "");
}

// image stuff
GdkPixbuf * create_pixbuf(const char * filename) {
    GError * error = NULL;
    GdkPixbuf * pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    if(!pixbuf) {
        printf("Pixbuf creation error: %s\n", error->message);
        g_error_free(error);
    }
    return pixbuf;
}

void set_button_image(GtkWidget * button, const char * filename) {
    GdkPixbuf * icon = create_pixbuf(filename);
    GtkWidget * image = gtk_image_new_from_pixbuf(icon);
    gtk_button_set_image((GtkButton *) button, image);
    g_object_unref(icon);
}

// checking
int string_language_valid(char * str) {
    if(!strlen(str)) return 0;
    for(int i = 0; i < strlen(str); i++) {
        if((i == 0 && str[i] != '_' && !isalpha(str[i])) ||
            (str[i] != '_' && !isalnum(str[i]))) return 0;
    }
    return 1;
}

int string_word_valid(char * str) {
    if(!strlen(str)) return 0;
    for(int i = 0; i < strlen(str); i++)
        if(str[i] == '-' || str[i] == '\n') return 0;
    return 1;
}

// memory freeing
void memory_free(void) {
    for(size_t i = 0; i < _strings.count; i++)
        free((_strings.strings_to_free)[i]);
    free(_strings.strings_to_free);
    _strings.strings_to_free = NULL;
    _strings.len = 1;
    _strings.count = 0;
}