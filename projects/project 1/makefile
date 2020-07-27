c=gcc -o
f=-Wall -Werror


project1: myshell my-cat my-uniq my-wc

myshell: myshell.c
	$(c) myshell myshell.c 
	# $(f)

my-cat: my-cat.c
	$(c) my-cat my-cat.c 
	# $(f)

my-uniq: my-uniq.c
	$(c) my-uniq my-uniq.c 
	# $(f)

my-wc: my-wc.c
	$(c) my-wc my-wc.c 
	# $(f)

clean:
	rm myshell
	rm my-cat
	rm my-uniq
	rm my-wc