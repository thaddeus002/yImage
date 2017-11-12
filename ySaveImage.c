/**
 * \file ySaveImage.c : sauvegarder des images dans différents formats
 * fonctions inspirées de la bibliothèque Imlib 1.x
 *      => yImage est sous licence GPL (GNU GENERAL PUBLIC LICENSE)
 */


#include "ySaveImage.h"
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



/* sauvegarde "im" dans "file" au format ppm ou pnm */
/* retourne 1 en cas de succes */
int sauve_ppm(yImage *im, const char *file){
    FILE *f; /* descripteur du fichier à créer */

    f = fopen(file, "wb");
    if (f)
    {
        if (!fprintf(f, "P6\n# Created by yImage\n%i %i\n255\n", im->rgbWidth, im->rgbHeight))
        {
            fclose(f);
            return 0;
        }
        if (!fwrite(im->rgbData, 1, (im->rgbWidth * im->rgbHeight * 3), f))
        {
            fclose(f);
            return 0;
        }
        fclose(f);
        return 1;
    }
    return(0);
}



/* sauvegarde "im" dans "file" au format JPEG */
/* retourne 1 en cas de succes */
/* nécessite la bibliothèque libjpeg */
int sauve_jpeg(yImage *im, const char *file)
{
    #ifdef HAVE_LIBJPEG
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    int row_stride;
    FILE *f; /* descripteur du fichier à créer */

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
        return(1);
    }
    #endif
    return(0);
}



/* sauvegarde "im" dans "file" au format PNG */
/* retourne 0 en cas de succes */
/* nécessite la bibliothèque libpng */
int sauve_png(yImage *im, const char *file)
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



/* sauvegarde "im" dans "file" au format TIFF */
/* retourne 1 en cas de succes */
/* nécessite la bibliothèque libtiff */
int sauve_tiff(yImage *im, const char *file)
{
    #ifdef HAVE_LIBTIFF
    TIFF               *tif;
    unsigned char      *data;
    int                 y;
    //int                 w;

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
        return 1;
    }
    #endif
    return(0);
}



/* LOADING FILES */

