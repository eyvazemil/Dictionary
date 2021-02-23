#include "wrapper.h"

void * get_language(void * (*func_para)(void *), void * key) {
    Language * language_key = (Language *) key;
    Wrapper_Language_Title tmp_language;
    tmp_language.name = (language_key->language_name).str;
    tmp_language.word_count = language_key->word_count;
    (*func_para)(&tmp_language);
    return NULL;
}

void * get_title(void * (*func_para)(void *), void * key) {
    Title * title_key = (Title *) key;
    Wrapper_Language_Title tmp_title;
    tmp_title.name = (title_key->title_name).str;
    tmp_title.word_count = title_key->word_count;
    (*func_para)(&tmp_title);
    return NULL;
}

void * get_word(void * (*func_para)(void *), void * key) {
    Word * word_key = (Word *) key;
    Wrapper_Word tmp_word;
    tmp_word.word = (word_key->m_word).str;
    tmp_word.definition = (word_key->m_definition).str;
    (*func_para)(&tmp_word);
    return NULL;
}

void get_languages(void * (*func)(void *)) {
    in_order_get_func_para(&(get_m_dictionary()->language_tree), func, &get_language);
}

void get_titles(void * (*func)(void *)) {
    in_order_get_func_para(&(get_m_active_language()->title_tree), func, &get_title);
}

void get_words(void * (*func)(void *)) {
    Word * tmp_word = get_m_active_title()->first_word_ptr;
    while(tmp_word) {
        get_word(func, (void *) tmp_word);
        tmp_word = tmp_word->next_word;
    }
}

char * get_active_language(void) {
    Language * lang = get_m_active_language();
    if(!lang) return NULL;
    return (lang->language_name).str;
}

char * get_active_title(void) {
    Title * title = get_m_active_title();
    if(!title) return NULL;
    return (title->title_name).str;
}

void wrapper_find_word(Wrapper_Word * para_word) {
    Word * found_word = find_word(para_word->word);
    para_word->definition = (found_word->m_definition).str;
}

// debug
void * print_language(void * key) {
    Wrapper_Language_Title * lang = (Wrapper_Language_Title *) key;
    printf("Language: %s (%d)\n", lang->name, lang->word_count);
    return NULL;
}

void * print_title(void * key) {
    Wrapper_Language_Title * title = (Wrapper_Language_Title *) key;
    printf("Title: %s (%d)\n", title->name, title->word_count);
    return NULL;
}

void * print_word(void * key) {
    Wrapper_Word * word = (Wrapper_Word *) key;
    printf("%s - %s\n", word->word, word->definition);
    return NULL;
}
