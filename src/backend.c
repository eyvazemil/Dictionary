#include "backend.h"

static String * BASE_DIR;
static String * LANGUAGES_DIR;

Dictionary * m_dictionary = NULL; // actual dictionary object
Language * m_active_language = NULL; // currently active language
Title * m_active_title = NULL;

void initialize(char * base_dir) {
    struct dirent * pdir;
    DIR * _directory;
    // set base directory and languages directory
    BASE_DIR = (String *) malloc(sizeof(String));
    LANGUAGES_DIR = (String *) malloc(sizeof(String));
    _str_init_(BASE_DIR);
    str_append(BASE_DIR, base_dir, -1, 0);
    _str_init_(LANGUAGES_DIR);
    str_append(LANGUAGES_DIR, base_dir, -1, 0);
    str_append(LANGUAGES_DIR, "/Languages/", -1, 0);
    // synch with github
    String program_name;
    _str_init_(&program_name);
    str_append(&program_name, BASE_DIR->str, -1, 0);
    str_append(&program_name, "/src/_git_clone_.sh", -1, 0);
    run_exec(&program_name);
    str_free(&program_name);
    // open directory
    _directory = opendir(LANGUAGES_DIR->str);
    // initialize Directory structure
    m_dictionary = (Dictionary *) malloc(sizeof(Dictionary));
    _avl_init_(&(m_dictionary->language_tree));
    assign_compare_func(&(m_dictionary->language_tree), &avl_compare_language);
    assign_free_func(&(m_dictionary->language_tree), &avl_free_language);
    // recurse further
    while(pdir = readdir(_directory)) {
        // read file name
        size_t tmp_len = strlen(pdir->d_name);
        if((pdir->d_name)[tmp_len - 1] == '.' || (pdir->d_name)[0] == '.' ||
            ((pdir->d_name)[tmp_len - 1] == '.' && (pdir->d_name)[tmp_len - 2] == '.')) continue;
        // creating language in the dictionary
        Language * new_lang = create_language(pdir->d_name, strlen(pdir->d_name) - 4);
        insert(&(m_dictionary->language_tree), (void *) new_lang);
    }
    closedir(_directory);
    m_active_language = NULL;
    m_active_title = NULL;
}

void finish(void) {
    Language * tmp_active_language = m_active_language;
    close_file(tmp_active_language);
    // free language strings
    in_order_get(&(m_dictionary->language_tree), NULL, &free_language_strings);
    // free dictionary
    avl_free_dictionary();
    // set dictionary pointer to NULL
    m_dictionary = NULL;
    // synch with github
    String program_name;
    _str_init_(&program_name);
    str_append(&program_name, BASE_DIR->str, -1, 0);
    str_append(&program_name, "/src/_git_push_.sh", -1, 0);
    run_exec(&program_name);
    str_free(&program_name);
    // free resources
    str_free(BASE_DIR);
    str_free(LANGUAGES_DIR);
    free(BASE_DIR);
    free(LANGUAGES_DIR);
}

void change_language(char * lang) {
    // do not open the language, if it is already opened
    Language * found_language = find_language(lang);
    if(!found_language || found_language == m_active_language) return;
    // change language
    Language * tmp_active_language = m_active_language;
    void * status;
    pthread_t * threads[2];
    threads[0] = create_thread(&thread_close, (void *) tmp_active_language);
    threads[1] = create_thread(&thread_open, (void *) lang);
    for(int i = 0; i < 2; i++) {
        pthread_join(*(threads[i]), &status);
        free(threads[i]);
    }
}

