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


int y_display_text(yImage *background, int x, int y, char *text) {

    int err;
    font_t *font;

    font = read_default_font(&err);

    if(err != 0){
        fprintf(stderr, "Error opening default font - Write failed\n");
        return err;
    }

    err = y_display_text_with_font(background, x, y, text, font);

    release_font(font);

    return err;
}


int y_display_text_with_color(yImage *background, int x, int y, char *text, yColor *color) {

    int err;
    font_t *font;

    font = read_default_font(&err);

    if(err != 0){
        fprintf(stderr, "Error opening default font - Write failed\n");
        return err;
    }

    err = y_display_text_with_font_and_color(background, x, y, text, font, color);

    release_font(font);

    return err;
}

int y_display_text_with_font(yImage *background, int x, int y, char *text, font_t *font){

    yColor black;

    y_set_color(&black, 0, 0, 0, 255);
    return y_display_text_with_font_and_color(background, x, y, text, font, &black);
}


/**
 * The image of a glyph.
 */
yImage *y_create_glyph(font_t *font, int index, yColor *color){

    yImage *im;
    int err;
    unsigned char *car; /* the char to display */
    int longCar; /* number of bytes of car */

    if(font==NULL) return NULL;

    im=y_create_image(&err, NULL, font->header.width, font->header.height);
    y_transp(im);

    longCar=font->header.charsize;
    car = get_character(font, index);

    if(car!=NULL){
        /* TODO : do this better */
        int j;
        for(j=0; j<longCar; j++){
            int k;
            for(k=7; k>0; k--){
                int p=((1<<k)&car[j]);
                if(p) {
                    y_set_pixel(im, color, 7-k, j);
                }
            }
        }
    } else {
        // failled getting the character
        int j;
        for(j=0; j<longCar; j++){
            int k;
            for(k=7; k>0; k--){
                y_set_pixel(im, color, 7-k, j);
            }
        }
        fprintf(stderr, "Could not load the font's glyph with index %d\n", index);
    }

    return im;
}


/**
 * To display a font's glyph.
 */
int y_display_font_char(yImage *background, int x, int y, int index, font_t *font){

    yColor black;

    y_set_color(&black, 0, 0, 0, 255);
    return y_display_font_char_with_color(background, x, y, index, font, &black);
}


int y_display_text_with_font_and_color(yImage *background, int x, int y, char *text, font_t *font, yColor *color){

    yImage *textIm;

    textIm=y_create_text(font, text, color);

    if(textIm==NULL) return 0;

    y_superpose_images(background, textIm, x, y);
    y_destroy_image(textIm);
    return 0;
}


static yImage *rotate_90_degrees(yImage *input) {

    int i, j;
    int err;
    yImage *rotated = y_create_image(&err, NULL, input->rgbHeight, input->rgbWidth);
    if(input->hasShapeColor) {
        rotated->hasShapeColor = 1;
        rotated->shapeColor = input->shapeColor;
    }

    for(i=0; i<input->rgbWidth; i++) {
        for(j=0; j<input->rgbHeight; j++) {
            int oPos = j + (rotated->rgbHeight - 1 - i)*rotated->rgbWidth;
            int iPos = i + j*input->rgbWidth;
            rotated->rgbData[3*oPos] = input->rgbData[3*iPos];
            rotated->rgbData[3*oPos+1] = input->rgbData[3*iPos+1];
            rotated->rgbData[3*oPos+2] = input->rgbData[3*iPos+2];
            rotated->alphaChanel[oPos] = input->alphaChanel[iPos]; 
        }
    }

    return rotated;
}


int y_display_text_vertically_with_font_and_color(yImage *background, int x, int y, char *text, font_t *font, yColor *color){

    yImage *textIm;
    yImage *rotatedTextIm;
    textIm=y_create_text(font, text, color);

    if(textIm==NULL) return 0;

    rotatedTextIm = rotate_90_degrees(textIm);
    y_destroy_image(textIm);
    y_superpose_images(background, rotatedTextIm, x, y);
    y_destroy_image(rotatedTextIm);
    return 0;
}

int y_display_text_vertically_with_font(yImage *background, int x, int y, char *text, font_t *font){
    yColor black;

    y_set_color(&black, 0, 0, 0, 255);
    return y_display_text_vertically_with_font_and_color(background, x, y, text, font, &black);
}


/**
 * To display a font's glyph.
 */
int y_display_font_char_with_color(yImage *background, int x, int y, int index, font_t *font, yColor *color){

    yImage *textIm;

    textIm=y_create_glyph(font, index, color);

    if(textIm==NULL) return 0;

    y_superpose_images(background, textIm, x, y);
    y_destroy_image(textIm);
    return 0;
}

