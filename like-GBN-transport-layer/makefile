target:event_manager.o main.o tool.o transport_layer.o
	gcc -o simulator.bin main.o event_manager.o tool.o transport_layer.o

event_manager.o:
	gcc -c event_manager.c

main.o:
	gcc -c main.c

tool.o:
	gcc -c tool.c
transport.o:
	gcc -c transport_layer.c

clean:
	rm *.o
#.PHONY:clean
