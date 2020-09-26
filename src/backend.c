#include "backend.h"

Dictionary * m_dictionary = NULL; // actual dictionary object

void initialize(void) {
    struct dirent * pdir;
    DIR * _directory = opendir(LANGUAGES_DIR_NAME);
    // recurse further
    while(pdir = readdir(_directory)) {
        size_t tmp_len = strlen(pdir->d_name);
        if((pdir->d_name)[tmp_len - 1] == '.' ||
            ((pdir->d_name)[tmp_len - 1] == '.' && (pdir->d_name)[tmp_len - 2] == '.')) continue;
        // creating language in the dictionary
        char * lang = (char *) malloc(strlen(pdir->d_name) - 3); // do not include ".txt" extension
        memset(lang, '\0', strlen(pdir->d_name) - 3);
        strncpy(lang, pdir->d_name, strlen(pdir->d_name) - 4);
        // allocate memory for new language
        Dictionary * tmp = m_dictionary, * new_lang;
        new_lang = (Dictionary *) malloc(sizeof(Dictionary));
        new_lang->language_name = lang;
        new_lang->flag_opened = 0;
        new_lang->this_language = NULL;
        new_lang->prev_language = NULL;
        new_lang->next_language = NULL;
        if(tmp) {
            while(1) {
                if(strcmp(new_lang->language_name, tmp->language_name) < 0) {
                    new_lang->next_language = tmp;
                    new_lang->prev_language = tmp->prev_language;
                    if(tmp->prev_language)
                        tmp->prev_language->next_language = new_lang;
                    tmp->prev_language = new_lang;
                    if(m_dictionary == tmp)
                        m_dictionary = new_lang;
                    break;
                }
                if(!tmp->next_language) {
                    new_lang->next_language = NULL;
                    new_lang->prev_language = tmp;
                    tmp->next_language = new_lang;
                    break;
                }
                tmp = tmp->next_language;
            }
        } else {
            // there are no languages yet
            m_dictionary = new_lang;
        }
    }
    closedir(_directory);
}

void finish(void) {
    Dictionary * tmp = m_dictionary, * tmp_prev;
    while(tmp) {
        tmp_prev = tmp;
        tmp = tmp->next_language;
        if(tmp_prev->flag_opened)
            close_file(tmp_prev->language_name);
        m_dictionary = tmp;
        free(tmp_prev->language_name);
        free(tmp_prev);
    }
    m_dictionary = NULL;
}

void open_file(char * lang) {
    // find this language
    Dictionary * tmp = find_language(lang, NULL);
    if(!tmp) return;
    // set flag_opened to 1
    tmp->flag_opened = 1;
    // parse the file
    Language * new_word = NULL, * prev_word = NULL;
    char * word = NULL, * definition = NULL;
    size_t len = 1, count = 0; // for reallocation purposes
    FILE * fp;
    char c[1];
    int flag = 0; // set to 0 mean to read a word, 1 read a definition
    char * openfile = (char *) malloc(strlen(LANGUAGES_DIR_NAME) + strlen(lang) + 5);
    memset(openfile, '\0', strlen(LANGUAGES_DIR_NAME) + strlen(lang) + 5);
    strcpy(openfile, LANGUAGES_DIR_NAME);
    strncat(openfile, lang, strlen(lang));
    strcat(openfile, ".txt");
    fp = fopen(openfile, "r");
    free(openfile);
    // read words
    while(fread(c, sizeof(char), 1, fp)) {
        if(c[0] == '\n') {
            definition[count] = '\0';
            // create a new word and connect it with previous one
            new_word = (Language *) malloc(sizeof(Language));
            new_word->_word = word;
            new_word->_definition = definition;
            new_word->prev_word = prev_word;
            if(prev_word)
                prev_word->next_word = new_word;
            new_word->next_word = NULL;
            prev_word = new_word;
            // if it is the first word then connect it with the language
            if(!(tmp->this_language))
                tmp->this_language = new_word;
            // reset function variables
            flag = 0;
            count = 0;
            len = 1;
            word = NULL;
            definition = NULL;
            continue;
        }
        if(c[0] == '-') {
            word[count - 1] = '\0';
            fseek(fp, 1, SEEK_CUR); // skip space
            flag = 1;
            count = 0;
            len = 1;
            continue;
        }
        if(!flag) {
            // read a word
            if(count == len - 1) {
                size_t size = len *= 2;
                word = (char *) realloc(word, size * sizeof(char));
            }
            word[count] = c[0];
            count++;
        } else {
            // read a definition
            if(count == len - 1) {
                size_t size = len *= 2;
                definition = (char *) realloc(definition, size * sizeof(char));
            }
            definition[count] = c[0];
            count++;
        }
    }
    fclose(fp);
}

