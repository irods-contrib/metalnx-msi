GCC = g++

INC=-I${IRODSROOT}/irods/ -I${IRODSROOT}/irods/boost/  -I../libmetalnx -L../libmetalnx

LIBS=

all: libmsiobjget_microservices

libmsiobjget_microservices:
	${GCC} ${INC} -DRODS_SERVER -fPIC "-Wl,-E" -shared -g -Wno-deprecated -rdynamic -o $@.so $@.cpp
     
clean:
	@rm -f ./*.so