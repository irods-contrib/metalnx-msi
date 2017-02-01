GCC = g++

INC=-I${IRODSROOT}/irods/ -I${IRODSROOT}/irods/boost/  -I../libmetalnx -L../libmetalnx

LIBS=

all: libmsiobjput_populate

libmsiobjput_populate:
	${GCC} ${INC} -DRODS_SERVER -fPIC "-Wl,-E" -shared -g -Wno-deprecated -rdynamic -o $@.so $@.cpp
     
clean:
	@rm -f ./*.so