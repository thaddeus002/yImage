
PREFIX=..
LIBDIR=$(PREFIX)
INCDIR=$(PREFIX)
LDFLAGS=-L$(LIBDIR) -lyImage -lpng -lz -ljpeg -ltiff

EXAMPLES=hello draw_font fillPol png2ppm ppm2jpeg

all: $(EXAMPLES)

hello: hello.c ../libyImage.a

draw_font: draw_font.c ../libyImage.a

fillPol: fillPol.c ../libyImage.a

png2ppm: png2ppm.c ../libyImage.a

ppm2jpeg: ppm2jpeg.c ../libyImage.a

$(EXAMPLES):
	gcc -o $@ $< -I$(INCDIR) $(LDFLAGS)

../libyImage.a:
	make -C ..

mrproper:
	rm -f $(EXAMPLES)

.PHONY: mrproper
