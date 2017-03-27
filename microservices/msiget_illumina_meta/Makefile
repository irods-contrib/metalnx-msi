GCC = g++

INC=-I${IRODSROOT}/irods/ -I${IRODSROOT}/irods/boost/  -I../libmetalnx -L../libmetalnx

LIBS=

all: libmsiget_illumina_meta

libmsiget_illumina_meta:
	${GCC} ${INC} -DRODS_SERVER -fPIC "-Wl,-E" -shared -g -Wno-deprecated -rdynamic -o $@.so $@.cpp
     
clean:
	@rm -f ./*.so