CC=gcc
CFLAGS= -Wall -DLINUX -g3
PROGS= hid-mouse hid-keyboard cdc-acm stm32-vcp
OUTPUT_PATH= output

all:	${PROGS}



hid-mouse:	usbip.c hid-mouse.c
		${CC} ${CFLAGS} usbip.c -c -o ${OUTPUT_PATH}/usbip.o
		${CC} ${CFLAGS} ${OUTPUT_PATH}/usbip.o hid-mouse.c -o ${OUTPUT_PATH}/hid-mouse

hid-keyboard:	usbip.c hid-keyboard.c
		${CC} ${CFLAGS} usbip.c -c -o ${OUTPUT_PATH}/usbip.o
		${CC} ${CFLAGS} ${OUTPUT_PATH}/usbip.o hid-keyboard.c -o ${OUTPUT_PATH}/hid-keyboard

cdc-acm:	usbip.c cdc-acm.c
		${CC} ${CFLAGS} usbip.c -c -o ${OUTPUT_PATH}/usbip.o
		${CC} ${CFLAGS} ${OUTPUT_PATH}/usbip.o cdc-acm.c -o ${OUTPUT_PATH}/cdc-acm

stm32-vcp:	usbip.c stm32-vcp.c
		${CC} ${CFLAGS} usbip.c -c -o ${OUTPUT_PATH}/usbip.o
		${CC} ${CFLAGS} ${OUTPUT_PATH}/usbip.o stm32-vcp.c -o ${OUTPUT_PATH}/stm32-vcp

clean:
		rm -f ${PROGS} core core.* ${OUTPUT_PATH}/*.o temp.* *.out typescript*
