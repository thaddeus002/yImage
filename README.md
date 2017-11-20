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
 *  Write text

## dependencies

Some extern libraries are optionally used :

 * libz and libpng-1.6 for png reading and writing
 * libjpeg for jpeg writing
 * libtiff for tiff writing

## build the lib

To generated the static library file `yImage.a`, simply do :

    $ make

And then to install it in `/usr/local/lib` and the headers files in `/usr/local/include` :

    $ make install

Generate the API documentation needs doxygen. Process by typing :

    $ make doc

Then, open the file `doc/html/index.html` in a web browser.


## using libyImage

The images are represented as simple raster in the yImage struct :

    typedef struct {
        unsigned char *rgbData; /* table RGBRGBRGB... */
        unsigned char *alphaChanel; /* array of alpha (8bits) values */
        int rgbWidth, rgbHeight; /* image's xidth and height */
        int presShapeColor; /* indicate if shape_color is use or not */
        /* available if alpha_chanel == NULL and presShapeColor != 0 */
        yColor shapeColor; /* this color is for transparent pixels */
    } yImage;

### Short example

Here is a program png2ppm.c to convert a PNG file to the PPM binary format :

    #include "yImage.h"
    #include "yImage_io.h"


    int main(int argc, char**argv) {

        yImage *im;

        if(argc < 2) {
            return 1;
        }

        im = load_png(argv[1]);

        if(im == NULL) {
            return 2;
        }

        save_ppm(im, "out.ppm");

        destroy_yImage(im);

        return 0;
    }

Compile this program with command :

    $ gcc -o png2ppm png2ppm.c -DHAVE_LIBPNG -lyImage -lpng -lz
