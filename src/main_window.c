#include "main_window.h"

UI_Window m_window;

// main window and grid
void main_window(char ** args) {
    GdkPixbuf * window_icon;
    GtkWidget * window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // setting up window
    gtk_window_set_title((GtkWindow *) window, "Dictionary");
    gtk_window_set_default_size((GtkWindow *) window, 800, 600);
    gtk_window_set_position((GtkWindow *) window, GTK_WIN_POS_CENTER);
    gtk_container_set_border_width((GtkContainer *) window, 5);
    window_icon = create_pixbuf("./Resources/dictionary_icon.png");
    gtk_window_set_icon((GtkWindow *) window, window_icon);
    gtk_window_maximize((GtkWindow *) window);
    // setting up window widgets
    m_window._window = window;
    m_window.window_grid._grid = set_grid_cells(strtol(args[0], NULL, 10), strtol(args[1], NULL, 10)); // create a grid for the main window
    m_window.window_grid.width = strtol(args[0], NULL, 10);
    m_window.window_grid.height = strtol(args[1], NULL, 10);
    m_window.text_size = strtol(args[2], NULL, 10);
    gtk_container_add((GtkContainer *) m_window._window, m_window.window_grid._grid); // attach grid to the main window
    populate_main_window();
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
            gtk_widget_set_margin_top(cell, 0);
            gtk_widget_set_margin_bottom(cell, 0);
            gtk_widget_set_margin_start(cell, 0);
            gtk_widget_set_margin_end(cell, 0);
            gtk_grid_attach((GtkGrid *) grid, cell, j, i, 1, 1);
        }
    }
    return grid;
}

void populate_main_window(void) {
    // search entry
    search_entry();
    // language list
    languages_list();
    // titles list
    titles_list();
    // words list
    words_list();
}

// language list
void languages_list(void) {
    GtkWidget * entry, * list_languages, * label_chosen_language;
    GtkWidget * button_add_language, * button_remove_language;
    GtkWidget * frame_chosen_language, * scrolled_window_chosen_language, * scrolled_window_languages;
    GtkWidget * grid = m_window.window_grid._grid;
    int grid_height = m_window.window_grid.height;
    // setting up an enrtry to add a new language
    entry = gtk_entry_new();
    m_window.window_languages.language_entry = entry;
    gtk_grid_attach((GtkGrid *) grid, entry, 0, 0, 4, 1);
    gtk_widget_set_margin_bottom(entry, 5);
    set_font(entry);
    // setting up button to add a language
    button_add_language = gtk_button_new();
    set_button_image(button_add_language, "./Resources/add_language.png");
    gtk_grid_attach((GtkGrid *) grid, button_add_language, 4, 0, 1, 1);
    gtk_widget_set_margin_end(button_add_language, 10);
    g_signal_connect(button_add_language, "clicked", G_CALLBACK(button_add_language_callback), NULL);
    // setting up language remove button
    button_remove_language = gtk_button_new();
    set_button_image(button_remove_language, "./Resources/remove.png");
    gtk_grid_attach((GtkGrid *) grid, button_remove_language, 11, 0, 1, 1);
    g_signal_connect(button_remove_language, "clicked", G_CALLBACK(button_remove_language_callback), NULL);
    // setting up a scrollable window for language list
    scrolled_window_languages = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow *) scrolled_window_languages, GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
    gtk_grid_attach((GtkGrid *) grid, scrolled_window_languages, 0, 1, 5, grid_height - 1);
    gtk_widget_set_margin_top(scrolled_window_languages, 5);
    gtk_widget_set_margin_bottom(scrolled_window_languages, 5);
    gtk_widget_set_margin_start(scrolled_window_languages, 1);
    // setting up a language list
    list_languages = gtk_list_box_new();
    gtk_list_box_set_selection_mode((GtkListBox * ) list_languages, GTK_SELECTION_NONE);
    gtk_container_add((GtkContainer *) scrolled_window_languages, list_languages);
    m_window.window_languages.languages_list = list_languages;
    // set chosen language label
    frame_chosen_language = gtk_frame_new("Language:");
    gtk_widget_set_margin_bottom(frame_chosen_language, 5);
    gtk_widget_set_margin_start(frame_chosen_language, 10);
    label_chosen_language = gtk_label_new("<None>");
    scrolled_window_chosen_language = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow *) scrolled_window_chosen_language, GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    gtk_container_add((GtkContainer *) scrolled_window_chosen_language, label_chosen_language);
    gtk_container_add((GtkContainer *) frame_chosen_language, scrolled_window_chosen_language);
    gtk_grid_attach((GtkGrid *) grid, frame_chosen_language, 5, 0, 6, 1);
    set_font(label_chosen_language);
    m_window.window_languages.chosen_language = label_chosen_language;
    // initializing list with languages
    update_languages_list();
}