void close_file(char * lang) {
    // find this language
    Dictionary * tmp = find_language(lang, NULL);
    if(!tmp) return;
    // set flag_opened to 0
    tmp->flag_opened = 0;
    // close the file
    FILE * fp;
    char * openfile = (char *) malloc(strlen(LANGUAGES_DIR_NAME) + strlen(lang) + 5);
    memset(openfile, '\0', strlen(LANGUAGES_DIR_NAME) + strlen(lang) + 5);
    strcpy(openfile, LANGUAGES_DIR_NAME);
    strncat(openfile, lang, strlen(lang));
    strcat(openfile, ".txt");
    fp = fopen(openfile, "w");
    free(openfile);
    // write words to the file
    char * str_to_write;
    Language * current_word = tmp->this_language, * prev_word;
    if(!current_word) {
        fclose(fp);
        return;
    }
    tmp->this_language = NULL; // set the words in the language to NULL
    while(current_word) {
        // create a string to write
        size_t size = strlen(current_word->_word) + strlen(current_word->_definition) + 5;
        str_to_write = (char *) malloc(size * sizeof(char));
        memset(str_to_write, '\0', size);
        strncpy(str_to_write, current_word->_word, strlen(current_word->_word));
        strcat(str_to_write, " - ");
        strncat(str_to_write, current_word->_definition, strlen(current_word->_definition));
        strcat(str_to_write, "\n");
        // actually write to the file
        fwrite(str_to_write, 1, strlen(str_to_write), fp);
        // free memory and go to the next word
        prev_word = current_word;
        current_word = current_word->next_word;
        free(str_to_write);
        free(prev_word->_word);
        free(prev_word->_definition);
        free(prev_word);
    }
    fclose(fp);
}

int add_language(char * lang, int * para_count) {
    // check if such a language already exists
    Dictionary * check = find_language(lang, para_count); // the position of language in the list
    if(check) return LANGUAGE_EXISTS_ERROR;
    *para_count = 0;
    // create a file
    FILE * fp;
    char * openfile = (char *) malloc(strlen(LANGUAGES_DIR_NAME) + strlen(lang) + 5);
    memset(openfile, '\0', strlen(LANGUAGES_DIR_NAME) + strlen(lang) + 5);
    strcpy(openfile, LANGUAGES_DIR_NAME);
    strncat(openfile, lang, strlen(lang));
    strcat(openfile, ".txt");
    fp = fopen(openfile, "w");
    fclose(fp);
    free(openfile);
    // language name
    char * new_name = (char *) malloc(strlen(lang) + 1);
    memset(new_name, '\0', strlen(lang) + 1);
    strncpy(new_name, lang, strlen(lang));
    // allocate memory for new language
    Dictionary * tmp = m_dictionary, * new_lang;
    new_lang = (Dictionary *) malloc(sizeof(Dictionary));
    new_lang->language_name = new_name;
    new_lang->this_language = NULL;
    new_lang->prev_language = NULL;
    new_lang->next_language = NULL;
    if(tmp) {
        while(1) {
            if(strcmp(new_lang->language_name, tmp->language_name) < 0) {
                new_lang->next_language = tmp;
                new_lang->prev_language = tmp->prev_language;
                if(tmp->prev_language)
                    tmp->prev_language->next_language = new_lang;
                tmp->prev_language = new_lang;
                if(m_dictionary == tmp)
                        m_dictionary = new_lang;
                break;
            }
            if(!tmp->next_language) {
                new_lang->next_language = NULL;
                new_lang->prev_language = tmp;
                tmp->next_language = new_lang;
                (*para_count)++; // new position of the language in the list
                break;
            }
            tmp = tmp->next_language;
            (*para_count)++; // new position of the language in the list
        }
    } else {
        // there are no languages yet
        m_dictionary = new_lang;
    }
    return 0;
}

void delete_language(char * lang, int * para_count) {
    // delete a file
    FILE * fp;
    char * openfile = (char *) malloc(strlen(LANGUAGES_DIR_NAME) + strlen(lang) + 5);
    memset(openfile, '\0', strlen(LANGUAGES_DIR_NAME) + strlen(lang) + 5);
    strcpy(openfile, LANGUAGES_DIR_NAME);
    strncat(openfile, lang, strlen(lang));
    strcat(openfile, ".txt");
    remove(openfile);
    free(openfile);
    // find language
    Dictionary * tmp = find_language(lang, para_count); // the position of language in the list
    if(!tmp) return;
    if(m_dictionary == tmp)
        m_dictionary = tmp->next_language;
    if(tmp->prev_language)
        tmp->prev_language->next_language = tmp->next_language;
    if(tmp->next_language)
        tmp->next_language->prev_language = tmp->prev_language;
    // deallocate
    Language * word = tmp->this_language, * word_tmp;
    while(word) {
        word_tmp = word;
        word = word->next_word;
        free(word_tmp->_word);
        free(word_tmp->_definition);
        free(word_tmp);
    }
    free(tmp->language_name);
    free(tmp);
}

