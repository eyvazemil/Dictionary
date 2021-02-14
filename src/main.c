//#include "main_window.h"
#include "backend.h"

int main(int argc, char ** argv) {

    /*
    // initialize gtk
    gtk_init(&argc, &argv);
    // initialize backend
    initialize();
    // create a main window and all the objects in it
    main_window();
    // main event loop
    gtk_main();
    // free memory for front end(list_words)
    memory_free();
    // finalize the backend
    finish();
    */
    initialize();
    change_language("French");
    traverse();
    change_language("Spanish");
    /*change_language("French");
    add_word("GTX", "RTX", 0);
    traverse();
    add_language("Spanish");
    add_word("A", "B", 0);
    add_title("new:", 0);
    add_word("B-B:-", "-:C-V", 0);
    //change_word("B", NULL, "V", "C");
    add_word("G", "T", 0);
    traverse();
    //delete_title("new");
    traverse();
    //delete_language("Spanish");
    traverse();
    //change_language("French");*/
    traverse();
    finish();

    return 0;
}
