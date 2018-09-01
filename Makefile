
ifndef PREFIX
PREFIX=/usr/local
endif

ifndef HAVE_PNG
HAVE_LIBPNG=yes
endif

ifndef HAVE_LIBTIFF
HAVE_LIBTIFF=no
endif 

ifndef HAVE_LIBJPEG
HAVE_LIBJPEG=no
endif

INCLUDEPNG=-I/usr/local/include
INCLUDEJPEG=-I/usr/include
INCLUDETIFF=-I/usr/local/include
INCLUDEDIV=-I/usr/local/include

CC=gcc

INCLUDEDIR = $(INCLUDEDIV)

LIBS =

OPTIONS =

ifeq ($(HAVE_LIBPNG),yes)
	INCLUDEDIR += $(INCLUDEPNG)
	LIBS += -lpng -lz
	OPTIONS += -DHAVE_LIBPNG
endif

ifeq ($(HAVE_LIBJPEG),yes)
	INCLUDEDIR += $(INCLUDEJPEG)
	LIBS += -ljpeg
	OPTIONS += -DHAVE_LIBJPEG
endif

ifeq ($(HAVE_LIBTIFF),yes)
	INCLUDEDIR += $(INCLUDETIFF)
	LIBS += -ltiff
	OPTIONS += -DHAVE_LIBTIFF
endif


CFLAGS = -Wall -O2 -s $(INCLUDEDIR) $(OPTIONS)

OBJS=yImage.o yColor.o yImage_io.o yDraw.o yFont.o yText.o
HEADERS=yImage.h yColor.h yImage_io.h yDraw.h yFont.h yText.h

all: libyImage.a

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $<

yFont.o: yFont.c yFont.h yLat1-14.h

libyImage.a: $(OBJS)
	@echo "Creating the static library"
	rm -f libyImage.a
	ar -cvq libyImage.a $(OBJS)

.PHONY: clean mrproper install uninstall doc

clean:
	rm -f *.o *.a

mrproper: clean


install: libyImage.a
	install -m 0640 libyImage.a $(PREFIX)/lib/
	install -m 0640 $(HEADERS) $(PREFIX)/include/

uninstall:
	rm -f $(PREFIX)/lib/libyImage.a
	rm -f $(PREFIX)/include/yImage.h
	rm -f $(PREFIX)/include/yColor.h
	rm -f $(PREFIX)/include/yImage_io.h
	rm -f $(PREFIX)/include/yDraw.h
	rm -f $(PREFIX)/include/yFont.h
	rm -f $(PREFIX)/include/yText.h

exec: $(EXEC)

doc:
	doxygen