Dictionary * find_language(char * lang, int * para_count) {
    Dictionary * tmp = m_dictionary;
    while(tmp) {
        if(!strcmp(tmp->language_name, lang)) return tmp;
        tmp = tmp->next_language;
        if(para_count) (*para_count)++;
    }
    return NULL;
}

Language * find_word(char * word, char * lang, int * para_count) {
    Dictionary * tmp = find_language(lang, NULL);
    if(!tmp) return NULL;
    Language * tmp_lang = tmp->this_language;
    while(tmp_lang) {
        if(!strcmp(tmp_lang->_word, word)) return tmp_lang;
        tmp_lang = tmp_lang->next_word;
        if(para_count) (*para_count)++;
    }
    return NULL;
}

int add_word(char * word_str, char * def_str, char * lang, int * para_count) {
    Dictionary * tmp = find_language(lang, NULL);
    if(!tmp) return 1;
    // check if such a new word already exists
    Language * check = find_word(word_str, lang, para_count); // the position of word in the list
    if(check) return WORD_EXISTS_ERROR;
    *para_count = 0;
    // create a new word
    Language * new_language = (Language *) malloc(sizeof(Language));
    char * new_word, * new_definition;
    new_word = (char *) malloc(strlen(word_str) + 1);
    memset(new_word, '\0', strlen(word_str) + 1);
    strncpy(new_word, word_str, strlen(word_str));
    new_definition = (char *) malloc(strlen(def_str) + 1);
    memset(new_definition, '\0', strlen(def_str) + 1);
    strncpy(new_definition, def_str, strlen(def_str));
    new_language->_word = new_word;
    new_language->_definition = new_definition;
    new_language->prev_word = NULL;
    new_language->next_word = NULL;
    // add word to the language
    Language * tmp_word = tmp->this_language, * tmp_prev = NULL;
    if(!tmp_word) {
        // if there are no words in the language
        tmp->this_language = new_language;
    } else {
        while(tmp_word) {
            tmp_prev = tmp_word;
            tmp_word = tmp_word->next_word;
        }
        new_language->prev_word = tmp_prev;
        tmp_prev->next_word = new_language;
    }
    return 0;
}

void delete_word(char * word, char * lang, int * para_count) {
    Dictionary * tmp = find_language(lang, NULL);
    if(!tmp) return;
    Language * tmp_lang = find_word(word, lang, para_count); // the position of word in the list
    if(!tmp_lang) return;
    if(tmp->this_language == tmp_lang) {
        // the found word is the first word in the language
        tmp->this_language = tmp_lang->next_word;
    }
    if(tmp_lang->next_word)
        tmp_lang->next_word->prev_word = tmp_lang->prev_word;
    if(tmp_lang->prev_word)
        tmp_lang->prev_word->next_word = tmp_lang->next_word;
    free(tmp_lang->_word);
    free(tmp_lang->_definition);
    free(tmp_lang);
}

int change_word(char * new_word, char * new_def, char * word, char * lang, int * para_count) {
    Dictionary * tmp = find_language(lang, NULL);
    if(!tmp) return 1;
    // check if such a new word already exists
    Language * check = find_word(new_word, lang, NULL);
    if(check) return WORD_EXISTS_ERROR;
    *para_count = 0;
    // change the word
    Language * tmp_word = find_word(word, lang, para_count);
    char * tmp_new_word = (char *) malloc(strlen(new_word) + 1);
    memset(tmp_new_word, '\0', strlen(new_word) + 1);
    strncpy(tmp_new_word, new_word, strlen(new_word));
    free(tmp_word->_word);
    tmp_word->_word = tmp_new_word;
    // change the definition
    char * tmp_new_def = (char *) malloc(strlen(new_def) + 1);
    memset(tmp_new_def, '\0', strlen(new_def) + 1);
    strncpy(tmp_new_def, new_def, strlen(new_def));
    free(tmp_word->_definition);
    tmp_word->_definition = tmp_new_def;
    return 0;
}

void change_definition(char * new_def, char * word, char * lang, int * para_count) {
    Dictionary * tmp = find_language(lang, NULL);
    if(!tmp) return;
    // change the definition
    Language * tmp_word = find_word(word, lang, para_count);
    char * tmp_new_def = (char *) malloc(strlen(new_def) + 1);
    memset(tmp_new_def, '\0', strlen(new_def) + 1);
    strncpy(tmp_new_def, new_def, strlen(new_def));
    free(tmp_word->_definition);
    tmp_word->_definition = tmp_new_def;
}

// wrapper stuff
Dictionary * get_m_dictionary(void) {
    return m_dictionary;
}

// debugging
void traverse(void) {
    puts("traverse:");
    Dictionary * tmp = m_dictionary;
    Language * tmp_word;
    while(tmp) {
        printf("%s:\n", tmp->language_name);
        tmp_word = tmp->this_language;
        while(tmp_word) {
            printf("|  %s - %s\n", tmp_word->_word, tmp_word->_definition);
            tmp_word = tmp_word->next_word;
        }
        tmp = tmp->next_language;
    }
}