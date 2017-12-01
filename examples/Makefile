
PREFIX=/home/yannick/.local

EXAMPLES=hello draw_font fillPol png2ppm ppm2jpeg

all: $(EXAMPLES)

hello: hello.c

draw_font: draw_font.c

fillPol: fillPol.c

png2ppm: png2ppm.c

ppm2jpeg: ppm2jpeg.c

$(EXAMPLES):
	gcc -o $@ $< -I$(PREFIX)/include -L$(PREFIX)/lib -lyImage -lpng -lz -ljpeg -ltiff

mrproper:
	rm -f $(EXAMPLES)

.PHONY: mrproper