void open_file(char * lang) {
    // find this language
    Language * found_language = find_language(lang);
    Title * found_title;
    if(!found_language || found_language == m_active_language) return;
    m_active_language = found_language; // set active language
    m_active_language->word_count = 0;
    // open a file
    FILE * fp;
    String openfile;
    _str_init_(&openfile);
    str_append(&openfile, LANGUAGES_DIR->str, -1, 0);
    str_append(&openfile, lang, -1, 0);
    str_append(&openfile, ".txt", -1, 0);
    fp = fopen(openfile.str, "r");
    str_free(&openfile);
    // variables to read the file content
    String word, definition, * title = NULL;
    char * line = NULL;
    size_t line_len = 0; // for line reading purposes
    int line_count;
    // parse the file
    while((line_count = getline(&line, &line_len, fp)) > 0) { // read till EOF
        line_count--; // do not include the last newline
        // parse the line
        if(line[line_count - 1] == ':' && (line_count == 1 || (line_count > 1 && line[line_count - 2] != '\\'))) { // title
            if(title) {
                str_free(title);
                free(title);
            }
            title = (String *) malloc(sizeof(String));
            _str_init_(title);
            //str_append(&title, line, 0, line_count - 1);
            for(int i = 0; i < line_count - 1; i++) { // truncate all '\' characters if it preceeds ':'
                if(i < line_count - 2 && line[i] == '\\' && line[i + 1] == ':') continue;
                str_append_char(title, line[i]);
            }
            add_title(title->str, 1);
            found_title = find_title(title->str);
            m_active_title = found_title;
        } else { // word
            // find '-'
            int dash_location;
            for(int i = 0; i < line_count; i++) {
                if(i > 0 && line[i] == '-' && line[i - 1] != '\\') {
                    dash_location = i;
                    break;
                }
            }
            // read word and definition
            _str_init_(&word);
            for(int i = 0; i < dash_location - 1; i++) {
                if(i < dash_location - 2 && line[i] == '\\' && (line[i + 1] == ':' || line[i + 1] == '-')) continue;
                str_append_char(&word, line[i]);
            }
            //str_append(&word, line, 0, dash_location - 1);
            _str_init_(&definition);
            for(int i = dash_location + 2; i < line_count; i++) {
                if(i < line_count - 1 && line[i] == '\\' && (line[i + 1] == ':' || line[i + 1] == '-')) continue;
                str_append_char(&definition, line[i]);
            }
            //str_append(&definition, line, dash_location + 2, line_count - dash_location - 2);
            // add word and definition
            add_word(word.str, definition.str, 1);
            // free word and definition resources
            str_free(&word);
            str_free(&definition);
        }
        // reset the line string
        free(line);
        line = NULL;
        line_len = 0;
    }
    m_active_title = find_title(NULL); // set active title to NULL
    if(title) {
        str_free(title);
        free(title);
    }
    fclose(fp);
}

void close_file(Language * tmp_active_language) {
    if(!tmp_active_language) return;
    // write titles and words into the file
    if(tmp_active_language->flag_modified) { // write to the file only if the language was modified
        // open a file
        FILE * fp;
        String openfile;
        _str_init_(&openfile);
        str_append(&openfile, LANGUAGES_DIR->str, -1, 0);
        str_append(&openfile, (tmp_active_language->language_name).str, -1, 0);
        str_append(&openfile, ".txt", -1, 0);
        fp = fopen(openfile.str, "w");
        str_free(&openfile);
        in_order_get(&(tmp_active_language->title_tree), (void *) fp, &write_title);
        fclose(fp);
    }
    tmp_active_language->flag_modified = 0; // reset modified bit
    tmp_active_language->word_count = 0; // reset word count
    avl_free_language((void *) tmp_active_language); // free resources
    tmp_active_language = NULL; // reset active language
}

Language * create_language(char * lang_name, int len) {
    Language * new_lang = (Language *) malloc(sizeof(Language));
    _str_init_(&(new_lang->language_name));
    str_append(&(new_lang->language_name), lang_name, 0, len);
    _avl_init_(&(new_lang->title_tree));
    assign_compare_func(&(new_lang->title_tree), &avl_compare_title);
    assign_free_func(&(new_lang->title_tree), &avl_free_title);
    new_lang->flag_modified = 0;
    new_lang->word_count = 0;
    return new_lang;
}

int add_language(char * lang) {
    // check if language name is empty
    if(!lang || !strcmp(lang, "")) return LANGUAGE_INCORRECT_NAME;
    // check if such a language already exists
    Language * found_language = find_language(lang);
    if(found_language) return LANGUAGE_EXISTS_ERROR;
    // create a file
    FILE * fp;
    String openfile;
    _str_init_(&openfile);
    str_append(&openfile, LANGUAGES_DIR->str, -1, 0);
    str_append(&openfile, lang, -1, 0);
    str_append(&openfile, ".txt", -1, 0);
    fp = fopen(openfile.str, "w");
    fclose(fp);
    str_free(&openfile);
    // close currently active language
    Language * tmp_active_language = m_active_language;
    void * status;
    pthread_t * thread = create_thread(&thread_close, (void *) tmp_active_language);
    // insert new language
    insert(&(m_dictionary->language_tree), (void *) create_language(lang, strlen(lang)));
    m_active_language = find_language(lang);
    add_title(NULL, 0); // add NONE title
    m_active_title = find_title(NULL); // set active title to NULL
    // join thread
    pthread_join(*thread, &status);
    free(thread);
    return 0;
}

