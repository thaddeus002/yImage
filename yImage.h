/**
 * \file yImage.h
 * \brief images representation and basic operations. Plus load or save actions.
 *
 * Can save images in various formats with functions which are inspired by the
 * imlib library.
 *
 * This file is under GPL license (GNU GENERAL PUBLIC LICENSE).
 */

#include <stdint.h>
#include <stdio.h>
#include "yColor.h"


#ifndef __Y_IMAGE_H_
#define __Y_IMAGE_H_



/** \brief A raster image */
typedef struct {
    unsigned char *rgbData; /* table RGBRGBRGB... */
    unsigned char *alphaChanel; /* array of alpha (8bits) values */
    int rgbWidth, rgbHeight; /* image's xidth and height */
    int presShapeColor; /* indicate if shape_color is use or not */
    /* available if alpha_chanel == NULL and presShapeColor != 0 */
    yColor shapeColor; /* this color is for transparent pixels */
} yImage;



/************************************************************/
/*                    CREATE / DESTROY IMAGES               */
/************************************************************/

/**
 * Create an yImage without transparency.
 * \param err the function will write here the returned error code
 * \param rbg_data the background image. Background will be black if NULL
 * \param width the new image's width
 * \param height the new image's height
 * \return a newly allocated yImage struct
 */
yImage *create_yImage(int *err, const unsigned char *rgb_data, int width, int height);


/**
 * Create an yImage with an uniform background color.
 * \param err the function will write here the returned error code
 * \param background the yColor to use for the image's background
 * \param width the new image's width
 * \param height the new image's height
 * \return a newly allocated yImage struct
 */
yImage *create_uniform_yImage(int *err, yColor *background, int width, int height);


/**
 * free memory.
 * \param im the struct to free
 */
void destroy_yImage(yImage *im);

/************************************************************/
/*                   HANDLING IMAGES                        */
/************************************************************/


/**
 * \brief retrive the color of a pixel in the image.
 * \param im the image
 * \param x x coordinate of the pixel
 * \param y y coordinate of the pixel
 * \return a newly allocated yColor or NULL in case of fail
 */
yColor *y_get_color(yImage *im, int x, int y);


/**
 * \brief set the max transparency to the image.
 * \param im the image to tranform
 * \return 0 in case of success, or a negative error code
 */
int transp(yImage *im);


/**
 * \brief superimpose two images.
 * \param back image to put at the bottom
 * \param fore image to put at the top
 * \param x x coordinate of "back" for the top/left corner of "fore"
 * \param y y coordinate of "back" for the top/left corner of "fore"
 */
void superpose_images(yImage *back, yImage *fore, int x, int y);


/**
 * \brief Each pixel of the image will become white with alpha=max(r,g,b).
 *
 * So, black will become full transparency, white will remain white.
 * \param im the image to transform
 */
void y_grey_level_to_alpha(yImage *im);



/************************************************************/
/*                   DRAWING FUNCTIONS                      */
/************************************************************/

/**
 * \brief Set a color on a pixel of the image.
 * \param im the image to modify
 * \param color the color for the pixel
 * \param x x coordinate of pixel : 0 is left border
 * \param y y coordinate of pixel : 0 is top border
 */
void yImage_set_pixel(yImage *im, yColor *color, int x, int y);



#endif
