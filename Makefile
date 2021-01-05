CXX = gcc
OPT = `pkg-config --libs --cflags gtk+-3.0`
OBJECTS = main.o backend.o string.o avl_tree.o #main_window.o
RM = rm -f
OUT = dictionary

all: compile

main.o: ./src/main.c #./main_window.o
	$(CXX) -c ./src/main.c $(OPT)

main_window.o: ./src/main_window.h ./src/main_window.c ./backend.o
	$(CXX) -c ./src/main_window.c $(OPT)

backend.o: ./src/backend.h ./src/backend.c ./string.o ./avl_tree.o
	$(CXX) -c ./src/backend.c $(OPT)

string.o: ./src/string.h ./src/string.c
	$(CXX) -c ./src/string.c $(OPT)

avl_tree.o: ./src/avl_tree.h ./src/avl_tree.c
	$(CXX) -c ./src/avl_tree.c $(OPT)

compile: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUT) $(OPT)

run:
	./$(OUT)

clean: $(OUT)
	$(RM) *.o
	$(RM) $(OUT)