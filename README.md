# libyImage

A simple, low level, drawing library in C.

The aim of this library is to provide basic drawing feature while remaining simple and easy to use.

## features

 *  Read PNG and PPM image files
 *  Save in PNG, PPM, JPEG or TIFF format
 *  Support transparency (alpha channel)
 *  Image superposition, like using calcs
 *  Draw lines and polygons
 *  Fill polygons
 *  Draw circles
 *  Draw filled circles
 *  Write text

## dependencies

Some extern libraries are optionally used :

 * libz and libpng-1.6 for png reading and writing
 * libjpeg for jpeg writing
 * libtiff for tiff writing

## build the lib

To generated the static library file `yImage.a`, simply do :

```sh
$ make
```

By default, only PNG is supported, use

```sh
$ HAVE_JPEG=yes make
```
or
```sh
$ HAVE_TIFF=yes make
```

To add JPEG or TIFF support.
And then to install it in `/usr/local/lib` and the headers files in `/usr/local/include` :

``` sh
$ make install
```

Generate the API documentation needs doxygen (and graphviz for the graphs). Process by typing :

```sh
$ make doc
```

Then, open the file `doc/html/index.html` in a web browser.


## Usage

The images are represented as simple raster in the yImage struct :

```C
typedef struct {
    unsigned char *rgbData; /* data table : RGBRGBRGB... */
    unsigned char *alphaChanel; /* array of alpha (8bits) values */
    int rgbWidth, rgbHeight; /* image's width and height */
    int presShapeColor; /* indicate if shape_color is use or not */
    /* available if alpha_chanel == NULL and presShapeColor != 0 */
    yColor shapeColor; /* this color is for transparent pixels */
} yImage;
```

### Short examples

#### Image format conversion

Here is a program png2ppm.c to convert a PNG file to the PPM binary format :

```C
#include "yImage.h"
#include "yImage_io.h"


int main(int argc, char**argv) {

    yImage *im;

    if(argc < 2) {
        return 1;
    }

    im = y_load_png(argv[1]);

    if(im == NULL) {
        return 2;
    }

    y_save_ppm(im, "out.ppm");
    y_destroy_image(im);

    return 0;
}
```

Compile this program with command :

```sh
$ gcc -o png2ppm png2ppm.c -lyImage -lpng -lz -ljpeg -ltiff
```

#### Writing text

The programme hello.c create an image with text "Hello World!".

```C
#include "yImage.h"
#include "yColor.h"
#include "yText.h"
#include "yImage_io.h"
#include <stdlib.h>

#define WIDTH 300
#define HEIGHT 50

int main(int argc, char **argv) {

    int err;
    yImage *image;
    yColor *back;

    back = y_color(WHITE);
    image = y_create_uniform_image(&err, back, WIDTH, HEIGHT);
    free(back);

    if(err) {
        fprintf(stderr, "Error %d creating image\n", err);
        return err;
    }

    y_display_text(image, 100, 17, "Hello World!");

    y_save_png(image, "hello.png");
    y_destroy_image(image);

    return 0;
}
```

Compile this program with command :

```sh
$ gcc -o hello hello.c -lyImage -lpng -lz -ljpeg -ltiff
```

#### Drawing exemple

This program, named fillPol, draw a yellow square centered on a red window.

```C
#include "yImage.h"
#include "yImage_io.h"
#include "yColor.h"
#include "yDraw.h"
#include <stdlib.h>

#define WIDTH 300
#define HEIGHT 300

int main(int argc, char **argv) {

    int err;
    yImage *image;
    yColor *back, *fore;
    yPoint polygon[4];


    back = y_color(RED);
    image = y_create_uniform_image(&err, back, WIDTH, HEIGHT);
    free(back);

    if(err) {
        fprintf(stderr, "Error %d creating image\n", err);
        return err;
    }

    // Define polygon
    polygon[0].X = 50;
    polygon[0].Y = 50;
    polygon[1].X = 250;
    polygon[1].Y = 50;
    polygon[2].X = 250;
    polygon[2].Y = 250;
    polygon[3].X = 50;
    polygon[3].Y = 250;

    fore = y_color(YELLOW);
    y_fill_polygon(image,*fore,polygon,4);
    free(fore);

    y_save_png(image, "fillPol.png");
    y_destroy_image(image);

    return 0;
}
```

Compile this program with command :

```sh
$ gcc -o fillPol fillPol.c -DHAVE_LIBPNG -lyImage -lpng -lz
```
