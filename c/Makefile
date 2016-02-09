CC=gcc
CFLAGS= -Wall -O5 -DLINUX -Wno-overflow
PROGS= hid-mouse hid-keyboard cdc

all:	${PROGS}

hid-mouse:	usbip.c hid-mouse.c 
		${CC} ${CFLAGS} usbip.c -c 
		${CC} ${CFLAGS} usbip.o hid-mouse.c -o hid-mouse 

hid-keyboard:	usbip.c hid-keyboard.c 
		${CC} ${CFLAGS} usbip.c -c 
		${CC} ${CFLAGS} usbip.o hid-keyboard.c -o hid-keyboard
	
cdc:	usbip.c cdc.c 
		${CC} ${CFLAGS} usbip.c -c 
		${CC} ${CFLAGS} usbip.o cdc.c -o cdc
clean:
		rm -f ${PROGS} core core.* *.o temp.* *.out typescript*
