CC       = clang
CFLAGS   = -Wall -Wpedantic -Werror -Wextra -gdwarf-4 
OBJECTS  = numtheory.o randstate.o ss.o

all: keygen encrypt decrypt

keygen: keygen.o $(OBJECTS)
	$(CC) -o $@ $^ -lgmp

encrypt: encrypt.o $(OBJECTS)
	$(CC) -o $@ $^ -lgmp

decrypt: decrypt.o $(OBJECTS)
	$(CC) -o $@ $^ -lgmp

clean:
	rm -f keygen encrypt decrypt *.o 

%.o : %.c
	$(CC) $(CFLAGS) -c $<

format:
	clang-format -i -style=file *.[ch]

