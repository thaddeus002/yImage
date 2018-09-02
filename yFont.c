/*
 * Copyright (c) 2009-2017 Yannick Garcia <thaddeus.dupont@free.fr>
 *
 * yImage is free software; you can redistribute it and/or modify
 * it under the terms of the GPL license. See LICENSE for details.
 */

/**
 * \file yFont.c
 * \brief font management.
 *
 * This can handle fonts in the PSF2 format.
 * 
 * The fonts here are bitmap fonts (not, for example, vector fonts), and
 * each glyph has a height and a width. The bitmap for a glyph is stored
 * as height consecutive pixel rows, where each pixel row consists of
 * width bits followed by some filler bits in order to fill an integral
 * number of (8-bit) bytes. Altogether the bitmap of a glyph takes
 * charsize bytes.
 *
 * The number of glyphs in the font equals length.
 *
 * The font is followed by a table associating Unicode values with each
 * glyph in case the PSF2_HAS_UNICODE_TABLE bit is set in the flags
 * field.
 *
 * The starting offset of the bitmaps in the font file is given by
 * headersize.
 *
 * The integers in the psf2 header struct are little endian 4-byte
 * integers.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yFont.h"
#include "yLat1-14.h"


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


/* returns 1 if header is valid, 0 otherwise */
int is_header_valid(struct psf2_header header){

	if(header.magic[0]!=PSF2_MAGIC0) return 0;
	if(header.magic[1]!=PSF2_MAGIC1) return 0;
	if(header.magic[2]!=PSF2_MAGIC2) return 0;
	if(header.magic[3]!=PSF2_MAGIC3) return 0;
	if(header.version>PSF2_MAXVERSION) return 0;

	return 1;
}


/**
 * Read the font in an array of unsigned char.
 */
static font_t *read_array_font(int *err, unsigned char *binary) {

	font_t *font;
	int data_size;

	*err=0;

	font = malloc(sizeof(font_t));
	if(font==NULL){
		*err=Y_ERR_ALLOCATE_FAIL;
		return(NULL);
	}

    memcpy(&(font->header), binary, sizeof(struct psf2_header)); 

	if(!is_header_valid(font->header)){
		*err=Y_ERR_BAD_FILE;
		free(font);
		return(NULL);
	}

	data_size=sizeof(unsigned char)*(font->header.length*font->header.charsize);
	font->glyphs=malloc(data_size);

	if(font->glyphs==NULL){
		*err=Y_ERR_ALLOCATE_FAIL;
		free(font);
		return(NULL);
	}

    memcpy(font->glyphs, binary+sizeof(struct psf2_header), data_size);

	return(font);
}


font_t *read_default_font(int *err) {
    return read_array_font(err, yLat1_14_psfu);
}


font_t *read_font(int *err, char *filename){

	font_t *font;
	FILE *fd;
	int data_size;
	int nb_lus;

	*err=0;

	fd=fopen(filename, "r");

	if(fd==NULL){
		*err=Y_ERR_FILE_NOT_FOUND;
		return(NULL);
	}

	font = malloc(sizeof(font_t));
	if(font==NULL){
		*err=Y_ERR_ALLOCATE_FAIL;
		fclose(fd);
		return(NULL);
	}

	nb_lus=fread(&(font->header), sizeof(struct psf2_header), 1, fd);

	if((nb_lus<1)||(!is_header_valid(font->header))){
		*err=Y_ERR_BAD_FILE;
		free(font);
		fclose(fd);
		return(NULL);
	}

	data_size=sizeof(unsigned char)*(font->header.length*font->header.charsize);
	font->glyphs=malloc(data_size);

	if(font->glyphs==NULL){
		*err=Y_ERR_ALLOCATE_FAIL;
		free(font);
		fclose(fd);
		return(NULL);
	}

	nb_lus=fread(font->glyphs, data_size, 1, fd);

	if(nb_lus<1){
		*err=Y_ERR_BAD_FILE;
		free(font->glyphs);
		free(font);
		fclose(fd);
		return(NULL);
	}

	fclose(fd);
	return(font);
}


/* free memory use by a font */
void release_font(font_t *font){
	if(font==NULL) return;

	if(font->glyphs!=NULL) free(font->glyphs);

	free(font);
}


void print_header_infos(struct psf2_header header){

	fprintf(stdout, "Header PSF2:\n");
	fprintf(stdout, "\tMagic: %c%c%c%c\n", header.magic[3], header.magic[2],header.magic[1], header.magic[0]);
	fprintf(stdout, "\tVersion: %d\n", header.version);
	fprintf(stdout, "\tHeader's size: %d\n", header.headersize);
	fprintf(stdout, "\tFlags: %d -", header.flags);
	if(header.flags && PSF2_HAS_UNICODE_TABLE) fprintf(stdout, " Has_unicode_table");
	fprintf(stdout, "\n");
	fprintf(stdout, "\tNumber of glyphs: %d\n", header.length);
	fprintf(stdout, "\tNumber of bytes by character: %d\n", header.charsize);
	fprintf(stdout, "\tMax size of glyphs: %dx%d\n\n", header.height, header.width);
}

/* returns a pointer on the data for the glyph on index "number" */
/* TODO make this static */
unsigned char *get_character(font_t *font, int number){


	if(font->glyphs==NULL) return NULL;

	if(number <= 127) {
		return(font->glyphs+(font->header.charsize)*number);
	}

	/* Use the default glyph for non ASCII character */
	return(font->glyphs);
}


unsigned char *get_glyph(font_t *font, char *character, int *nbBytes){

	int first = (unsigned char) character[0];
	*nbBytes = 1;
	int scalar = first;

	if(first <= 127) {
		return get_character(font, first);
	}

	if(first >> 5 == 6) {
		*nbBytes = 2;
	} else if (first >> 4 == 14) {
		*nbBytes = 3;
	} else if (first >> 3 == 30) {
		*nbBytes = 4;
	}

	int i;
	for (i=1; i < *nbBytes; i++) {
		scalar = scalar * 256 + (unsigned char) character[i];
	}
	return get_character(font, scalar);
}
