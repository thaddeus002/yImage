/*
 * Copyright (c) 2009-2017 Yannick Garcia <thaddeus.dupont@free.fr>
 *
 * yImage is free software; you can redistribute it and/or modify
 * it under the terms of the GPL license. See LICENSE for details.
 */

/**
 * \file yDraw.c
 * \brief Basic functions for drawing
 */

#include <math.h> // round()
#include "yImage.h"
#include "yDraw.h"


int y_fill_image(yImage *im, yColor *c){

    int i; /* counter */

    for(i=0; i<im->rgbWidth*im->rgbHeight; i++){
        im->rgbData[3*i]=c->r;
        im->rgbData[3*i+1]=c->g;
        im->rgbData[3*i+2]=c->b;
        im->alphaChanel[i]=c->alpha;
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


int y_draw_point(yImage *im, yPoint P, yColor *c){

    int index = convert_point_to_index(im, P);

    if(index < 0) return 0;

    if((im->presShapeColor==1) && (c->alpha=0)){
        im->rgbData[3*index]=im->shapeColor.r;
        im->rgbData[3*index+1]=im->shapeColor.g;
        im->rgbData[3*index+2]=im->shapeColor.b;
    } else {
        im->rgbData[3*index]=c->r;
        im->rgbData[3*index+1]=c->g;
        im->rgbData[3*index+2]=c->b;
        im->alphaChanel[index]=c->alpha;
    }

    return 1;
}



int y_draw_line(yImage *im, yPoint M, yPoint N, yColor *c){

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

    if((deltaX==0) && (deltaY==0)) return y_draw_point(im, M, c);

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
            tmp = y_draw_point(im, pointCourant, c);
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
            tmp = y_draw_point(im, pointCourant, c);
            if(!drawn) drawn=tmp;
        }
    }

    return drawn;
}


void y_draw_lines(yImage *im, yColor *color, yPoint *points, int nbPoints){

    int i;

    if(nbPoints == 0) return;

    if(nbPoints == 1) {
        y_draw_point(im, points[0], color);
        return;
    }

    for(i=0; i<nbPoints-1; i++) {

        yPoint pointI, pointJ;

        pointI.X = points[i].X;
        pointI.Y = points[i].Y;
        pointJ.X = points[i+1].X;
        pointJ.Y = points[i+1].Y;

        y_draw_line(im, pointI, pointJ, color);
    }
}


/**
 * Find the "bounding box" of a polygon, within an image.
 * \param image the image in which there is a polygon
 * \param bounds a pre-allocated array of points of size 2
 * \param points the polygon's cormers
 * \param nbPoints the number of points in the polygon
 */
static void find_boundaries(yImage *image, yPoint *bounds, yPoint *points, int nbPoints) {

    yPoint *min = bounds;
    yPoint *max = bounds+1;
    int i;

    min->X=image->rgbWidth+1;
    min->Y=image->rgbHeight+1;
    max->X=-1;
    max->Y=-1;

    for(i=0; i<nbPoints; i++) {
        yPoint *point = points+i;

        if(point->X > max->X) {
            max->X=point->X;
        }

        if(point->X < min->X) {
            min->X=point->X;
        }

        if(point->Y > max->Y) {
            max->Y=point->Y;
        }

        if(point->Y < min->Y) {
            min->Y=point->Y;
        }
    }
}


/*
 * Use the even-odd rule. The SVG specification says:
 * This rule determines the "insideness" of a point on the canvas by
 * drawing a ray from that point to infinity in any direction and
 * counting the number of path segments from the given shape that the
 * ray crosses. If this number is odd, the point is inside; if even,
 * the point is outside.
 *
 * For this implementation we increase x to infinite, y remaining
 * constant.
 */
static int is_point_in_polygon(yPoint P, yPoint *points, int nbPoints){

    int i, j;
    int c;

    j=nbPoints-1;
    c=0;

    for(i=0; i<nbPoints; i++) {

        if( ((points[i].Y > P.Y) != (points[j].Y > P.Y)) &&
            (P.X < points[i].X + (points[j].X - points[i].X) * (P.Y - points[i].Y) / (points[j].Y - points[i].Y))
        ) {
            c=!c;
        }

        j=i;
    }

    return c;
}


void y_fill_polygon(yImage *im, yColor *color, yPoint *points, int nbPoints){

    yPoint bounds[2];
    int x, y;

    find_boundaries(im, bounds, points, nbPoints);

    if(bounds[0].X > bounds[1].X || bounds[0].Y > bounds[1].Y) {
        // An error occured while findind boundaries
        return;
    }

    for(x=bounds[0].X; x<=bounds[1].X; x++) {
        for(y=bounds[0].Y; y<=bounds[1].Y; y++) {
            yPoint P;
            P.X=x;
            P.Y=y;
            if(is_point_in_polygon(P, points, nbPoints)) {
                y_draw_point(im, P, color);
            }
        }
    }
}


static yPoint translate(yPoint origin, int x, int y) {
    yPoint translated;
    translated.X=origin.X + x;
    translated.Y=origin.Y + y;
    return translated;
}

/*
 * This algorithm draws all eight octants simultaneously, starting from each
 * cardinal direction (0°, 90°, 180°, 270°) and extends both ways to reach
 * the nearest multiple of 45° (45°, 135°, 225°, 315°). It can determine
 * where to stop because when y = x, it has reached 45°. Between 0 and 45°,
 * as y increases, it does not skip nor repeat any y value until reaching
 * 45°. So during the while loop, y increments by 1 each iteration, and x
 * decrements by 1 on occasion, never exceeding 1 in one iteration.
 */
void y_draw_circle(yImage *im, yColor *color, yPoint center, int radius) {

    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while(x>=y) {
        y_draw_point(im, translate(center, x, y) , color);
        y_draw_point(im, translate(center, y, x) , color);
        y_draw_point(im, translate(center, -y, x) , color);
        y_draw_point(im, translate(center, -x, y) , color);
        y_draw_point(im, translate(center, -x, -y) , color);
        y_draw_point(im, translate(center, -y, -x) , color);
        y_draw_point(im, translate(center, y, -x) , color);
        y_draw_point(im, translate(center, x, -y) , color);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}


static int dist_square(yPoint *M, yPoint *N) {
    return (M->X-N->X)*(M->X-N->X) + (M->Y-N->Y)*(M->Y-N->Y);
}


void y_fill_circle(yImage *im, yColor *color, yPoint center, int radius) {

    yPoint bounds[2];
    int x, y;

    if(radius < 0) {
        radius = -radius;
    }

    bounds[0].X=center.X-radius;
    bounds[1].X=center.X+radius;
    bounds[0].Y=center.Y-radius;
    bounds[1].Y=center.Y+radius;

    for(x=bounds[0].X; x<=bounds[1].X; x++) {
        for(y=bounds[0].Y; y<=bounds[1].Y; y++) {
            yPoint P;
            P.X=x;
            P.Y=y;
            int r2 = radius*radius;
            if(dist_square(&P, &center) <= r2) {
                y_draw_point(im, P, color);
            }
        }
    }
}

