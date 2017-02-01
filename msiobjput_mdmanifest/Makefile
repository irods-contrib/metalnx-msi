GCC=g++

INC=-I${IRODSROOT}/irods/ -I${IRODSROOT}/irods/boost/ -I/usr/include/libxml2  -I../libmetalnx -L../libmetalnx

LIBS=-lxml2

all: libmsiobjput_mdmanifest

libmsiobjput_mdmanifest:
	${GCC} ${INC} -DRODS_SERVER -fPIC "-Wl,-E" -shared -g -Wno-deprecated -rdynamic -o $@.so $@.cpp ${LIBS}
     
clean:
	@rm -f ./*.so