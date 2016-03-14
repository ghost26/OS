CC=gcc
OBJECTS= cat.o
all: cat

%.o: %.c
	$(CC) $< -o $@ -c

cat: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

clean:
	rm -f *.o cat
