/*
 * Copyright (c) 2009-2017 Yannick Garcia <thaddeus.dupont@free.fr>
 *
 * yImage is free software; you can redistribute it and/or modify
 * it under the terms of the GPL license. See LICENSE for details.
 */

/**
 * \file yText.c
 * \brief text drawing functions.
 */

#include "yText.h"
#include <string.h>
#include <stdio.h>


yImage *y_create_text(font_t *font, char *text, yColor *color){

    int l;
    yImage *im;
    int err;
    unsigned char *car; /* the char to display */
    int longCar; /* number of bytes of car */
    int i;

    if(font==NULL) return NULL;
    if(text==NULL) return NULL;

    l=strlen(text);

    im=y_create_image(&err, NULL, font->header.width*l, font->header.height);
    y_transp(im);

    int nb;
    int pos = 0;
    for(i=0; i<l; i=i+nb){
        longCar=font->header.charsize;
        car=get_glyph(font, text+i, &nb);

        if(car!=NULL){
            /* TODO : do this better */
            int j;
            for(j=0; j<longCar; j++){
                int k;
                for(k=7; k>0; k--){
                    int p=((1<<k)&car[j]);
                    if(p) {
                        y_set_pixel(im, color, (pos*font->header.width)+(7-k), j);
                    }
                }
            }
        } else {
            // failled getting the character
            int j;
            for(j=0; j<longCar; j++){
                int k;
                for(k=7; k>0; k--){
                    y_set_pixel(im, color, (pos*font->header.width)+(7-k), j);
                }
            }
            fprintf(stderr, "Could not load the font's glyph number %d\n",(unsigned char) text[i]);
        }
        pos++;
    }

    return im;
}


int y_display_text(yImage *fond, int x, int y, char *text) {

    int err;
    font_t *font;

    font = read_default_font(&err);

    if(err != 0){
        fprintf(stderr, "Error opening default font - Write failed\n");
        return err;
    }

    err = y_display_text_with_font(fond, x, y, text, font);

    release_font(font);

    return err;
}


int y_display_text_with_color(yImage *fond, int x, int y, char *text, yColor *color) {

    int err;
    font_t *font;

    font = read_default_font(&err);

    if(err != 0){
        fprintf(stderr, "Error opening default font - Write failed\n");
        return err;
    }

    err = y_display_text_with_font_and_color(fond, x, y, text, font, color);

    release_font(font);

    return err;
}

int y_display_text_with_font(yImage *fond, int x, int y, char *text, font_t *font){

    yColor black;

    y_set_color(&black, 0, 0, 0, 255);
    return y_display_text_with_font_and_color(fond, x, y, text, font, &black);
}


int y_display_text_with_font_and_color(yImage *fond, int x, int y, char *text, font_t *font, yColor *color){

    yImage *textIm;

    textIm=y_create_text(font, text, color);

    if(textIm==NULL) return 0;

    y_superpose_images(fond, textIm, x, y);
    return 0;
}

