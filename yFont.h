/**
 * \file yFont.h
 */

#ifndef FONT_H_
#define FONT_H_

#include <stdio.h>
#include <stdlib.h>

#define PSF2_MAGIC0     0x72
#define PSF2_MAGIC1     0xb5
#define PSF2_MAGIC2     0x4a
#define PSF2_MAGIC3     0x86

/* bits used in flags */
#define PSF2_HAS_UNICODE_TABLE 0x01

/* max version recognized so far */
#define PSF2_MAXVERSION 0

/* UTF8 separators */
#define PSF2_SEPARATOR  0xFF
#define PSF2_STARTSEQ   0xFE

/* Error codes */
#define Y_ERR_ALLOCATE_FAIL 1
#define Y_ERR_FILE_NOT_FOUND 2
#define Y_ERR_BAD_FILE 3



struct psf2_header {
	unsigned char magic[4];
	unsigned int version;
	unsigned int headersize;    /* offset of bitmaps in file */
	unsigned int flags;
	unsigned int length;        /* number of glyphs */
	unsigned int charsize;      /* number of bytes for each character */
	unsigned int height, width; /* max dimensions of glyphs */
	/* charsize = height * ((width + 7) / 8) */
};



typedef struct {
	struct psf2_header header;
	unsigned char *glyphs;
} font_t;


/**
 * \return 1 if header is valid, 0 otherwise
 */
int is_header_valid(struct psf2_header header);


/**
 * \brief Init a font with a file content.
 *
 * Read the font in a given file or in the default file if the given
 * file is not found.
 * \param err a pointer to an integer to put the error code
 * \param filename the name of the font file (psf)
 * \return a newly allocated font struct
 */
font_t *read_font(int *err, char *filename);

/**
 * \brief free memory use by a font
 */
void release_font(font_t *font);


void print_header_infos(struct psf2_header header);


/**
 * \return a pointer on the data for the glyph on index "number"
 */
unsigned char *get_character(font_t *font, int number);

#endif
