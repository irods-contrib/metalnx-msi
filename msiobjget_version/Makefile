GCC = g++

INC=-I${IRODSROOT}/irods/ -I${IRODSROOT}/irods/boost/ 

LIBS=

all: libmsiobjget_version

libmsiobjget_version:
	${GCC} ${INC} -DRODS_SERVER -fPIC "-Wl,-E" -shared -g -Wno-deprecated -rdynamic -o $@.so $@.cpp
     
clean:
	@rm -f ./*.so