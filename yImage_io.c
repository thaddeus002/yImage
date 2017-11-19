/**
 * \file yImage_io.c : sauvegarder des images dans différents formats
 * fonctions inspirées de la bibliothèque Imlib 1.x
 *      => yImage est sous licence GPL (GNU GENERAL PUBLIC LICENSE)
 */


#include "yImage_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h> //memset()

#ifdef HAVE_LIBPNG
#include "png.h"
#endif
#ifdef HAVE_LIBJPEG
#include "jpeglib.h"
#endif
#ifdef HAVE_LIBTIFF
#include "tiffio.h"
#endif




int save_ppm(yImage *im, const char *file){
    FILE *f; /* file descriptor */

    f = fopen(file, "wb");
    if (f)
    {
        if (!fprintf(f, "P6\n# Created by yImage\n%i %i\n255\n", im->rgbWidth, im->rgbHeight))
        {
            fclose(f);
            return 1;
        }
        if (!fwrite(im->rgbData, 1, (im->rgbWidth * im->rgbHeight * 3), f))
        {
            fclose(f);
            return 1;
        }
        fclose(f);
        return 0;
    }
    return 1;
}



int save_jpeg(yImage *im, const char *file)
{
    #ifdef HAVE_LIBJPEG
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    int row_stride;
    FILE *f; /* file to create descriptor */

    f = fopen(file, "wb");
    if(f)
    {
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);
        jpeg_stdio_dest(&cinfo, f);
        cinfo.image_width = im->rgbWidth;
        cinfo.image_height = im->rgbHeight;
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;
        jpeg_set_defaults(&cinfo);
        jpeg_set_quality(&cinfo, (100 * DEFAULT_JPEG_QUALITY) >> 8, TRUE);
        jpeg_start_compress(&cinfo, TRUE);
        row_stride = cinfo.image_width * 3;
        while (cinfo.next_scanline < cinfo.image_height)
        {
            row_pointer[0] = im->rgbData + (cinfo.next_scanline * row_stride);
            jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }
        jpeg_finish_compress(&cinfo);
        fclose(f);
        return 0;
    }
    #endif
    return 1;
}



int save_png(yImage *im, const char *file)
{
    #ifdef HAVE_LIBPNG
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned char *data, *ptr;
    int x, y;
    png_bytep row_ptr;
    png_color_8 sig_bit;
    FILE *f; /* descripteur du fichier à créer */

    f = fopen(file, "wb");
    if (f)
    {
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr)
        {
            fclose(f);
            fprintf(stderr, "Fail create png file %s\n", file);
            return 1;
        }
        info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr == NULL)
        {
            fclose(f);
            png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
            fprintf(stderr, "Fail create png file %s\n", file);
            return 2;
        }
        //if (setjmp(png_ptr->jmpbuf))
        if (setjmp(png_jmpbuf(png_ptr)))
        {
            fclose(f);
            png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
            fprintf(stderr, "Fail create png file %s\n", file);
            return 3;
        }
        png_init_io(png_ptr, f);
        png_set_IHDR(png_ptr, info_ptr, im->rgbWidth, im->rgbHeight, 8,
            PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        sig_bit.red = 8;
        sig_bit.green = 8;
        sig_bit.blue = 8;
        sig_bit.alpha = 8;
        png_set_sBIT(png_ptr, info_ptr, &sig_bit);
        png_write_info(png_ptr, info_ptr);
        png_set_shift(png_ptr, &sig_bit);
        png_set_packing(png_ptr);
        data = (unsigned char *) malloc(im->rgbWidth * 4);
        if (!data)
        {
            fclose(f);
            png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
            fprintf(stderr, "Fail create png file %s : no data\n", file);
            return 4;
        }
        for (y = 0; y < im->rgbHeight; y++)
        {
            ptr = im->rgbData + (y * im->rgbWidth * 3);
            for (x = 0; x < im->rgbWidth; x++)
            {
                data[(x << 2) + 0] = *ptr++;
                data[(x << 2) + 1] = *ptr++;
                data[(x << 2) + 2] = *ptr++;
                if(im->alphaChanel!=NULL)
                    data[(x << 2) + 3] = im->alphaChanel[ x + y*im->rgbWidth ];
                else if(im->presShapeColor)
                    if ((data[(x << 2) + 0] == im->shapeColor.r) &&
                        (data[(x << 2) + 1] == im->shapeColor.g) &&
                        (data[(x << 2) + 2] == im->shapeColor.b))
                         data[(x << 2) + 3] = 0; /* transparent */
                    else
                        data[(x << 2) + 3] = 255; /* opaque */
                else
                    data[(x << 2) + 3] = 255;
            }
            row_ptr = data;
            png_write_rows(png_ptr, &row_ptr, 1);
        }
        free(data);
        png_write_end(png_ptr, info_ptr);
        png_destroy_write_struct(&png_ptr, (png_infopp) NULL);

        fclose(f);
        return 0;
    }
    #endif
    return 5;
}



int save_tiff(yImage *im, const char *file)
{
    #ifdef HAVE_LIBTIFF
    TIFF               *tif;
    unsigned char      *data;
    int                 y;

    tif = TIFFOpen(file, "w");
    if (tif)
    {
        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, im->rgbWidth);
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, im->rgbHeight);
        TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
        TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
        {
            TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
            /*w = */TIFFScanlineSize(tif);
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,
            TIFFDefaultStripSize(tif, -1));
            for (y = 0; y < im->rgbHeight; y++)
            {
                data = im->rgbData + (y * im->rgbWidth * 3);
                TIFFWriteScanline(tif, data, y, 0);
            }
        }
        TIFFClose(tif);
        return 0;
    }
    #endif
    return 1;
}



