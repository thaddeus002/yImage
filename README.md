# libyImage

A simple, low level, drawing library in C.

## features

 *  Read PNG and PPM image files
 *  Save in PNG, PPM, JPEG or TIFF format
 *  Support transparency (alpha channel)
 *  Image superposition, like using calcs
 *  Draw lines and polygons
 *  Fill polygons
 *  Write text

## dependencies

Some extern libraries are optionally used :

 * libz and libpng-1.6 for png reading and writing
 * libjpeg for jpeg writing
 * libtiff for tiff writing

## build the lib

Simply do :

    $ make

to generated the static library file yImage.a
