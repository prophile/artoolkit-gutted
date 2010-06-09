CC=gcc
CFLAGS=-gfull -O0 -I..
INCLUDES=-I..
FULL_CFLAGS=$(CFLAGS) $(INCLUDES)

all: ceilingcat libAR.a libV4LVideo.a
	echo "BEES"

ceilingcat: src/ceilingcat.c AR/*.h AR/sys/*.h libAR.a libV4LVideo.a
	$(CC) -o $@ $(FULL_CFLAGS) $< -L. -lAR -lV4LVideo

libAR.a: AR/*.c AR/*.h AR/sys/*.h
	cd AR ; $(CC) $(FULL_CFLAGS) -c *.c
	rm -f $@
	ar -crs $@ AR/*.o

libV4LVideo.a: VideoLinuxV4L/*.c VideoLinuxV4L/*.h AR/*.h
	cd VideoLinuxV4L ; $(CC) $(FULL_CFLAGS) -c *.c
	rm -f $@
	ar -crs $@ VideoLinuxV4L/*.o

clean:
	rm -f libAR.a AR/*.o VideoLinuxV4L/*.o
