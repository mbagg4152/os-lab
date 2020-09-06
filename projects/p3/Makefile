run-cmd=gcc -pthread -o
args=-p 5 -w 8 -f 13 < sample-input.txt

all: project3

clean-run:  clean project3 run

project3: project3.c
	$(run-cmd) project3 project3.c

run:
	./project3  $(args)

clean:
	rm project3
	clear