void update_languages_list(void) {
    // empty previous list
    empty_languages_list();
    // create new list
    get_languages(&update_languages_list_helper);
    gtk_widget_show_all(m_window.window_languages.languages_list);
}

void * update_languages_list_helper(void * para) {
    GtkWidget * one_row, * button_language;
    GtkWidget * list_languages = m_window.window_languages.languages_list;
    Wrapper_Language_Title * lang = (Wrapper_Language_Title *) para;
    one_row = set_grid_cells(5, 1);
    gtk_widget_set_margin_top(one_row, 1);
    gtk_widget_set_margin_bottom(one_row, 1);
    button_language = gtk_button_new_with_label(lang->name);
    set_font(button_language);
    g_signal_connect(button_language, "clicked", G_CALLBACK(button_language_callback), (void *) lang->name);
    gtk_grid_attach((GtkGrid *) one_row, button_language, 0, 0, 5, 1);
    gtk_list_box_insert((GtkListBox *) list_languages, one_row, -1); // insert to the end
}

void set_chosen_language(char * lang_name) {
    GtkWidget * current_lang = m_window.window_languages.chosen_language;
    if(!lang_name)
        gtk_label_set_text((GtkLabel *) current_lang, "<None>");
    else
        gtk_label_set_text((GtkLabel *) current_lang, lang_name);
    gtk_widget_show_all(current_lang);
}

void empty_language_entry(void) {
    GtkWidget * entry_lang = m_window.window_languages.language_entry;
    gtk_entry_set_text((GtkEntry *) entry_lang, "");
    gtk_widget_show_all(entry_lang);
}

void empty_languages_list(void) {
    destroy_all_children(m_window.window_languages.languages_list);
}

// message dialogs
void message_simple(char * message) {
    GtkWidget * dialog;
    dialog = gtk_message_dialog_new((GtkWindow *) m_window._window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, NULL);
    gtk_message_dialog_set_markup((GtkMessageDialog *) dialog, message);
    g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);
    gtk_widget_show_all(dialog);
}

int message_with_response(char * message) {
    int dialog_response;
    GtkWidget * dialog;
    dialog = gtk_message_dialog_new((GtkWindow *) m_window._window, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, NULL);
    gtk_dialog_add_buttons((GtkDialog *) dialog, "YES", 1, "NO", 0, NULL);
    gtk_message_dialog_set_markup((GtkMessageDialog *) dialog, message);
    dialog_response = gtk_dialog_run((GtkDialog *) dialog);
    gtk_widget_destroy(dialog);
    return dialog_response;
}

void message_change(GtkWidget * content, UI_Change_Parameter * func_para, void (*func)(UI_Change_Parameter *), char * dialog_name) {
    GtkWidget * dialog, * dialog_content_area;
    int dialog_response;
    // create dialog
    dialog = gtk_dialog_new_with_buttons(dialog_name, (GtkWindow *) m_window._window, GTK_DIALOG_MODAL, "Change", 1, "Cancel", 0, NULL);
    dialog_content_area = gtk_dialog_get_content_area((GtkDialog *) dialog);
    gtk_container_add((GtkContainer *) dialog_content_area, content);
    // show the dialog
    gtk_widget_show_all(dialog);
    // dialog run
    dialog_response = gtk_dialog_run((GtkDialog *) dialog);
    // set parameter for a callback function
    func_para->dialog_response = dialog_response;
    (*func)(func_para);
    // destroy the dialog screen
    gtk_widget_destroy(dialog);
}

