scheduler: main.o
	gcc main.o -o scheduler

main.o: main.c
	gcc -c main.c

clean:
	rm -f *.o scheduler

.PHONY:clean