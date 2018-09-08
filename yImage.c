/*
 * Copyright (c) 2009-2017 Yannick Garcia <thaddeus.dupont@free.fr>
 *
 * yImage is free software; you can redistribute it and/or modify
 * it under the terms of the GPL license. See LICENSE for details.
 */

/**
 * \file yImage.c
 * \brief images representation and basic operations. Plus load or save actions.
 */


#include "yImage.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h> //memset()


/************************************************************/
/*               CREATION / DESTRUCTION DES IMAGES          */
/************************************************************/


/* create an yImage without transparency */
yImage *y_create_image(int *err, const unsigned char *rgbData, int width, int height){

    yImage *im;

    im=(yImage *)malloc(sizeof(yImage));

    if (im==NULL) {
        *err=ERR_ALLOCATE_FAIL;
        return(NULL);
    }

    im->rgbData=(unsigned char *) malloc(3*width*height);
    if(im->rgbData==NULL) {
        *err=ERR_ALLOCATE_FAIL;
        free(im);
        return(NULL);
    }

    im->alphaChanel=(unsigned char *) malloc(width*height);
        if(im->alphaChanel==NULL) {
        *err=ERR_ALLOCATE_FAIL;
        free(im->rgbData);
        free(im);
        return(NULL);
    }

    memset(im->alphaChanel, 255, width*height);

    if(rgbData==NULL) {
        memset(im->rgbData, 0, 3*width*height);
    } else memcpy(im->rgbData, rgbData, 3*width*height);

    im->rgbHeight=height;
    im->rgbWidth=width;

    im->hasShapeColor=0;

    im->shapeColor.r=0;
    im->shapeColor.g=0;
    im->shapeColor.b=0;

    *err=0;
    return(im);
}


yImage *y_create_uniform_image(int *err, yColor *background, int width, int height){

    yImage *img = y_create_image(err, NULL, width, height);
    int pix;

    for(pix=0; pix<width*height; pix++) {
        img->rgbData[3*pix+0]=background->r;
        img->rgbData[3*pix+1]=background->g;
        img->rgbData[3*pix+2]=background->b;
    }

    memset(img->alphaChanel, background->alpha, width*height);

    return img;
}




/* libération de la memoire */
void y_destroy_image(yImage *im){
    if(im!=NULL){
        if(im->rgbData!=NULL) free(im->rgbData);
        free(im);
    }
}


/************************************************************/
/*               MANIPULATION DES IMAGES                    */
/************************************************************/


yColor *y_get_color(yImage *im, int x, int y){

    yColor *value = NULL;
    int pos;

    pos = x + y*im->rgbWidth;

    if(pos < im->rgbWidth*im->rgbHeight) {

        value = malloc(sizeof(yColor));
        if(value==NULL) return NULL;

        value->r = im->rgbData[3*pos];
        value->g = im->rgbData[3*pos+1];
        value->b = im->rgbData[3*pos+2];
        value->alpha = im->alphaChanel[pos];
    }

    return value;
}





/* rend l'image transparente */
int y_transp(yImage *im){
    if(im==NULL) return -1;

    if(im->alphaChanel==NULL) im->alphaChanel=(unsigned char *)malloc(im->rgbWidth*im->rgbHeight);
    if(im->alphaChanel==NULL) return ERR_ALLOCATE_FAIL;

    memset(im->alphaChanel, 0, im->rgbWidth*im->rgbHeight);
    im->hasShapeColor=0;

    return 0;
}




void y_superpose_images(yImage *back, yImage *fore, int x, int y){

    yColor composition;
    int i, j;

    for(i=0; i<fore->rgbWidth; i++)
        for(j=0; j<fore->rgbHeight; j++){

        int xb, yb;
        xb=i+x; yb=j+y;

        if((xb>=0) && (xb<back->rgbWidth) && (y>=0) && (yb<back->rgbHeight)){

            int ab= back->alphaChanel[xb+yb*back->rgbWidth];
            int rb= back->rgbData[3*(xb+yb*back->rgbWidth)];
            int gb= back->rgbData[3*(xb+yb*back->rgbWidth)+1];
            int bb= back->rgbData[3*(xb+yb*back->rgbWidth)+2];

            int af= fore->alphaChanel[i+j*fore->rgbWidth];
            int rf= fore->rgbData[3*(i+j*fore->rgbWidth)];
            int gf= fore->rgbData[3*(i+j*fore->rgbWidth)+1];
            int bf= fore->rgbData[3*(i+j*fore->rgbWidth)+2];

            yColor foreColor;
            y_set_color(&foreColor, rf, gf, bf, af);

            if(!fore->hasShapeColor || y_compare_colors(&(fore->shapeColor), &foreColor)) {
                /* TODO make a better colors superposition */
                composition.r=((255-af)*rb + af*rf)/255;
                composition.b=((255-af)*bb + af*bf)/255;//af>0?bf:bb;
                composition.g=((255-af)*gb + af*gf)/255;//af>0?gf:gb;
                composition.alpha=ab+((255-ab)*af/255);

                y_set_pixel(back, &composition, xb, yb);
            }
        }
    }
}



void y_grey_level_to_alpha(yImage *im){

    int i, j;
    yColor *color;


    for(i=0; i<im->rgbWidth; i++) {
        for(j=0; j<im->rgbHeight; j++) {

            color = y_get_color(im, i, j);

            if(color!=NULL) {

                int m = color->r;
                if(color->g>m) m=color->g;
                if(color->b>m) m=color->b;

                if(m>0) {

                    color->r=255;
                    color->g=255;
                    color->b=255;
                }
                color->alpha=m;
                y_set_pixel(im, color, i, j);

                free(color);
            }
        }
    }
}





/************************************************************/
/*                   DRAWING FUNCTIONS                      */
/************************************************************/



void y_set_pixel(yImage *im, yColor *color, int x, int y){
    int pos; /* position of the pixel (x,y) in the data array */

    if(im==NULL) return;
    if(color==NULL) return;
    if((x<0) || (y<0)) return;
    if((x>=im->rgbWidth) || (y>=im->rgbHeight) )return;

    pos = 3*y*im->rgbWidth + 3*x;

    im->rgbData[pos]=color->r;
    im->rgbData[pos+1]=color->g;
    im->rgbData[pos+2]=color->b;

    im->alphaChanel[y*im->rgbWidth + x]=color->alpha;
}

