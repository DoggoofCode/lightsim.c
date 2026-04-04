CC ?= cc
CFLAGS ?= -O0 -g -lraylib -L/opt/homebrew/lib -I/opt/homebrew/include -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL # -fsanitize=address
OBJ = main.o

lightsim: $(OBJ)
	$(CC) $(CFLAGS) -o lightsim $(OBJ)

.PHONY: clean
clean:
	rm -f lightsim *.o