void delete_language(char * lang) {
    Language * found_language = find_language(lang);
    if(!found_language) return;
    // delete a file
    String openfile;
    _str_init_(&openfile);
    str_append(&openfile, LANGUAGES_DIR->str, -1, 0);
    str_append(&openfile, lang, -1, 0);
    str_append(&openfile, ".txt", -1, 0);
    remove(openfile.str);
    str_free(&openfile);
    // reset active language
    if(m_active_language == found_language) {
        m_active_language = NULL;
        m_active_title = NULL;
    }
    // find language
    Language * para_language = create_language(lang, strlen(lang)); // parameter for AVL tree
    delete(&(m_dictionary->language_tree), (void *) para_language);
    free_language_strings(NULL, (void *) para_language);
    avl_free_language((void *) para_language);
}

Language * find_language(char * lang) {
    Language * para_language = create_language(lang, (strlen(lang))), * found_language; // parameter for finding key in AVL tree
    Node * avl_node = find_node(&(m_dictionary->language_tree), (void *) para_language);
    if(avl_node)
        found_language = (Language *) (avl_node->key); // key in AVL tree
    else found_language = NULL;
    free_language_strings(NULL, (void *) para_language);
    avl_free_language((void *) para_language);
    return found_language;
}

Title * create_title(char * title) {
    Title * new_title = (Title *) malloc(sizeof(Title));
    _str_init_(&(new_title->title_name));
    str_append(&(new_title->title_name), title, -1, 0);
    _avl_init_(&(new_title->word_tree));
    assign_compare_func(&(new_title->word_tree), &avl_compare_word);
    assign_free_func(&(new_title->word_tree), &avl_free_word);
    new_title->first_word_ptr = NULL;
    new_title->last_word_ptr = NULL;
    new_title->word_count = 0;
    return new_title;
}

int add_title(char * title, int flag_open) {
    Language * found_language = m_active_language;
    if(!found_language) return LANGUAGE_IS_NOT_SET;
    Title * found_title = find_title(title);
    if(found_title) return TITLE_EXISTS_ERROR;
    // if this function is not called from open_file function, set modified flag to 1
    if(!flag_open)
        found_language->flag_modified = 1;
    // insert new title
    insert(&(found_language->title_tree), (void *) create_title(title));
    // set active title
    m_active_title = find_title(title);
    return 0;
}

int delete_title(char * title) {
    Language * found_language = m_active_language;
    if(!found_language) return LANGUAGE_IS_NOT_SET;
    if(!title || !strcmp(title, "")) return NULL_TITLE_CHANGE_ATTEMPT;
    Title * found_title = find_title(title);
    // change active title to NULL, if this one was freed
    if(m_active_title == found_title)
        m_active_title = find_title(NULL);
    // delete title
    delete(&(found_language->title_tree), (void *) found_title);
    avl_free_title((void *) found_title);
    // set modified flag to 1
    found_language->flag_modified = 1;
    return 0;
}

Title * find_title(char * title) {
    Language * found_language = m_active_language;
    Title * para_title = create_title(title), * found_title;
    Node * avl_node = find_node(&(found_language->title_tree), (void *) para_title);
    if(avl_node)
        found_title = (Title *) (avl_node->key);
    else found_title = NULL;
    avl_free_title(para_title);
    return found_title;
}

void choose_title(char * title) {
    Title * found_title = find_title(title);
    if(!found_title) return;
    m_active_title = found_title;
}

