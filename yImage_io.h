/**
 * \file yImage_io.h
 * \brief Load and save yImage in differents formats.
 *
 * PPM read or write is available without the need of an extern library.
 * The use of PNG, JPEG or TIFF format needs the correspondant library
 * at build time.
 *
 * libyImage is under GPL license (GNU GENERAL PUBLIC LICENSE)
 */


#ifndef Y_IMAGE_IO_H_
#define Y_IMAGE_IO_H_


#include <stdint.h>
#include <stdio.h>
#include "yImage.h"


/** default quality for JPEG compression : 80% (208/255) */
#define DEFAULT_JPEG_QUALITY 208


// READING

/**
 * \brief load an yImage from a binary ppm file.
 * \param file
 *            the filename for the data to read
 * \return a new yImage or NULL if the reading failed
 */
yImage *load_ppm(const char *file);


/**
 * \brief Load an yImage from a png file
 * \param file
 *            the filename for the data to read
 * \return a new yImage or NULL if the reading failed
 */
yImage *load_png(const char *file);



// WRITING


/**
 * \brief Save "im" into "file" at binary ppm format.
 * \param im
 *            the image's data
 * \param file
 *            the filename of the file to create
 * \return 0 in case of success
 */
int save_ppm(yImage *im, const char *file);


/**
 * \brief save "im" into "file" at JPEG format
 *
 * Needs libjpeg library
 * \param im
 *            the image's data
 * \param file
 *            the filename of the file to create
 * \return 0 in case of success
 */
int save_jpeg(yImage *im, const char *file);


/**
 * \brief save "im" into "file" at PNG format.
 *
 * Needs libpng library
 * \param im
 *            the image's data
 * \param file
 *            the filename of the file to create
 * \return 0 in case of success
 */
int save_png(yImage *im, const char *file);



/**
 * \brief save "im" in "file" at the TIFF format.
 *
 * Needs the libtiff library.
 * \param im
 *            the image's data
 * \param file
 *            the filename of the file to create
 * \return 0 in case of success
 */
int save_tiff(yImage *im, const char *file);



#endif
