target:event_manager.o main.o
	gcc -o simulator.bin main.o event_manager.o

event_manager.o:
	gcc -c event_manager.c

main.o:
	gcc -c main.c

clean:
	rm *.o
#.PHONY:clean