int change_title_name(char * title, char * new_title) {
    if(!m_active_language) return LANGUAGE_IS_NOT_SET;
    if(!title || !strcmp(title, "")) return NULL_TITLE_CHANGE_ATTEMPT;
    Title * found_title, * found_new_title;
    Title * title1 = create_title(title), * title2 = create_title(new_title);
    if(avl_compare_title(title1, title2)) { // title name has changed
        found_new_title = find_title(new_title);
        if(found_new_title) return TITLE_EXISTS_ERROR;
        found_title = find_title(title);
        delete(&(m_active_language->title_tree), (void *) found_title);
        str_free(&(found_title->title_name));
        str_append(&(found_title->title_name), new_title, -1, 0);
        insert(&(m_active_language->title_tree), (void *) found_title);
        // set modified flag to 1
        m_active_language->flag_modified = 1;
    }
    // free resources
    avl_free_title((void *) title1);
    avl_free_title((void *) title2);
    return 0;
}

void * write_title(void * my_para, void * avl_key) {
    Title * title = (Title *) avl_key;
    FILE * fp = (FILE *) my_para;
    String line, title_name, correct_title;
    _str_init_(&line);
    _str_init_(&correct_title);
    title_name.str = (title->title_name).str;
    title_name.len = (title->title_name).len;
    if(title_name.str) {
        for(int i = 0; i < strlen(title_name.str); i++) {
            if((title_name.str)[i] == ':')
                str_append_char(&correct_title, '\\');
            str_append_char(&correct_title, (title_name.str)[i]);
        }
    }
    str_append(&line, correct_title.str, -1, 0);
    str_append(&line, ":\n", -1, 0);
    fwrite(line.str, strlen(line.str), 1, fp);
    str_free(&line);
    str_free(&correct_title);
    // write words into the file
    Word * tmp = title->last_word_ptr;
    while(tmp) {
        write_word(my_para, (void *) tmp);
        tmp = tmp->prev_word;
    }
    //in_order_get(&(title->word_tree), my_para, &write_word);
    return NULL;
}

Word * create_word(char * word, char * def) {
    Word * new_word = (Word *) malloc(sizeof(Word));
    _str_init_(&(new_word->m_word));
    str_append(&(new_word->m_word), word, -1, 0);
    _str_init_(&(new_word->m_definition));
    str_append(&(new_word->m_definition), def, -1, 0);
    new_word->prev_word = NULL;
    new_word->next_word = NULL;
    return new_word;
}

int add_word(char * word, char * def, int flag_open) {
    Language * found_language = m_active_language;
    Title * found_title = m_active_title;
    if(!found_language) return LANGUAGE_IS_NOT_SET;
    Word * found_word = find_word(word);
    if(found_word) return WORD_EXISTS_ERROR;
    if(!word || !strcmp(word, "") || !def || !strcmp(def, "")) return WORD_INCORRECT_NAME;
    // create new word and connect pointers in the linked list
    Word * new_word = create_word(word, def);
    new_word->next_word = found_title->first_word_ptr;
    if(found_title->first_word_ptr)
        found_title->first_word_ptr->prev_word = new_word;
    else found_title->last_word_ptr = new_word;
    found_title->first_word_ptr = new_word;
    // increase the counter of words
    (found_language->word_count)++;
    (found_title->word_count)++;
    // if this function is not called from open_file function, set modified flag to 1
    if(!flag_open)
        found_language->flag_modified = 1;
    // insert into AVL tree
    insert(&(found_title->word_tree), (void *) new_word);
    return 0;
}

void delete_word(char * word) {
    Language * found_language = m_active_language;
    Title * found_title = m_active_title;
    Word * found_word = find_word(word);
    // change pointers in the linked list
    if(found_title->first_word_ptr == found_word)
        found_title->first_word_ptr = found_word->next_word;
    if(found_title->last_word_ptr == found_word)
        found_title->last_word_ptr = found_word->prev_word;
    // increase the counter of words
    (found_language->word_count)--;
    (found_title->word_count)--;
    // set modified flag to 1
    found_language->flag_modified = 1;
    // delete from AVL tree
    delete(&(found_title->word_tree), (void *) found_word);
    avl_free_word(found_word);
}

