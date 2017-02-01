GCC=g++

INC=-I${IRODSROOT}/irods/ -I${IRODSROOT}/libexif -I/usr/include/irods/boost/  -I../libmetalnx -L../libmetalnx

LIBS=-lexif

all: libmsiobjjpeg_extract

libmsiobjjpeg_extract:
	${GCC} ${INC} -DRODS_SERVER -fPIC "-Wl,-E" -shared -g -Wno-deprecated -rdynamic -o $@.so $@.cpp ${LIBS}
     
clean:
	@rm -f ./*.so