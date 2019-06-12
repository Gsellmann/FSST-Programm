prog: Hausaufgabe.o
	gcc Hausaufgabe.o -lpthread -lwiringPi

Hausaufgabe.o: Hausaufgabe.c
	gcc -c Hausaufgabe.c 