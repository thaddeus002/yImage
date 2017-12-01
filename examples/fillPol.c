/**
 * \file fillPol.c
 */

#include "yImage.h"
#include "yImage_io.h"
#include "yColor.h"
#include "yDraw.h"
#include <stdlib.h>

#define WIDTH 300
#define HEIGHT 300


static void square(yPoint *polygon) {
    polygon[0].X = 50;
    polygon[0].Y = 50;
    polygon[1].X = 250;
    polygon[1].Y = 50;
    polygon[2].X = 250;
    polygon[2].Y = 250;
    polygon[3].X = 50;
    polygon[3].Y = 250;
}

static void hexagon(yPoint *polygon) {
    polygon[0].X = 150;
    polygon[0].Y = 35;
    polygon[1].X = 250;
    polygon[1].Y = 87;
    polygon[2].X = 250;
    polygon[2].Y = 213;
    polygon[3].X = 150;
    polygon[3].Y = 265;
    polygon[4].X = 50;
    polygon[4].Y = 213;
    polygon[5].X = 50;
    polygon[5].Y = 87;
}


int main(int argc, char **argv) {

    int err;
    yImage *image;
    yColor *back, *fore;
    yPoint polygon[6];


    back = y_color(RED);
    image = y_create_uniform_image(&err, back, WIDTH, HEIGHT);
    free(back);

    if(err) {
        fprintf(stderr, "Error %d creating image\n", err);
        return err;
    }

    // Define polygon
    hexagon(polygon);

    fore = y_color(YELLOW);
    y_fill_polygon(image,*fore,polygon,6);
    free(fore);

    y_save_png(image, "fillPol.png");
    y_destroy_image(image);

    return 0;
}





