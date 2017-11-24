/*
 * Copyright (c) 2009-2017 Yannick Garcia <thaddeus.dupont@free.fr>
 *
 * yImage is free software; you can redistribute it and/or modify
 * it under the terms of the GPL license. See LICENSE for details.
 */

/**
 * \file yText.h
 * \brief text drawing functions.
 * \author Yannick Garcia
 */

#ifndef TEXT_H_
#define TEXT_H_


#include "yFont.h"
#include "yImage.h"


/**
 * Create a new image with transparent background and showing a given
 * text.
 * \param font the font to use
 * \param text the text to display
 * \param color the foreground color
 * \return a newly allocated image
 */
yImage *create_text(font_t *font, char *text, yColor *color);


/**
 * Display a text in black on an existing image.
 * \param fond the background image
 * \param x x position on the background image to begin writing
 * \param y y position on the background image to write
 * \param texte the text to display
 * \param font the font to use
 * \return 0 in case of success
 */
int display_text(yImage *fond, int x, int y, char *texte, font_t *font);


#endif