/* LOADING FILES */



static void ignore_comments(FILE *stream) {

    int c = fgetc(stream);

    while(c == '#') {
        int d = fgetc(stream);
        while (d != '\n' && d != EOF) {
            d = fgetc(stream);
        }
        c = fgetc(stream);
    }

    if(c != EOF) {
        ungetc(c, stream);
    }
}


yImage *load_ppm(const char *file) {

    FILE *f; /* file descriptor */
    yImage *im;

    f = fopen(file, "rb");
    if (f)
    {
        char magic[4]; // magic number
        char line[20]; // header line
        int w, h; // width and height of the image
        int n; // read on the file, must be 255
        int err; // error code
        int r; // number of elts read

        fread(magic, 1, 3, f);
        magic[3]='\0';

        if(strcmp(magic, "P6\n")) {
            fprintf(stderr, "Bad file format for %s\n", file);
            fclose(f);
            return NULL;
        }

        ignore_comments(f);

        fgets(line, 19, f);
        line[19]='\0';
        sscanf(line, "%d %d\n", &w, &h);

        ignore_comments(f);

        fgets(line, 19, f);
        line[19]='\0';
        sscanf(line, "%d\n", &n);

        ignore_comments(f);

        if(h < 0 || w < 0) {
            fprintf(stderr, "Bad file format for %s : width x height = %d x %d\n", file, w, h);
            fclose(f);
            return NULL;
        }

        if(n != 255) {
            fprintf(stderr, "Bad file format for %s : n = %d\n", file, n);
            fclose(f);
            return NULL;
        }


        im = create_yImage(&err, NULL, w, h);
        r=fread(im->rgbData, w * h * 3, 1, f);

        if(r != 1) {
            fprintf(stderr, "Reading PPM file %s : Unexpected end of file\n", file);
            destroy_yImage(im);
            im = NULL;
        }

        fclose(f);
        return im;
    }
    return NULL;
}




static yImage *LoadPNG(FILE *f);

yImage *load_png(const char *file) {

    FILE *fd;
    yImage *im = NULL;

    fd=fopen(file, "r");

    if(fd == NULL) {
        fprintf(stderr, "Could not open file %s\n", file);
        return NULL;
    }

    #ifdef HAVE_LIBPNG
    im = LoadPNG(fd);
    #endif

    fclose(fd);
    return im;
}


static yImage *LoadPNG(FILE *f)
{
    png_structp png_ptr;
    png_infop info_ptr;

    unsigned char *ptr, *ptr2; /* parcourt of data arrays */
    unsigned char **lines; /* PNG data */
    unsigned char r, g, b, a; /* RGBA of a pixel */
    int i, x, y/*, interlace_type*/;


    unsigned char *ptrAlpha;
    int width, height;
    png_byte color_type;
    char header[8];    // 8 is the maximum size that can be checked
    int err; /* error code */

    yImage *im=NULL;


    /* Init PNG Reader */

    fread((void *) header, 1, 8, f);
    if (png_sig_cmp((png_const_bytep)header, 0, 8))
    {
        return NULL;
    }


    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) return NULL;
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }


    png_init_io(png_ptr, f);

    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);

    /* Setup Translators */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);
    png_set_strip_16(png_ptr);
    png_set_packing(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);
    png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);


    /* Allocate memory for read png data*/
    lines = (unsigned char **)malloc(height * sizeof(unsigned char *));

    if (lines == NULL)
    {
        destroy_yImage(im);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }

    for (i = 0; i < height; i++)
    {
        if ((lines[i] = (unsigned char*) malloc(width * (sizeof(unsigned char) * 4))) == NULL)
        {
            int n;

            destroy_yImage(im);

            for (n = 0; n < i; n++)
                free(lines[n]);
            free(lines);
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            return NULL;
        }
    }

    /* Reading */
    png_read_image(png_ptr, lines);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);


    /* allocate memory for yImage structure */
    im=create_yImage(&err, NULL, width, height);
    if(im==NULL)
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }

    im->rgbData = (unsigned char*) malloc(im->rgbWidth * im->rgbHeight * 3);
    if (!im->rgbData)
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return NULL;
    }

    im->alphaChanel = (unsigned char*) malloc(im->rgbWidth * im->rgbHeight);
    if (!im->alphaChanel)
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      return NULL;
    }


    /* Data interpretation */
    ptr = im->rgbData;
    ptrAlpha= im->alphaChanel;

    if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        for (y = 0; y < im->rgbHeight; y++)
        {
            ptr2 = lines[y];
            for (x = 0; x < im->rgbWidth; x++)
            {
                r = *ptr2++;
                a = *ptr2++;

                *ptr++ = r;
                *ptr++ = r;
                *ptr++ = r;
                *ptrAlpha++=a;
            }
        }
    }

    else if (color_type == PNG_COLOR_TYPE_GRAY)
    {
        for (y = 0; y < im->rgbHeight; y++)
        {
            ptr2 = lines[y];
            for (x = 0; x < im->rgbWidth; x++)
            {
                r = *ptr2++;
                *ptr++ = r;
                *ptr++ = r;
                *ptr++ = r;
            }
        }
    }

    else
    {
        for (y = 0; y < im->rgbHeight; y++)
        {
            ptr2 = lines[y];
            for (x = 0; x < im->rgbWidth; x++)
            {
                r = *ptr2++;
                g = *ptr2++;
                b = *ptr2++;
                a = *ptr2++;

                *ptr++ = r;
                *ptr++ = g;
                *ptr++ = b;
                *ptrAlpha++=a;
            }
        }
    }

    /* Freing memory */
    for (i = 0; i < im->rgbHeight; i++)
        free(lines[i]);
    free(lines);

    /* End */
    return im;
}
