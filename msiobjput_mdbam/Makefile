GCC = g++

INC=-I/usr/include/irods/ -I/usr/include/irods/boost/ -I/usr/local/include

LIBS=-lhts

all: libmsiobjput_mdbam

libmsiobjput_mdbam:
	${GCC} ${INC} -DRODS_SERVER -fPIC "-Wl,-E" -shared -g -Wno-deprecated -rdynamic -o $@.so $@.cpp ${LIBS}
     
clean:
	@rm -f ./*.so