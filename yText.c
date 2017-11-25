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


yImage *create_text(font_t *font, char *text, yColor *color){

    int l;
    yImage *im;
    int err;
    unsigned char *car; /* the char to display */
    int longCar; /* number of bytes of car */
    int i;

    if(font==NULL) return NULL;
    if(text==NULL) return NULL;

    l=strlen(text);

    im=create_yImage(&err, NULL, font->header.width*l, font->header.height);
    transp(im);

    for(i=0; i<l; i++){
        longCar=font->header.charsize;
        car=get_character(font, (unsigned char) text[i]);

        if(car!=NULL){
            /* TODO : do this better */
            int j;
            for(j=0; j<longCar; j++){
                int k;
                for(k=7; k>0; k--){
                    int p=((1<<k)&car[j]);
                    if(p) {
                        yImage_set_pixel(im, color, (i*font->header.width)+(7-k), j);
                    }
                }
            }
        } else {
            // failled getting the character
            int j;
            for(j=0; j<longCar; j++){
                int k;
                for(k=7; k>0; k--){
                    yImage_set_pixel(im, color, (i*font->header.width)+(7-k), j);
                }
            }
            fprintf(stderr, "Could not load the font's glyph number %d\n",text[i]);
        }
    }

    return im;
}



int display_text(yImage *fond, int x, int y, char *texte, font_t *font){

    yImage *textIm;
    yColor black;

    black.r=0;
    black.b=0;
    black.g=0;
    black.alpha=255;

    textIm=create_text(font, texte, &black);

    if(textIm==NULL) return 0;

    superpose_images(fond, textIm, x, y);
    return 0;
}
