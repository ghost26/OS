CC=gcc
OBJECTS= sigusr.o
all: sigusr

%.o: %.c
	$(CC) $< -o $@ -c

sigusr: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@
	
clean:
	rm -f *.o sigusr
