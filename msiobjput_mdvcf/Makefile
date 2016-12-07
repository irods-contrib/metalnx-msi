GCC = g++

INC=-I${IRODSROOT}/irods/ -I${IRODSROOT}/irods/boost/

LIBS=

all: libmsiobjput_mdvcf

libmsiobjput_mdvcf:
	${GCC} ${INC} -DRODS_SERVER -fPIC "-Wl,-E" -shared -g -Wno-deprecated -rdynamic -o $@.so $@.cpp

clean:
	@rm -f ./*.so

