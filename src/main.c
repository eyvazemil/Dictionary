#include "main_window.h"


int main(int argc, char ** argv) {

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

    return 0;
}
