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
    open_file("Spanish");
    //if(change_word(NULL, "V", "book", "C", "A"))
    //    puts("Already exists");
    if(add_title(NULL, 0))
        puts("Can't add title");
    if(delete_title("book"))
        puts("Can't delete title");
    traverse();
    finish();

    return 0;
}
