CC=gcc
CFLAGS=-gfull -O0 -I..

all: libAR.a libV4LVideo.a
	echo "BEES"

libAR.a: AR/*.c AR/*.h
	cd AR ; $(CC) $(CFLAGS) -c *.c
	rm -f $@
	ar -crs $@ AR/*.o

libV4LVideo.a: VideoLinuxV4L/*.c VideoLinuxV4L/*.h AR/*.h
	cd VideoLinuxV4L ; $(CC) $(CFLAGS) -c *.c
	rm -f $@
	ar -crs $@ VideoLinuxV4L/*.o

clean:
	rm -f libAR.a AR/*.o VideoLinuxV4L/*.o
