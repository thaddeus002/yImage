/**
 * \file yDraw.c
 * \brief Basic functions for drawing
 * \author Yannick Garcia
 * Licence : GPL v3
 */

#include <math.h> // round()
#include "yImage.h"
#include "yDraw.h"


int y_fill_image(yImage *im, yColor c){

  int i; /* counter */

  for(i=0; i<im->rgbWidth*im->rgbHeight; i++){
    im->rgbData[3*i]=c.r;
    im->rgbData[3*i+1]=c.g;
    im->rgbData[3*i+2]=c.b;
    im->alphaChanel[i]=c.alpha;
    im->presShapeColor=0;
  }

  return 0;
}


/**
 * \brief Give the index of a point.
 * \param im the image
 * \param P the point of interest
 * \return the index of P in the yImage or -1 if the point is not in the image
 */
static int convert_point_to_index(yImage *im, yPoint P){

    if((P.X<0) || (P.X>=im->rgbWidth) || (P.Y<0) || (P.Y>=im->rgbHeight)) {
        return -1;
    }

    return ((P.Y*im->rgbWidth) + P.X);
}


int y_draw_point(yImage *im, yPoint P, yColor c){

    int index = convert_point_to_index(im, P);

    if(index < 0) return 0;

    if((im->presShapeColor==1) && (c.alpha=0)){
        im->rgbData[3*index]=im->shapeColor.r;
        im->rgbData[3*index+1]=im->shapeColor.g;
        im->rgbData[3*index+2]=im->shapeColor.b;
    } else {
        im->rgbData[3*index]=c.r;
        im->rgbData[3*index+1]=c.g;
        im->rgbData[3*index+2]=c.b;
        im->alphaChanel[index]=c.alpha;
    }

    return 1;
}



int y_draw_line(yImage *im, yPoint M, yPoint N, yColor c){

  int orientation; /* 1 if Delta X > Delta Y, 0 otherwise */
  int deltaX;
  int deltaY;
  int Z; /* temporary coordonate */
  yPoint pointCourant;
  int drawn=0; /* return value */
  int tmp; /* temporary value */

  deltaX=N.X-M.X;
  if(deltaX<0) deltaX=-deltaX;
  deltaY=N.Y-M.Y;
  if(deltaY<0) deltaY=-deltaY;

  if((deltaX==0) && (deltaY==0)) return draw_point(im, M, c);

  if(deltaX>=deltaY) orientation=1;
  else orientation = 0;

  if(orientation){
    // boucle on X

    if(M.X>N.X){
      /* inversion of points */
      Z=M.X; M.X=N.X; N.X=Z;
      Z=M.Y; M.Y=N.Y; N.Y=Z;
    }

    for(pointCourant.X=M.X; pointCourant.X<=N.X; pointCourant.X++){

      pointCourant.Y=M.Y+(N.Y-M.Y)*(pointCourant.X-M.X)/deltaX;
      tmp = draw_point(im, pointCourant, c);
      if(!drawn) drawn=tmp;
    }

  } else {
    // boucle on Y

    if(M.Y>N.Y){
      /* inversion of points */
      Z=M.X; M.X=N.X; N.X=Z;
      Z=M.Y; M.Y=N.Y; N.Y=Z;
    }

    for(pointCourant.Y=M.Y; pointCourant.Y<=N.Y; pointCourant.Y++){

      pointCourant.X=M.X+(N.X-M.X)*(pointCourant.Y-M.Y)/deltaY;
      tmp = draw_point(im, pointCourant, c);
      if(!drawn) drawn=tmp;
    }
  }

  return drawn;
}


void y_draw_lines(yImage *im, yColor *color, yPoint *points, int nbPoints){

    int i;

    if(nbPoints == 0) return;

    if(nbPoints == 1) {
        draw_point(im, points[0], *color);
        return;
    }

    for(i=0; i<nbPoints-1; i++) {

        yPoint pointI, pointJ;

        pointI.X = points[i].X;
        pointI.Y = points[i].Y;
        pointJ.X = points[i+1].X;
        pointJ.Y = points[i+1].Y;

        draw_line(im, pointI, pointJ, *color);
    }
}

