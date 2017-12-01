/**
 * \file png2ppm.c
 *
 * Compile with : gcc -o png2ppm png2ppm.c -DHAVE_LIBPNG -L. -lyImage -lpng -lz
 */


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
