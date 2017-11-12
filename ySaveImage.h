/**
 * \file ySaveImage.h
 * \brief load and save yImage in differents formats.
 *
 * yImage is under GPL license (GNU GENERAL PUBLIC LICENSE)
 */


#ifndef Y_SAVE_IMAGE_H_
#define Y_SAVE_IMAGE_H_


#include <stdint.h>
#include <stdio.h>
#include "yImage.h"


/** default quality for JPEG compression : 80% (208/255) */
#define DEFAULT_JPEG_QUALITY 208


// READING

/**
 * \brief Load a yImage from a png file
 * \param f file descriptor of the png image
 * \return the yImage contained in f
 */
yImage *LoadPNG(FILE * f);


// WRITING


/**
 * \brief sauvegarde "im" dans "file" au format ppm ou pnm.
 * \return 1 en cas de succes
 */
int sauve_ppm(yImage *im, const char *file);


/**
 * \brief sauvegarde "im" dans "file" au format JPEG
 * \return 1 en cas de succes
 * nécessite la bibliothèque libjpeg
 */
int sauve_jpeg(yImage *im, const char *file);


/**
 * \brief sauvegarde "im" dans "file" au format PNG
 * \return 0 en cas de succes
 * nécessite la bibliothèque libpng
 */
int sauve_png(yImage *im, const char *file);



/**
 * \brief sauvegarde "im" dans "file" au format TIFF
 * \return retourne 1 en cas de succes
 * nécessite la bibliothèque libtiff
 */
int sauve_tiff(yImage *im, const char *file);



#endif
