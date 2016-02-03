CC=gcc
CFLAGS= -Wall -O5 -DLINUX -Wno-overflow
PROGS= hid-mouse

all:	${PROGS}

hid-mouse:	usbip.c hid-mouse.c 
		${CC} ${CFLAGS} usbip.c -c 
		${CC} ${CFLAGS} usbip.o hid-mouse.c -o hid-mouse 

clean:
		rm -f ${PROGS} core core.* *.o temp.* *.out typescript*
