CC=gcc
CFLAGS=-g -O0 -I..
LIBRARIES=-lm
INCLUDES_LIBRARY=-I..
INCLUDES_BINARY=-I.
FULL_CFLAGS_LIBRARY=$(CFLAGS) $(INCLUDES_LIBRARY)
FULL_CFLAGS_BINARY=$(CFLAGS) $(INCLUDES_BINARY)

all: ceilingcat libAR.a libV4LVideo.a
	echo "BEES"

ceilingcat: src/ceilingcat.c AR/*.h AR/sys/*.h libAR.a libV4LVideo.a
	$(CC) $(LIBRARIES) $(LDFLAGS) -o $@ $(FULL_CFLAGS_BINARY) $< -L. -lAR -lV4LVideo

libAR.a: AR/*.c AR/*.h AR/sys/*.h
	cd AR ; $(CC) $(FULL_CFLAGS_LIBRARY) -c *.c
	rm -f $@
	ar -crs $@ AR/*.o

libV4LVideo.a: VideoLinuxV4L/*.c VideoLinuxV4L/*.h AR/*.h
	cd VideoLinuxV4L ; $(CC) $(FULL_CFLAGS_LIBRARY) -c *.c
	rm -f $@
	ar -crs $@ VideoLinuxV4L/*.o

clean:
	rm -f *.a AR/*.o VideoLinuxV4L/*.o ceilingcat
