/**
 * \file hello.c
 * Create an image with text "Hello World!".
 */

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
