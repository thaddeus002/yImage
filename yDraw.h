/*
 * Copyright (c) 2009-2017 Yannick Garcia <thaddeus.dupont@free.fr>
 *
 * yImage is free software; you can redistribute it and/or modify
 * it under the terms of the GPL license. See LICENSE for details.
 */

/**
 * \file yDraw.h
 * \brief Basic functions for drawing
 */

#ifndef DESSIN_H_
#define DESSIN_H_

#include "yImage.h"


/**
 * A point, or a pixel of the image. Is representated by (X,Y) coordinates.
 * Y is zero at the top of image, and increase to the bottom.
 */
typedef struct {
    int X; /**< x coordinate */
    int Y; /**< y coordinate from top to bottom */
} yPoint;


/**
 * \brief Fill an image with the specified color.
 * \param im the image to modify
 * \param c the new color to use
 */
int y_fill_image(yImage *im, yColor c);



/**
 * \brief Draw a point on the image.
 * \param im the image where to draw
 * \param P the point
 * \param c the new color for the point
 * \return 1 if a point were really drawn
 */
int y_draw_point(yImage *im, yPoint P, yColor c);



/**
 * \brief Draw a line between two points.
 * \param im the image where to draw
 * \param M the first endpoint of the line
 * \param N the second endpoint of the line
 * \param color the color for the line
 * \return
 */
int y_draw_line(yImage *im, yPoint M, yPoint N, yColor c);


/**
 * \brief draw some lines by linking points.
 * \param im the image where to draw
 * \param color the color for the lines
 * \param points a points table
 * \param nbPoints size of the points table
 */
void y_draw_lines(yImage *im, yColor *color, yPoint *points, int nbPoints);


/**
 * \brief fill a polygon with a uniform color.
 *
 * Use the even-odd rule.
 * \param im the image where to draw
 * \param color the color for inside the polygon
 * \param points a points table fo polygon 's corners
 * \param nbPoints size of the points table
 */
void y_fill_polygon(yImage *im, yColor color, yPoint *points, int nbPoints);


#endif
