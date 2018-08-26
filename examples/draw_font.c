/**
 * \file draw_font.c
 * Create an image with the font's glyphts.
 */


#include "yImage.h"
#include "yColor.h"
#include "yText.h"
#include "yFont.h"
#include "yImage_io.h"
#include "yDraw.h"


int main(int argc, char **argv) {

    font_t *font;
    int err;
    yImage *image;
    int h,w;
    int i;
    yColor *back;
    yColor *sep = NULL;
    char *fontfile = NULL;


    if(argc >=2) {
        fontfile = argv[1];
    }

    font = read_font(&err, fontfile);

    if(err) {
        fprintf(stderr, "Reading font : error %d\n", err);
        return err;
    }

    print_header_infos(font->header);

    w=font->header.width * 16;
    h=font->header.height * 16;

    if(font->header.flags && font->header.length > 256) {
        h+=(font->header.length - 256)/16*font->header.height;
        sep = y_color(RED);
    }

    back = y_color(WHITE);
    image = y_create_uniform_image(&err, back, w, h);
    free(back);

    if(sep != NULL) {

        yPoint M, N;

        M.X=0;
        N.X=w;
        M.Y=font->header.height * 16;
        N.Y=font->header.height * 16;
        y_draw_line(image,M,N,sep);
        free(sep);
    }

    for(i=0; i<font->header.length; i++) {
        char c[2];
        c[0] = i;
        c[1] = '\0';
        int x=(i%16)*font->header.width;
        int y=(i/16)*font->header.height;
        y_display_text_with_font(image, x, y, c, font);
    }

    release_font(font);

    y_save_png(image, "font.png");
    y_destroy_image(image);

    return 0;
}
