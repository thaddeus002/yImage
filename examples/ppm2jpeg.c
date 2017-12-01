/**
 * \file ppm2jpeg.c
 *
 * Compile with : gcc -o ppm2jpeg ppm2jpeg.c -DHAVE_LIBJPEG -L. -lyImage -ljpeg
 */


#include "yImage.h"
#include "yImage_io.h"


int main(int argc, char**argv) {

    yImage *im;

    if(argc < 2) {
        return 1;
    }

    im = y_load_ppm(argv[1]);

    if(im == NULL) {
        return 2;
    }

    y_save_jpeg(im, "out.jpeg");

    y_destroy_image(im);

    return 0;
}
