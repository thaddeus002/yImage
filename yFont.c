/*
 * Copyright (c) 2009-2017 Yannick Garcia <thaddeus.dupont@free.fr>
 *
 * yImage is free software; you can redistribute it and/or modify
 * it under the terms of the GPL license. See LICENSE for details.
 */

/**
 * \file yFont.c
 * \brief font management.
 */


#include <stdio.h>
#include <stdlib.h>
#include "yFont.h"

#ifndef INSTALL_DIR
/* the prefix for fonts file installation */
#define INSTALL_DIR "/usr"
#endif

#define DEFAULT_FONT "yLat1-14.psfu"

static char FontFileName[4000];


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


static char *getDefaultFontFilename() {
    sprintf(FontFileName, "%s/share/consolefonts/%s", INSTALL_DIR, DEFAULT_FONT);
    return FontFileName;
}


font_t *read_default_font(int *err) {
    return read_font(err, getDefaultFontFilename());
}


font_t *read_font(int *err, char *filename){

	font_t *font;
	FILE *fd;
	int taille_data;
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

	taille_data=sizeof(unsigned char)*(font->header.length*font->header.charsize);
	font->glyphs=malloc(taille_data);

	if(font->glyphs==NULL){
		*err=Y_ERR_ALLOCATE_FAIL;
		free(font);
		fclose(fd);
		return(NULL);
	}

	nb_lus=fread(font->glyphs, taille_data, 1, fd);

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
unsigned char *get_character(font_t *font, int number){

	if(number>=font->header.length) return NULL;

	if(font->glyphs==NULL) return NULL;

	return(font->glyphs+(font->header.charsize)*number);
}

