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
 * \brief load an yImage from a binary ppm file.
 * \param the filename for the data to read
 * \return a new yImage or NULL if the reading failed
 */
yImage *load_ppm(const char *file);


/**
 * \brief Load an yImage from a png file
 * \param the filename for the data to read
 * \return a new yImage or NULL if the reading failed
 */
yImage *load_png(const char *file);



// WRITING


/**
 * \brief Save "im" into "file" at binary ppm format.
 * \return 0 in case of success
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
 * \brief save "im" in "file" at the TIFF format.
 *
 * Needs the libtiff library.
 * \return 1 in case of success
 */
int sauve_tiff(yImage *im, const char *file);



#endif
