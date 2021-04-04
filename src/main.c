#include "main_window.h"
#include "wrapper.h"

int main(int argc, char ** argv) {

    // initialize gtk
    gtk_init(&argc, &argv);
    // initialize backend
    initialize(argv[1]);
    // create a main window and all the objects in it
    char ** args = (char **) malloc(3 * sizeof(char *));
    args[0] = argv[2];
    args[1] = argv[3];
    args[2] = argv[4];
    main_window(args);
    free(args);
    // main event loop
    gtk_main();
    // finalize the backend
    finish();
    
    return 0;
}
