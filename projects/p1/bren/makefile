FLAGS=#-Wall -Werror
CC=gcc -o

all: my-cat my-uniq my-wc myshell

my-cat: my-cat.c
	$(CC) my-cat my-cat.c $(FLAGS)

my-uniq: my-uniq.c
	$(CC) my-uniq my-uniq.c $(FLAGS)

my-wc: my-wc.c
	$(CC) my-wc my-wc.c $(FLAGS)

myshell: myshell.c
	$(CC) myshell myshell.c $(FLAGS)

clean:
	rm myshell
	rm my-cat
	rm my-uniq
	rm my-wc