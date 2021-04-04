CXX = gcc
OPT = `pkg-config --libs --cflags gtk+-3.0`
DEBUG_OPTS = -fsanitize=address
OBJECTS = main.o backend.o wrapper.o string.o avl_tree.o main_window.o
RM = rm -f
OUT = dictionary
BASE_DIR = "/home/emil/Documents/Programming/GTK_Files/Dictionary"
WINDOW_GRID_WIDTH = 28
WINDOW_GRID_HEIGHT = 16
TEXT_SIZE = 11

all: compile

main.o: ./src/main.c #./main_window.o
	$(CXX) -c ./src/main.c $(OPT)

main_window.o: ./src/main_window.h ./src/main_window.c ./backend.o
	$(CXX) -c ./src/main_window.c $(OPT)

wrapper.o: ./src/wrapper.h ./src/wrapper.c ./backend.o
	$(CXX) -c -lpthread ./src/wrapper.c $(OPT)

backend.o: ./src/backend.h ./src/backend.c ./string.o ./avl_tree.o
	$(CXX) -c -lpthread ./src/backend.c $(OPT)

string.o: ./src/string.h ./src/string.c
	$(CXX) -c ./src/string.c $(OPT)

avl_tree.o: ./src/avl_tree.h ./src/avl_tree.c
	$(CXX) -c ./src/avl_tree.c $(OPT)

compile: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUT) $(OPT)

run:
	./$(OUT) $(BASE_DIR) $(WINDOW_GRID_WIDTH) $(WINDOW_GRID_HEIGHT) $(TEXT_SIZE)

clean: $(OUT)
	$(RM) *.o
	$(RM) $(OUT)