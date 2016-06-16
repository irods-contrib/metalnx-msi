GCC = g++

INC=-I/usr/include/irods/ -I/usr/include/irods/boost/

LIBS=

all: libmsiobjput_mdvcf

libmsiobjput_mdvcf:
	${GCC} ${INC} -DRODS_SERVER -fPIC "-Wl,-E" -shared -g -Wno-deprecated -rdynamic -o $@.so $@.cpp

clean:
	@rm -f ./*.so

