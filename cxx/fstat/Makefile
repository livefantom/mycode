first: all

stat.o:
	gcc -g -c fstat.c -o stat.o

all: stat

stat: stat.o
	g++ -o stat stat.o

clean:
	rm stat.o stat
