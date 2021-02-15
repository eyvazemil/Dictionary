//#include "main_window.h"
#include "backend.h"

int main(int argc, char ** argv) {

    /*
    // initialize gtk
    gtk_init(&argc, &argv);
    // initialize backend
    initialize(argv[1]);
    // create a main window and all the objects in it
    main_window();
    // main event loop
    gtk_main();
    // free memory for front end(list_words)
    memory_free();
    // finalize the backend
    finish();
    */
    initialize(argv[1]);
    change_language("French");
    traverse();
    //delete_language("Spanish");
    add_language("Spanish");
    change_language("Spanish");
    add_word("GTX", "RTX", 0);
    add_word("A", "B", 0);
    change_word("A", NULL, "V", "C");
    add_title("new:", 0);
    add_word("B-B:-", "-:C-V", 0);
    add_word("G", "T", 0);
    traverse();
    //change_language("French");*/
    finish();

    return 0;
}
