GCC = g++

INC=-I${IRODSROOT}/irods/ -I${IRODSROOT}/irods/boost/ -I/usr/local/include  -I../libmetalnx -L../libmetalnx

LIBPATH=-L/usr/local/lib

LIBS=-lhts

all: libmsiobjput_mdbam

libmsiobjput_mdbam:
	${GCC} ${INC} ${LIBPATH} -DRODS_SERVER -fPIC "-Wl,-E" -shared -g -Wno-deprecated -rdynamic -o $@.so $@.cpp ${LIBS}
     
clean:
	@rm -f ./*.so