int change_word(char * word, char * new_title, char * new_word, char * new_def) {
    Language * found_language = m_active_language;
    Title * title1 = create_title((m_active_title->title_name).str), * title2 = create_title(new_title);
    Title * current_title = m_active_title, * new_title_ptr = find_title(new_title);
    Word * found_word;
    if(avl_compare_title(title1, title2)) { // title has changed
        m_active_title = new_title_ptr;
        found_word = find_word(new_word);
        if(found_word) return WORD_EXISTS_ERROR;
        m_active_title = current_title;
        delete_word(word); // delete from the AVL tree
        m_active_title = new_title_ptr;
    } else { // the same title
        found_word = find_word(new_word);
        if(found_word) return WORD_EXISTS_ERROR;
        delete_word(word); // delete from the AVL tree
    }
    add_word(new_word, new_def, 0); // insert new word into AVL tree
    found_language->flag_modified = 1; // set modified flag to 1
    // free resources
    avl_free_title((void *) title1);
    avl_free_title((void *) title2);
    return 0;
}

Word * find_word(char * word) {
    Title * para_title = m_active_title;
    Word * para_word = create_word(word, NULL), * found_word;
    Node * avl_node = find_node(&(para_title->word_tree), (void *) para_word);
    if(avl_node)
        found_word = (Word *) (avl_node->key);
    else found_word = NULL;
    avl_free_word(para_word);
    return found_word;
}

void * write_word(void * my_para, void * avl_key) {
    Word * word = (Word *) avl_key;
    FILE * fp = (FILE *) my_para;
    String line, _word, _definition, correct_word, correct_definition;
    _str_init_(&line);
    _str_init_(&correct_word);
    _str_init_(&correct_definition);
    _word.str = (word->m_word).str;
    _word.len = (word->m_word).len;
    _definition.str = (word->m_definition).str;
    _definition.len = (word->m_definition).len;
    for(int i = 0; i < strlen(_word.str); i++) {
        if((_word.str)[i] == ':' || (_word.str)[i] == '-')
            str_append_char(&correct_word, '\\');
        str_append_char(&correct_word, (_word.str)[i]);
    }
    for(int i = 0; i < strlen(_definition.str); i++) {
        if((_definition.str)[i] == ':' || (_definition.str)[i] == '-')
            str_append_char(&correct_definition, '\\');
        str_append_char(&correct_definition, (_definition.str)[i]);
    }
    str_append(&line, correct_word.str, -1, 0);
    str_append(&line, " - ", -1, 0);
    str_append(&line, correct_definition.str, -1, 0);
    str_append_char(&line, '\n');
    fwrite(line.str, strlen(line.str), 1, fp);
    str_free(&line);
    str_free(&correct_word);
    str_free(&correct_definition);
    return NULL;
}

// thread functions
pthread_t * create_thread(void * (*func)(void *), void * para) {
    pthread_t * thread = (pthread_t *) malloc(sizeof(pthread_t));
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(thread, &attr, *func, (void *) para);
    pthread_attr_destroy(&attr);
    return thread;
}

void * thread_close(void * para) {
    close_file((Language *) para);
    pthread_exit(NULL);
}

void * thread_open(void * para) {
    open_file((char *) para);
    pthread_exit(NULL);
}

// AVL tree functions
int avl_compare_language(const void * para1, const void * para2) {
    String arg1 = ((Language *) para1)->language_name;
    String arg2 = ((Language *) para2)->language_name;
    if(!arg1.str || !arg2.str) {
        if(!arg1.str && arg2.str && strcmp(arg2.str, "")) return -1;
        if(arg1.str  && strcmp(arg1.str, "") && !arg2.str) return 1;
    } else {
        if(strcmp(arg1.str, arg2.str) < 0) return -1;
        if(strcmp(arg1.str, arg2.str) > 0) return 1;
    }
    return 0;
}

int avl_compare_title(const void * para1, const void * para2) {
    String arg1 = ((Title *) para1)->title_name;
    String arg2 = ((Title *) para2)->title_name;
    if(!arg1.str || !arg2.str) {
        if(!arg1.str && arg2.str && strcmp(arg2.str, "")) return -1;
        if(arg1.str  && strcmp(arg1.str, "") && !arg2.str) return 1;
    } else {
        if(strcmp(arg1.str, arg2.str) < 0) return -1;
        if(strcmp(arg1.str, arg2.str) > 0) return 1;
    }
    return 0;
}

