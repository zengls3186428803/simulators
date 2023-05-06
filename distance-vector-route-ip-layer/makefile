target:event_manager.o main.o tool.o node;
	gcc -g -o simulator.bin main.o event_manager.o tool.o node.o

event_manager.o:
	gcc -g -c event_manager.c

main.o:
	gcc -g -c main.c

tool.o:
	gcc -g -c tool.c

node:
	@for var in $(shell ls node*.c);do \
		echo "gcc -g -c $$var";\
		gcc -g -c $$var;\
	done


clean:
	rm *.o
#.PHONY:clean