// title list
void titles_list(void) {
    GtkWidget * frame_chosen_title, * label_chosen_title, * scrolled_window_chosen_title;
    GtkWidget * entry_title, * button_add_title, * button_change_title, * button_remove_title;
    GtkWidget * scrolled_window_titles, * list_titles;
    GtkWidget * grid = m_window.window_grid._grid;
    int grid_width = m_window.window_grid.width;
    // set chosen title label
    frame_chosen_title = gtk_frame_new("Title:");
    gtk_widget_set_margin_bottom(frame_chosen_title, 5);
    gtk_widget_set_margin_start(frame_chosen_title, 10);
    label_chosen_title = gtk_label_new("");
    scrolled_window_chosen_title = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow *) scrolled_window_chosen_title, GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    gtk_container_add((GtkContainer *) scrolled_window_chosen_title, label_chosen_title);
    gtk_container_add((GtkContainer *) frame_chosen_title, scrolled_window_chosen_title);
    gtk_grid_attach((GtkGrid *) grid, frame_chosen_title, 5, 2, 4, 1);
    set_font(label_chosen_title);
    m_window.window_titles.chosen_title = label_chosen_title;
    // setting up title change button
    button_change_title = gtk_button_new();
    set_button_image(button_change_title, "./Resources/change.png");
    gtk_widget_set_margin_start(button_change_title, 5);
    gtk_grid_attach((GtkGrid *) grid, button_change_title, 9, 2, 1, 1);
    g_signal_connect(button_change_title, "clicked", G_CALLBACK(button_change_title_callback), NULL);
    // setting up title remove button
    button_remove_title = gtk_button_new();
    set_button_image(button_remove_title, "./Resources/remove.png");
    gtk_widget_set_margin_start(button_remove_title, 10);
    gtk_grid_attach((GtkGrid *) grid, button_remove_title, 10, 2, 1, 1);
    g_signal_connect(button_remove_title, "clicked", G_CALLBACK(button_remove_title_callback), NULL);
    // seting up add title entry
    entry_title = gtk_entry_new();
    gtk_grid_attach((GtkGrid *) grid, entry_title, 14, 2, 5, 1);
    set_font(entry_title);
    m_window.window_titles.title_entry = entry_title;
    // setting up button to add a language
    button_add_title = gtk_button_new();
    set_button_image(button_add_title, "./Resources/add_language.png");
    gtk_widget_set_margin_start(button_add_title, 5);
    gtk_grid_attach((GtkGrid *) grid, button_add_title, 19, 2, 1, 1);
    g_signal_connect(button_add_title, "clicked", G_CALLBACK(button_add_title_callback), NULL);
    // setting up scrollable window for titles list
    scrolled_window_titles = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_margin_bottom(scrolled_window_titles, 10);
    gtk_scrolled_window_set_policy((GtkScrolledWindow *) scrolled_window_titles, GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    // insert the second row into the titles' grid list
    gtk_grid_attach((GtkGrid *) grid, scrolled_window_titles, grid_width - 7, 2, 7, 2);
    // setting up a titles list
    list_titles = gtk_list_box_new();
    gtk_container_add((GtkContainer *) scrolled_window_titles, list_titles);
    m_window.window_titles.titles_list = list_titles;
}

void update_titles_list(void) {
    // empty titles list
    empty_titles_list();
    // create new titles list
    get_titles(&update_titles_list_helper);
    gtk_widget_show_all(m_window.window_titles.titles_list);
}

void * update_titles_list_helper(void * para) {
    String button_text;
    GtkWidget * button_title;
    GtkWidget * list_titles = m_window.window_titles.titles_list;
    Wrapper_Language_Title * title = (Wrapper_Language_Title *) para;
    char * str_word_count = int_to_str(title->word_count);
    _str_init_(&button_text);
    if(!title->name)
        str_append(&button_text, "<None>", -1, 0);
    else
        str_append(&button_text, title->name, -1, 0);
    str_append(&button_text, " (", -1, 0);
    str_append(&button_text, str_word_count, -1, 0);
    str_append_char(&button_text, ')');
    button_title = gtk_button_new_with_label(button_text.str);
    set_font(button_title);
    g_signal_connect(button_title, "clicked", G_CALLBACK(button_title_callback), (void *) title->name);
    gtk_list_box_insert((GtkListBox *) list_titles, button_title, -1); // insert to the end
    // free resources
    str_free(&button_text);
    free(str_word_count);
}

