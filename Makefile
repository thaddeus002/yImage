
ifndef PREFIX
PREFIX=/usr/local
endif

HAVE_LIBPNG=yes
HAVE_LIBTIFF=yes
HAVE_LIBJPEG=yes

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

OBJS=yImage.o yColor.o yImage_io.o yDraw.o graphiques.o range_optimization.o font.o text.o
# first basic version
OBJS=yImage.o yColor.o yImage_io.o yDraw.o
HEADERS=yImage.h yColor.h yImage_io.h yDraw.h

EXEC=test test_dessin test_graphique test_font fillIm trace_graphe

all: libyImage.a

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $<

libyImage.a: $(OBJS)
	@echo "Creating the static library"
	rm -f libyImage.a
	ar -cvq libyImage.a $(OBJS)

.PHONY: clean mrproper install uninstall doc

clean:
	rm -f *.o *.a

mrproper: clean
	rm -f $(EXEC)

install: libyImage.a
	install -m 0640 libyImage.a $(PREFIX)/lib/
	install -m 0640 $(HEADERS) $(PREFIX)/include/

uninstall:
	rm -f $(PREFIX)/lib/libyImage.a
	rm -f $(PREFIX)/include/yImage.h
	rm -f $(PREFIX)/include/yColor.h
	rm -f $(PREFIX)/include/yImage_io.h
	rm -f $(PREFIX)/include/yDraw.h

exec: $(EXEC)

doc:
	doxygen

test_dessin: test_dessin.c libyImage.a
	$(CC) $(CFLAGS) test_dessin.c -o test_dessin -L. -lyImage $(LIBS)

test_graphique: test_graphique.c libyImage.a
	$(CC) $(CFLAGS) test_graphique.c -o test_graphique -L. -lyImage $(LIBS)

test_font: test_font.c libyImage.a
	$(CC) $(CFLAGS) test_font.c -o test_font -L. -lyImage $(LIBS)

fillIm: fillIm.c libyImage.a
	$(CC) $(CFLAGS) fillIm.c -o fillIm -L. -lyImage $(LIBS)

trace_graphe: trace_graphe.c libyImage.a
	$(CC) -I/usr/local/yannick/include $(CFLAGS) trace_graphe.c -o trace_graphe -L. -lyImage -L/usr/local/lib -L/usr/local/yannick/lib -lcsv $(LIBS)
