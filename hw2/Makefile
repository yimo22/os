all : myps mytop mylscpu

myps: myps.o util.o
	gcc -o myps myps.o util.o -lncurses -g

mytop: mytop.o util.o
	gcc -o mytop mytop.o util.o -lncurses -g

mylscpu: mylscpu.o
	gcc -o mylscpu mylscpu.o -lncurses -g


myps.o: myps.c header.h
	gcc -c myps.c -lncurse -g

mytop.o: mytop.c header.h
	gcc -c mytop.c -lncurse -g

mylscpu.o: mylscpu.c header.h
	gcc -c mylscpu.c -lncurse -g

clean:
	rm *.o
	rm mytop
	rm myps
	rm mylscpu