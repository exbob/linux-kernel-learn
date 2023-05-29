KERDIR=/lib/modules/$(shell uname -r)/build
PWD=$(shell pwd)

obj-m:=chardev.o

default:
	make -C ${KERDIR} M=${PWD} modules KCFLAGS+=-DDEBUG
	gcc test.c -o test
clean:
	make -C ${KERDIR} M=${PWD} clean
	rm -rf test

