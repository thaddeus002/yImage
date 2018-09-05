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
 *
 * The bitmaps may be followed by a Unicode description of the glyphs.
 * This Unicode description of a position has grammar
 *    <unicodedescription> := <uc>*<seq>*0xFF
 *    <seq> := 0xFE<uc><uc>*
 * where <uc> is a Unicode value coded in UTF-8, and * denotes zero or
 * more occurrences of the preceding item.
 *
 * The semantics is as follows. The leading <uc>* part gives Unicode
 * symbols that are all represented by this font position. The following
 * sequences are sequences of Unicode symbols - probably a symbol
 * together with combining accents - also represented by this font
 * position. 
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
 * Read an UTF8 encoded value in a array of char.
 */
static unsigned int read_utf8_value(unsigned char *character, int *nbBytes){

    int first = (unsigned char) character[0];
    *nbBytes = 1;
    unsigned int scalar = first;

    if(first <= 127) {
        return first;
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

    return scalar;
}


static void init_utf8_table(unsigned int *utf8_values, int nb_glyphs, unsigned char *data, int data_len) {

    int pos = 0;
    int i = 0;
    while (pos < data_len && i < nb_glyphs) {

        int nb;
        unsigned int value = read_utf8_value(data+pos, &nb);

        utf8_values[i] = value;

        pos+=nb;
        while(pos < data_len && data[pos] != PSF2_SEPARATOR) {
            pos++;
        }
        pos++;
        i++;
    }
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

    font->utf8_values = NULL;

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

    if(font->header.flags & PSF2_HAS_UNICODE_TABLE) {

        font->utf8_values = malloc(font->header.length * sizeof(unsigned int));

        if(font->utf8_values==NULL){
            *err=Y_ERR_ALLOCATE_FAIL;
            free(font->glyphs);
            free(font);
            return(NULL);
        }

        init_utf8_table(font->utf8_values, font->header.length, binary + sizeof(struct psf2_header) + data_size, yLat1_14_psfu_len);
    }

    return(font);
}


font_t *read_default_font(int *err) {
    return read_array_font(err, yLat1_14_psfu);
}


static void remove_flag(font_t *font, int flag) {
    font->header.flags = font->header.flags & (0xffffffff - flag);
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

    font->utf8_values = NULL;

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

    if(font->header.flags & PSF2_HAS_UNICODE_TABLE) {

        font->utf8_values = malloc(font->header.length * sizeof(unsigned int));

        if(font->utf8_values==NULL){
            *err=Y_ERR_ALLOCATE_FAIL;
            free(font->glyphs);
            free(font);
            fclose(fd);
            return(NULL);
        }

        long curpos = ftell(fd);
        fseek(fd, 0L, SEEK_END);
        long endpos = ftell(fd);
        fseek(fd, curpos, SEEK_SET);

        unsigned char *utf_data;
        utf_data = malloc(sizeof(unsigned char) * (endpos - curpos));
        
        if(utf_data==NULL){
            *err=Y_ERR_ALLOCATE_FAIL;
            free(font->utf8_values);
            free(font->glyphs);
            free(font);
            fclose(fd);
            return(NULL);
        }

        int nbr = fread(utf_data, endpos-curpos, 1, fd);
        // TODO handle the case nbr == 0
        if(nbr == 0) {
            // UTF8 support is not possible
            fprintf(stderr, "font has flag PSF2_HAS_UNICODE_TABLE but not data was found\n");
            remove_flag(font, PSF2_HAS_UNICODE_TABLE);
            free(font->utf8_values);
            font->utf8_values=NULL;
            free(utf_data);
            fclose(fd);
            return(font);
        }

        init_utf8_table(font->utf8_values, font->header.length, utf_data, endpos - curpos);

        free(utf_data);
    }

    fclose(fd);
    return(font);
}


/* free memory use by a font */
void release_font(font_t *font){
    if(font==NULL) return;

    if(font->glyphs!=NULL) free(font->glyphs);

    if(font->utf8_values!=NULL) free(font->utf8_values);

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

    if(number > 0) {
        return(font->glyphs+(font->header.charsize)*number);
    }

    /* If not found, use the default glyph */
    return(font->glyphs);
}


/**
 * Returns glyph index if exists for an utf8 value, or -1.
 */
static int glyph_index(font_t *font, unsigned int value) {

    int i;

    if(! (font->header.flags & PSF2_HAS_UNICODE_TABLE)) {
        // Only 7-bits ASCII is supported
        if(value<=32 || value > 127) {
            // use a default glyph
            return -1;
        }
        return value;
    }

    for(i=0; i<font->header.length; i++) {
        if(font->utf8_values[i] == value) {
            return i;
        }
    }

    return -1;
}

unsigned char *get_glyph(font_t *font, char *character, int *nbBytes){
    return get_character(font, glyph_index(font, read_utf8_value(character, nbBytes)));
}