void set_chosen_title(char * title_name) {
    GtkWidget * current_title = m_window.window_titles.chosen_title;
    if(!title_name)
        gtk_label_set_text((GtkLabel *) current_title, "<None>");
    else
        gtk_label_set_text((GtkLabel *) current_title, title_name);
    gtk_widget_show_all(current_title);
}

void empty_title_entry(void) {
    GtkWidget * entry_title = m_window.window_titles.title_entry;
    gtk_entry_set_text((GtkEntry *) entry_title, "");
    gtk_widget_show_all(entry_title);
}

void empty_titles_list(void) {
    destroy_all_children(m_window.window_titles.titles_list);
}

void dialog_change_title_callback(UI_Change_Parameter * para) {
    GtkWidget * entry = (para->widgets_array)[0];
    int dialog_response = para->dialog_response;
    char * active_title = get_active_title();
    if(dialog_response) { // if "change" button was pressed
        String str_entry;
        const char * new_title_name = gtk_entry_get_text((GtkEntry *) entry);
        _str_init_(&str_entry);
        str_append(&str_entry, new_title_name, -1, 0);
        // change title name
        int res = change_title_name(active_title, str_entry.str);
        if(!res) { // title was changed
            set_chosen_title(str_entry.str);
            update_titles_list();
        } else { // title exists
            String text;
            _str_init_(&text);
            str_append(&text, "Title \"", -1, 0);
            str_append(&text, str_entry.str, -1, 0);
            str_append(&text, "\" already exists.", -1, 0);
            message_simple(text.str);
            str_free(&text);
        }
        str_free(&str_entry);
    }
}

