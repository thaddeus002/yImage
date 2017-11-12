/**
 * \file yXinterface.h
 * yXinterface is part of yImage under GNU GENERAL PUBLIC LICENSE.
 * Creating an "yImage" from a Xlib Drawable.
 */

#ifndef YXINTERFACE_H_
#define YXINTERFACE_H_ 1

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include "yImage.h"



/**
 * Create an "yImage" from a Xlib Drawable (Window ou Pixmap).
 * \param win the Drawable
 * \param vis X Visual linked to "win"
 * \param disp X Display linked to "win"
 * \param x horizontal coordinate of the beginning point for image extraction
 * \param y vertical coordinate of the beginning point for image extraction
 * \param height height of the image to extract
 * \param width width of the image to extract
 * Cette fonction a été simplifiée et ne prend pas en compte SHM (shm.h)
 * Elle se limite également aux profondeur d'affichage de 24 ou 32 :
 *      Imlib prend également en compte 0 à 8 et 15 et 16
 * \return NULL if it fail
 */
yImage *create_yImage_from_drawable(Drawable win, Visual *vis, Display  *disp, int x, int y, int width, int height);

#endif
