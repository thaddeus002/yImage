/*
 * Copyright (c) 2009-2017 Yannick Garcia <thaddeus.dupont@free.fr>
 *
 * yImage is free software; you can redistribute it and/or modify
 * it under the terms of the GPL license. See LICENSE for details.
 */

/**
 * \file yColor.h
 * \brief four channels color representation.
 */

#include <stdint.h>
#include <stdio.h>

#ifndef __Y_COLOR_H_
#define __Y_COLOR_H_



/* error code */
#define ERR_NULL_PALETTE -1
#define ERR_NULL_COLOR -2
#define ERR_BAD_INDEX -3
#define ERR_ALLOCATE_FAIL -4



/** \brief Represents a color */
typedef struct _yColor {
    unsigned char r, g, b; /**< Red, green and blue levels, between 0 and 255 */
    unsigned char alpha; /**< alpha 0 => transparency ; 255 => opacity */
} yColor;


/** \brief a colors palete for 8 bits color images */
typedef uint8_t yColorPalette_t[256*3];

/** \brief enumeration of basics color to direct access */
typedef enum{BLACK=0, WHITE, RED, GREEN, BLUE, ORANGE_, YELLOW, CYAN_, MAGENTA_, MARRON_} ySimpleColor;



/* FUNCTIONS */

// Palette functions

/**
 * \brief Init a yColorPalette with the data read.
 *
 * Values in palette are four time the values in pal.
 * \param palette the objet to init
 * \param pal the source data
 */
void y_init_palette(yColorPalette_t palette, const uint8_t *pal);


/**
 * Get a color on the palette.
 * \param color a allocated struct to get the result
 * \param palette the palette
 * \param index the index of the color we want (between 0 and 255)
 * \return the index of the wanted color or a negative error code :
 * ERR_NULL_COLOR, ERR_NULL_PALETTE, or ERR_BAD_INDEX (if "index" is
 * out of range)
 */
int y_get_color_index(yColor *color, yColorPalette_t palette, int index);

// yColor functions


/**
 * \brief Init a color.
 * \param color an ySimpleColor
 * \return a newly allocated struct
 */
yColor *y_color(ySimpleColor color);


/**
 * \brief Init the data of a yColor struct.
 * \param color a pre-allocated struct to initialize
 * \param r the red level of the color (0 to 255)
 * \param g the green level of the color (0 to 255)
 * \param b the blue level of the color (0 to 255)
 * \param a the alpha chanel (0 for totally transparent, 255 for
 * totally opaque)
 */
void y_set_color(yColor *color, unsigned char r, unsigned char g, unsigned char b, unsigned char a);


/**
 * \brief Init the data of a yColor struct.
 * \param color a pre-allocated struct to initialize
 * \param rgba the four bytes r,g,b, and a in an unique integer
 */
void y_init_color(yColor *color, unsigned int rgba);


/**
 * \brief Compare two colors.
 * \param c1 the first color to compare
 * \param c2 the second color to compare
 * \return 0 if the colors are identical
 */
int y_compare_colors(yColor *c1, yColor *c2);




#endif