// word list
void words_list(void) {
    // widgets
    GtkWidget * entry_add_word, * entry_add_definition, * scrolled_window_words, * list_words;
    GtkWidget * button_add_word, * frame_word_str, * frame_definition_str;
    GtkWidget * grid = m_window.window_grid._grid;
    int grid_width = m_window.window_grid.width, grid_height = m_window.window_grid.height;
    // create new word entries and add them to the grid
    entry_add_word = gtk_entry_new();
    m_window.window_words.word_entry = entry_add_word;
    //gtk_widget_set_margin_start(entry_add_word, 10);
    entry_add_definition = gtk_entry_new();
    m_window.window_words.definition_entry = entry_add_definition;
    // add Pango font to the entries
    set_font(entry_add_word);
    set_font(entry_add_definition);
    //gtk_widget_set_margin_start(entry_add_definition, 10);
    button_add_word = gtk_button_new();
    set_button_image(button_add_word, "./Resources/add_language.png");
    // set frames for new word and definition entries
    frame_word_str = gtk_frame_new("Word:");
    frame_definition_str = gtk_frame_new("Definition:");
    gtk_widget_set_margin_start(frame_word_str, 10);
    gtk_widget_set_margin_start(frame_definition_str, 2);
    gtk_grid_attach((GtkGrid *) grid, frame_word_str, 5, 4, 7, 1);
    gtk_grid_attach((GtkGrid *) grid, frame_definition_str, 12, 4, 14, 1);
    // add entries in corresponding frames
    gtk_container_add((GtkContainer *) frame_word_str, entry_add_word);
    gtk_container_add((GtkContainer *) frame_definition_str, entry_add_definition);
    gtk_grid_attach((GtkGrid *) grid, button_add_word, 26, 4, 2, 1);
    g_signal_connect(button_add_word, "clicked", G_CALLBACK(button_add_word_callback), NULL);
    // setting up a scrollable window for words list
    scrolled_window_words = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy((GtkScrolledWindow *) scrolled_window_words, GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
    gtk_grid_attach((GtkGrid *) grid, scrolled_window_words, 5, 5, grid_width - 5, grid_height - 5);
    gtk_widget_set_margin_top(scrolled_window_words, 5);
    gtk_widget_set_margin_bottom(scrolled_window_words, 5);
    gtk_widget_set_margin_start(scrolled_window_words, 10);
    gtk_widget_set_margin_end(scrolled_window_words, 1);
    // setting up a list
    list_words = gtk_list_box_new();
    gtk_list_box_set_selection_mode((GtkListBox * ) list_words, GTK_SELECTION_NONE);
    gtk_container_add((GtkContainer *) scrolled_window_words, list_words);
    gtk_container_set_focus_vadjustment((GtkContainer *) list_words, gtk_scrolled_window_get_vadjustment((GtkScrolledWindow *) scrolled_window_words));
    m_window.window_words.words_list = list_words;
}

void update_words_list(void) {
    // empty words list
    empty_words_list();
    // create new words list
    get_words(&update_words_list_helper);
    gtk_widget_show_all(m_window.window_words.words_list);
}

void * update_words_list_helper(void * para) {
    GtkWidget * one_row, * button_change_word, * button_delete_word;
    GtkWidget * scrolled_window_row_word, * scrolled_window_row_definition;
    GtkWidget * frame_word, * frame_definition, * label_word, * label_definition;
    GtkWidget * list_words = m_window.window_words.words_list;
    int grid_width = m_window.window_grid.width, grid_height = m_window.window_grid.height;
    Wrapper_Word * word = (Wrapper_Word *) para;
    // create layout for new word
    one_row = set_grid_cells(grid_width - 5, 1);
    // creating frames for scrollable windows
    frame_word = gtk_frame_new(NULL);
    frame_definition = gtk_frame_new(NULL);
    gtk_grid_attach((GtkGrid *) one_row, frame_word, 0, 0, 7, 1);
    gtk_grid_attach((GtkGrid *) one_row, frame_definition, 7, 0, 14, 1);
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
    label_word = gtk_label_new(word->word);
    label_definition = gtk_label_new(word->definition);
    gtk_label_set_selectable((GtkLabel *) label_word, TRUE);
    gtk_label_set_selectable((GtkLabel *) label_definition, TRUE);
    // add Pango font to the labels
    set_font(label_word);
    set_font(label_definition);
    // add labels to their respective scrolled windows
    gtk_container_add((GtkContainer *) scrolled_window_row_word, label_word);
    gtk_container_add((GtkContainer *) scrolled_window_row_definition, label_definition);
    // setting up buttons
    button_delete_word = gtk_button_new();
    set_button_image(button_delete_word, "./Resources/remove.png");
    button_change_word = gtk_button_new();
    set_button_image(button_change_word, "./Resources/change.png");
    // connect signals
    g_signal_connect(button_delete_word, "clicked", G_CALLBACK(button_remove_word_callback), (void *) word->word);
    g_signal_connect(button_change_word, "clicked", G_CALLBACK(button_change_word_callback), (void *) word->word);
    // attach buttons to the grid
    gtk_grid_attach((GtkGrid *) one_row, button_change_word, 21, 0, 1, 1);
    gtk_grid_attach((GtkGrid *) one_row, button_delete_word, 22, 0, 1, 1);
    gtk_list_box_insert((GtkListBox *) list_words, one_row, -1); // insert into row with the given position
    gtk_widget_show_all(list_words); // show the refreshed list
}

void empty_word_entry(void) {
    GtkWidget * entry_word = m_window.window_words.word_entry;
    GtkWidget * entry_definition = m_window.window_words.definition_entry;
    gtk_entry_set_text((GtkEntry *) entry_word, "");
    gtk_entry_set_text((GtkEntry *) entry_definition, "");
}

void empty_words_list(void) {
    destroy_all_children(m_window.window_words.words_list);
}

void dialog_change_word_callback(UI_Change_Parameter * para) {
    GtkWidget * entry_word = (para->widgets_array)[0];
    GtkWidget * entry_definition = (para->widgets_array)[1];
    char * word = para->current_word;
    int dialog_response = para->dialog_response;
    char * active_title = get_active_title();
    Wrapper_Word para_word;
    para_word.word = word;
    wrapper_find_word(&para_word);
    if(dialog_response) { // if "change" button was pressed
        String str_entry_word, str_entry_definition;
        const char * new_word = gtk_entry_get_text((GtkEntry *) entry_word);
        const char * new_definition = gtk_entry_get_text((GtkEntry *) entry_definition);
        _str_init_(&str_entry_word);
        str_append(&str_entry_word, new_word, -1, 0);
        _str_init_(&str_entry_definition);
        str_append(&str_entry_definition, new_definition, -1, 0);
        // check if entries are empty
        if(!str_entry_word.str || !strcmp(str_entry_word.str, "") ||
            !str_entry_definition.str || !strcmp(str_entry_definition.str, ""))
        {
            String text;
            _str_init_(&text);
            str_append(&text, "Word or definition entries can't be empty.", -1, 0);
            message_simple(text.str);
            str_free(&text);
        } else { // change word
            if(!strcmp(word, str_entry_word.str) && strcmp(para_word.definition, str_entry_definition.str)) {
                change_definition(word, str_entry_definition.str);
                update_words_list();
            } else {
                int res = change_word(word, active_title, str_entry_word.str, str_entry_definition.str);
                if(!res) { // word was changed
                    update_titles_list();
                    update_words_list();
                } else { // word exists
                    String text;
                    _str_init_(&text);
                    str_append(&text, "Word \"", -1, 0);
                    str_append(&text, str_entry_word.str, -1, 0);
                    str_append(&text, "\" already exists.", -1, 0);
                    message_simple(text.str);
                    str_free(&text);
                }
            }
        }
        str_free(&str_entry_word);
        str_free(&str_entry_definition);
    }
}

// search
void search_entry(void) {
    // set entry and search button
    GtkWidget * entry_search, * button_search, * frame_search;
    GtkWidget * grid = m_window.window_grid._grid;
    entry_search = gtk_entry_new();
    button_search = gtk_button_new();
    set_button_image(button_search, "./Resources/search.png");
    frame_search = gtk_frame_new("Search:");
    gtk_widget_set_margin_end(frame_search, 2);
    gtk_grid_attach((GtkGrid *) grid, frame_search, 20, 0, 7, 1);
    gtk_grid_attach((GtkGrid *) grid, button_search, 27, 0, 1, 1);
    gtk_container_add((GtkContainer *) frame_search, entry_search);
    // connect signal
    g_signal_connect(button_search, "clicked", G_CALLBACK(button_search_callback), (void *) entry_search);
}

// language callbacks
void button_add_language_callback(GtkWidget * button, void * para) {
    GtkWidget * entry = m_window.window_languages.language_entry;
    const char * tmp_entry_text = gtk_entry_get_text((GtkEntry *) entry);
    char * entry_text;
    String str_entry_text;
    _str_init_(&str_entry_text);
    str_append(&str_entry_text, tmp_entry_text, -1, 0);
    entry_text = str_entry_text.str;
    // add new language
    int res = add_language(entry_text);
    if(!res) { // added successfully
        set_chosen_language(entry_text);
        update_languages_list();
        update_titles_list();
        update_words_list();
    } else if(res == LANGUAGE_EXISTS_ERROR) { // language already exists
        String text;
        _str_init_(&text);
        str_append(&text, "Language \"", -1, 0);
        str_append(&text, entry_text, -1, 0);
        str_append(&text, "\" already exists.", -1, 0);
        message_simple(text.str);
        str_free(&text);
    } else { // incorrect language name
        String text;
        _str_init_(&text);
        str_append(&text, "Language can't be an empty string.", -1, 0);
        message_simple(text.str);
        str_free(&text);
    }
    empty_language_entry();
    str_free(&str_entry_text);
}

void button_remove_language_callback(GtkWidget * button, void * para) {
    char * active_language = get_active_language();
    if(active_language) { // active language is set
        // create validation message
        String message_text;
        _str_init_(&message_text);
        str_append(&message_text, "Do you want to delete language \"", -1, 0);
        str_append(&message_text, active_language, -1, 0);
        str_append_char(&message_text, '\"');
        if(message_with_response(message_text.str)) {
            // delete language
            delete_language(active_language);
            set_chosen_language(NULL);
            update_languages_list();
            // empty titles and words lists
            empty_titles_list();
            empty_words_list();
        }
        str_free(&message_text);
    } else { // active language is not set
        String text;
        _str_init_(&text);
        str_append(&text, "No language chosen.", -1, 0);
        message_simple(text.str);
        str_free(&text);
    }
}

void button_language_callback(GtkWidget * button, void * para) {
    char * lang_name = (char *) para;
    change_language(lang_name);
    set_chosen_language(lang_name);
    set_chosen_title(get_active_title());
    update_titles_list();
    update_words_list();
}

// title callbacks
void button_add_title_callback(GtkWidget * button, void * para) {
    const char * tmp_entry_text = gtk_entry_get_text((GtkEntry *) m_window.window_titles.title_entry);
    char * entry_text;
    String str_entry_text;
    _str_init_(&str_entry_text);
    str_append(&str_entry_text, tmp_entry_text, -1, 0);
    entry_text = str_entry_text.str;
    // add new title
    int res = add_title(entry_text, 0);
    if(!res) { // added successfully
        set_chosen_title(entry_text);
        update_titles_list();
        update_words_list();
    } else if(res == TITLE_EXISTS_ERROR) { // title already exists
        String text;
        _str_init_(&text);
        str_append(&text, "Title \"", -1, 0);
        str_append(&text, entry_text, -1, 0);
        str_append(&text, "\" already exists.", -1, 0);
        message_simple(text.str);
        str_free(&text);
    } else { // language is not set
        String text;
        _str_init_(&text);
        str_append(&text, "No language chosen.", -1, 0);
        message_simple(text.str);
        str_free(&text);
    }
    empty_title_entry();
    str_free(&str_entry_text);
}

void button_remove_title_callback(GtkWidget * button, void * para) {
    char * active_language = get_active_language();
    char * active_title = get_active_title();
    if(active_language && active_title) { // successful deletion
        // create validation message
        String message_text;
        _str_init_(&message_text);
        str_append(&message_text, "Do you want to delete title \"", -1, 0);
        str_append(&message_text, active_title, -1, 0);
        str_append_char(&message_text, '\"');
        if(message_with_response(message_text.str)) {
            // delete title
            delete_title(active_title);
            set_chosen_title(NULL);
            update_titles_list();
            update_words_list();
        }
        str_free(&message_text);
    } else if(!active_language) {
        String text;
        _str_init_(&text);
        str_append(&text, "No language chosen.", -1, 0);
        message_simple(text.str);
        str_free(&text);
    } else { // null title change attempt
        String text;
        _str_init_(&text);
        str_append(&text, "NULL title can't be deleted.", -1, 0);
        message_simple(text.str);
        str_free(&text);
    }
}

void button_change_title_callback(GtkWidget * button, void * para) {
    char * active_title = get_active_title();
    GtkWidget * label, * entry;
    GtkWidget * dialog_content = set_grid_cells(7, 1);
    // check if language is set
    if(!get_active_language()) {
        String text;
        _str_init_(&text);
        str_append(&text, "No language chosen.", -1, 0);
        message_simple(text.str);
        str_free(&text);
        return;
    }
    // check if title is NULL
    if(!get_active_title()) {
        String text;
        _str_init_(&text);
        str_append(&text, "NULL title can't be changed.", -1, 0);
        message_simple(text.str);
        str_free(&text);
        return;
    }
    // set dialog content
    label = gtk_label_new("Title:");
    gtk_grid_attach((GtkGrid *) dialog_content, label, 0, 0, 2, 1);
    entry = gtk_entry_new();
    gtk_entry_set_text((GtkEntry *) entry, active_title);
    gtk_grid_attach((GtkGrid *) dialog_content, entry, 2, 0, 4, 1);
    // create parameter for a change title dialog
    UI_Change_Parameter func_para;
    func_para.widgets_array = (GtkWidget **) malloc(sizeof(GtkWidget *));
    (func_para.widgets_array)[0] = entry;
    func_para.current_word = NULL;
    func_para.array_size = 1;
    // create a dialog
    message_change(dialog_content, &func_para, &dialog_change_title_callback, "Change title");
    // free resources
    free(func_para.widgets_array);
}

void button_title_callback(GtkWidget * button, void * para) {
    char * title_name = (char *) para;
    choose_title(title_name);
    set_chosen_title(get_active_title());
    update_words_list();
}

// word callbacks
void button_add_word_callback(GtkWidget * button, void * para) {
    GtkWidget * entry_word = m_window.window_words.word_entry;
    GtkWidget * entry_definition = m_window.window_words.definition_entry;
    const char * word = gtk_entry_get_text((GtkEntry *) entry_word);
    const char * definition = gtk_entry_get_text((GtkEntry *) entry_definition);
    String str_word_entry, str_definition_entry;
    _str_init_(&str_word_entry);
    str_append(&str_word_entry, word, -1, 0);
    _str_init_(&str_definition_entry);
    str_append(&str_definition_entry, definition, -1, 0);
    // add the word
    int res = add_word(str_word_entry.str, str_definition_entry.str, 0);
    if(!res) { // added successfully
        update_titles_list();
        update_words_list();
    } else if(res == LANGUAGE_IS_NOT_SET) { // no language was chosen
        String text;
        _str_init_(&text);
        str_append(&text, "No language chosen.", -1, 0);
        message_simple(text.str);
        str_free(&text);
    } else if(res == WORD_EXISTS_ERROR) { // such a word already exists
        String text;
        _str_init_(&text);
        str_append(&text, "Word \"", -1, 0);
        str_append(&text, str_word_entry.str, -1, 0);
        str_append(&text, "\" already exists.", -1, 0);
        message_simple(text.str);
        str_free(&text);
    } else { // incorrect word or definition
        String text;
        _str_init_(&text);
        str_append(&text, "Word or definition can't be an empty string.", -1, 0);
        message_simple(text.str);
        str_free(&text);
        return;
    }
    empty_word_entry();
    str_free(&str_word_entry);
    str_free(&str_definition_entry);
}

void button_remove_word_callback(GtkWidget * button, void * para) {
    char * word = (char *) para;
    // create validation message
    String message_text;
    _str_init_(&message_text);
    str_append(&message_text, "Do you want to delete word \"", -1, 0);
    str_append(&message_text, word, -1, 0);
    str_append_char(&message_text, '\"');
    if(message_with_response(message_text.str)) {
        // delete word
        delete_word(word);
        update_titles_list();
        update_words_list();
    }
    str_free(&message_text);
}

void button_change_word_callback(GtkWidget * button, void * para) {
    char * word = (char *) para;
    GtkWidget * label_word, * label_definition;
    GtkWidget * entry_word, * entry_definition;
    GtkWidget * dialog_content = set_grid_cells(10, 2);
    // find definition of the word
    Wrapper_Word word_with_definition;
    word_with_definition.word = word;
    wrapper_find_word(&word_with_definition); // definition will be set in this function
    // set dialog content
    label_word = gtk_label_new("Word:");
    gtk_grid_attach((GtkGrid *) dialog_content, label_word, 0, 0, 3, 1);
    entry_word = gtk_entry_new();
    gtk_entry_set_text((GtkEntry *) entry_word, word_with_definition.word);
    gtk_grid_attach((GtkGrid *) dialog_content, entry_word, 3, 0, 6, 1);
    label_definition = gtk_label_new("Definition:");
    gtk_grid_attach((GtkGrid *) dialog_content, label_definition, 0, 1, 3, 1);
    entry_definition = gtk_entry_new();
    gtk_entry_set_text((GtkEntry *) entry_definition, word_with_definition.definition);
    gtk_grid_attach((GtkGrid *) dialog_content, entry_definition, 3, 1, 6, 1);
    // create parameter for a change title dialog
    UI_Change_Parameter func_para;
    func_para.widgets_array = (GtkWidget **) malloc(2 * sizeof(GtkWidget *));
    (func_para.widgets_array)[0] = entry_word;
    (func_para.widgets_array)[1] = entry_definition;
    func_para.current_word = word;
    func_para.array_size = 2;
    // create a dialog
    message_change(dialog_content, &func_para, &dialog_change_word_callback, "Change word");
    // free resources
    free(func_para.widgets_array);
}

// search callback
void button_search_callback(GtkWidget * button, void * para) {
    
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

// misc
void destroy_all_children(GtkWidget * parent) {
    GList * children, * iter;
    children = gtk_container_get_children((GtkContainer *) parent);
    for(iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy((GtkWidget *) iter->data);
    g_list_free(children);
}

void set_font(GtkWidget * widget) {
    // create Pango font description
    PangoFontDescription * desc = pango_font_description_new();
    pango_font_description_set_size(desc, m_window.text_size * PANGO_SCALE);
    // add Pango font to the entries
    gtk_widget_override_font(widget, desc);
    // free Pango font description
    pango_font_description_free(desc);
}
