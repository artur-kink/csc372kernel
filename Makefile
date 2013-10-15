all:
	rm -f lab3
	gcc -g -o lab3 main.c
clean:
	rm -f *.o
	rm -f lab3
