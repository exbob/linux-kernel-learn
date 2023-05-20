KERDIR=/lib/modules/$(shell uname -r)/build
PWD=$(shell pwd)

obj-m:=hellomod.o

default:
	make -C ${KERDIR} M=${PWD} modules KCFLAGS+=-DDEBUG
clean:
	make -C ${KERDIR} M=${PWD} clean