int avl_compare_word(const void * para1, const void * para2) {
    String arg1 = ((Word *) para1)->m_word;
    String arg2 = ((Word *) para2)->m_word;
    if(!arg1.str || !arg2.str) {
        if(!arg1.str && arg2.str && strcmp(arg2.str, "")) return -1;
        if(arg1.str  && strcmp(arg1.str, "") && !arg2.str) return 1;
    } else {
        if(strcmp(arg1.str, arg2.str) < 0) return -1;
        if(strcmp(arg1.str, arg2.str) > 0) return 1;
    }
    return 0;
}

void avl_free_dictionary(void) {
    free_tree(&(m_dictionary->language_tree));
    free(m_dictionary);
}

void avl_free_language(void * para) {
    Language * tmp_lang = (Language *) para;
    free_tree(&(tmp_lang->title_tree));
    //str_free(&(tmp_lang->language_name));
    //free(tmp_lang);
}

void avl_free_title(void * para) {
    Title * tmp_title = (Title *) para;
    free_tree(&(tmp_title->word_tree));
    str_free(&(tmp_title->title_name));
    free(tmp_title);
}

void avl_free_word(void * para) {
    Word * tmp_word = (Word *) para;
    str_free(&(tmp_word->m_word));
    str_free(&(tmp_word->m_definition));
    if(tmp_word->prev_word)
        tmp_word->prev_word->next_word = tmp_word->next_word;
    if(tmp_word->next_word)
        tmp_word->next_word->prev_word = tmp_word->prev_word;
    free(tmp_word);
}

void * avl_print_language(void * my_para, void * avl_key) {
    Language * tmp = (Language *) avl_key;
    printf("%s:\n", (tmp->language_name).str);
    in_order_get(&(tmp->title_tree), NULL, &avl_print_title);
    return NULL;
}

void * avl_print_title(void * my_para, void * avl_key) {
    Title * tmp = (Title *) avl_key;
    printf("..%s:\n", (tmp->title_name).str);
    Word * word = tmp->first_word_ptr;
    while(word) {
        printf("....%s - %s\n", (word->m_word).str, (word->m_definition).str);
        word = word->next_word;
    }
    //in_order_get(&(tmp->word_tree), NULL, &avl_print_word);
    return NULL;
}

void * avl_print_word(void * my_para, void * avl_key) {
    Word * tmp = (Word *) avl_key;
    printf("....%s - %s\n", (tmp->m_word).str, (tmp->m_definition).str);
    return NULL;
}

void * free_language_strings(void * my_para, void * avl_key) {
    Language * tmp = (Language *) avl_key;
    str_free(&(tmp->language_name));
    free(tmp);
    return NULL;
}

// wrapper stuff
Dictionary * get_m_dictionary(void) {
    return m_dictionary;
}

Language * get_m_active_language(void) {
    return m_active_language;
}

Title * get_m_active_title(void) {
    return m_active_title;
}

// github synchronization
void run_exec(String * program_name) {
    int pid, signal_received;
    char * ppid = int_to_str(getpid());
    sigset_t signals_set;
    sigemptyset(&signals_set); // reset the signals set
    sigaddset(&signals_set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &signals_set, NULL);
    if(!(pid = fork())) {
        /* child */
        execl(program_name->str, program_name->str, BASE_DIR->str, ppid, (char *) NULL);
        exit(1);
    }
    // wait for a signal from child process
    sigwait(&signals_set, &signal_received);
    free(ppid);
}

// debugging
void traverse(void) {
    puts("__traverse:__");
    if(m_dictionary)
        in_order_get(&(m_dictionary->language_tree), NULL, &avl_print_language);
}

// misc
char * int_to_str(int num) {
    char * str = NULL;
	int para_num = num, count = 0, len = 1;
	char tmp;
    if(!num) {
        str = (char *) malloc(2 * sizeof(char));
        str[0] = '0';
        str[1] = '\0';
        return str;
    }
	while(para_num) {
        if(count == len - 1)
            str = (char *) realloc(str, (len *= 2) * sizeof(char));
		str[count] = (char) (para_num % 10 + 48);
		para_num /= 10;
		count++;
	}
    str[count] = '\0';
	str_reverse(str, count);
    return str;
}

void str_reverse(char * str, int count) {
	char tmp;
	for(int i = count - 1, j = 0; i > j; i--, j++) {
		tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;
	}
}