unsigned char *_LoadPPM(FILE * f, int *w, int *h)
{
    int                 done;
    unsigned char      *ptr;
    unsigned char       chr;
    char s[256]; /* read line */
    int                 a, b, i, j;
    int                 color /* is pixmap */, scale, ascii /* is ascii */, bw /* is bitmap */;

    a = b = scale = ascii = bw = color = 0;


    fgets(s, 256, f);
    s[2] = 0;
    if (!strcmp(s, "P6")) // pixmap binary
        color = 1;
    else if (!strcmp(s, "P5")) // gray binary
        color = 0;
    else if (!strcmp(s, "P4")) // bitmap binary
        bw = 1;
    else if (!strcmp(s, "P3")) // pixmap ascii
    {
        color = 1;
        ascii = 1;
    }
    else if (!strcmp(s, "P2")) // gray ascii
    {
        ascii = 1;
    }
    else if (!strcmp(s, "P1")) // bitmap ascii
    {
        ascii = 1;
        bw = 1;
    }
    else
        return NULL;



  done = 1;
  ptr = NULL;
  while (done)
    {
      if (fgets(s, 256, f) == NULL)
    break;

      if (s[0] != '#')
    {
      done = 0;
      sscanf(s, "%i %i", w, h);
      a = *w;
      b = *h;
      if (a > 32767)
        {
          fprintf(stderr, "IMLIB ERROR: Image width > 32767 pixels for file\n");
          return NULL;
        }
      if (b > 32767)
        {
          fprintf(stderr, "IMLIB ERROR: Image height > 32767 pixels for file\n");
          return NULL;
        }
      if (!bw)
        {
          fgets(s, 256, f);
          sscanf(s, "%i", &scale);
        }
      else
        scale = 99999;
      ptr = (unsigned char *)malloc(a * b * 3);
      if (!ptr)
        {
          fprintf(stderr, "IMLIB ERROR: Cannot allocate RAM for RGB data in file");
          return ptr;
        }
      if ((color) && (!ascii) && (!bw))
        {
          fread(ptr, a * b * 3, 1, f);
        }
      else if ((!color) && (!ascii) && (!bw))
        {
          b = (a * b * 3);
          a = 0;
          while ((fread(&chr, 1, 1, f)) && (a < b))
        {
          ptr[a++] = chr;
          ptr[a++] = chr;
          ptr[a++] = chr;
        }
        }
      else if ((!color) && (!ascii) && (bw))
        {
          b = (a * b * 3);
          a = 0;
          j = 0;
          while ((fread(&chr, 1, 1, f)) && (a < b))
        {
          for (i = 7; i >= 0; i--)
            {
              j++;
              if (j <= *w)
            {
              if (chr & (1 << i))
                {
                  ptr[a++] = 0;
                  ptr[a++] = 0;
                  ptr[a++] = 0;
                }
              else
                {
                  ptr[a++] = 255;
                  ptr[a++] = 255;
                  ptr[a++] = 255;
                }
            }
            }
          if (j >= *w)
            j = 0;
        }
        }
      else if ((color) && (ascii) && (!bw))
        {
          b = (a * b * 3);
          a = 0;
          i = 0;
          if (scale != 255)
        {
          while ((fread(&chr, 1, 1, f)) && (a < b))
            {
              s[i++] = chr;
              if (!isdigit(chr))
            {
              s[i - 1] = 0;
              if ((i > 1) && (isdigit(s[i - 2])))
                {
                  ptr[a++] = ((atoi(s)) * 255) / scale;
                }
              i = 0;
            }
            }
        }
          else
        {
          while ((fread(&chr, 1, 1, f)) && (a < b))
            {
              s[i++] = chr;
              if (!isdigit(chr))
            {
              s[i - 1] = 0;
              if ((i > 1) && (isdigit(s[i - 2])))
                {
                  ptr[a++] = atoi(s);
                }
              i = 0;
            }
            }
        }

        }
      else if ((!color) && (ascii) && (!bw))
        {
          b = (a * b * 3);
          a = 0;
          i = 0;
          if (scale != 255)
        {
          while ((fread(&chr, 1, 1, f)) && (a < b))
            {
              s[i++] = chr;
              if (!isdigit(chr))
            {
              s[i - 1] = 0;
              if ((i > 1) && (isdigit(s[i - 2])))
                {
                  ptr[a++] = ((atoi(s)) * 255) / scale;
                  ptr[a++] = ptr[a - 1];
                  ptr[a++] = ptr[a - 1];
                }
              i = 0;
            }
            }
        }
          else
        {
          while ((fread(&chr, 1, 1, f)) && (a < b))
            {
              s[i++] = chr;
              if (!isdigit(chr))
            {
              s[i - 1] = 0;
              if ((i > 1) && (isdigit(s[i - 2])))
                {
                  ptr[a++] = atoi(s);
                  ptr[a++] = ptr[a - 1];
                  ptr[a++] = ptr[a - 1];
                }
              i = 0;
            }
            }
        }
        }
      else if ((!color) && (ascii) && (bw))
        {
        }
    }
    }
  if (!ptr)
    return NULL;
  if (scale == 0)
    {
      free(ptr);
      return NULL;
    }
  if ((scale < 255) && (!ascii))
    {
      int                 rot;
      unsigned char      *po;

      if (scale <= 1)
    rot = 7;
      else if (scale <= 3)
    rot = 6;
      else if (scale <= 7)
    rot = 5;
      else if (scale <= 15)
    rot = 4;
      else if (scale <= 31)
            rot = 3;
        else if (scale <= 63)
            rot = 2;
        else
            rot = 1;

        if (rot > 0)
        {
            po = ptr;
            while (po < (ptr + (*w ** h * 3)))
            {
                *po++ <<= rot;
                *po++ <<= rot;
                *po++ <<= rot;
            }
        }
    }
    return ptr;
}





yImage *LoadPNG(FILE *f)
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
    //png_byte bit_depth;
    char header[8];    // 8 is the maximum size that can be checked
    int err; /* error code */

    yImage *im=NULL;


    /* Init PNG Reader */

    /*Raj*/
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

    /*Raj*/
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    //bit_depth = png_get_bit_depth(png_ptr, info_ptr);


    //png_get_IHDR(png_ptr, info_ptr, &ww, &hh, &bit_depth, &color_type, &interlace_type, NULL, NULL);


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

