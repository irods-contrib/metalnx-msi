GCC = g++

INC=-I/usr/include/irods/ -I/usr/include/irods/boost/ 

LIBS=

all: libmsiget_illumina_meta

libmsiget_illumina_meta:
	${GCC} ${INC} -DRODS_SERVER -fPIC "-Wl,-E" -shared -g -Wno-deprecated -rdynamic -o $@.so $@.cpp
     
clean:
	@rm -f ./*.so