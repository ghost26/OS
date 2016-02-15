CC=gcc
OBJECTS= hello_world.o
all: hello_world

%.o: %.c
	$(CC) $< -o $@ -c

hello_world: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@
	
clean:
	rm *.o hello_world
	
