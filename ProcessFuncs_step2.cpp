/*****************************************************************************************
*																						 *
*	Author		  : Petar Bijev											                 *
*	year		  : 2013												                 *
*	Project title : VirtualGuitar												         *
*																				         *
*																	                     *
*	ProcessFuncs_step2.cpp																 *
*																						 *
*	This file contain functions, used to process the depth/color frames.				 *
*	The List of functions defined in this file (in order):								 *
*																						 *
*		- min																			 *
*		- max																			 *
*		- cmp // decrease																 *
*		- cmp_increase																	 *
*		- ConvertFromIJ																	 *
*		- ConvertToIJ																	 *
*		- isSkin																		 *
*		- rawDepthToMeters																 *
*		- isPoint_insideHandContour														 *
*																						 *
******************************************************************************************/

#include <OpenNI.h>
#include <PS1080.h>

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <opencv2\opencv.hpp>

#include "ProcessFuncs_calledFromMain.h"
#include "config.h"
#include "main_structs.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


cv::Mat skinCrCbHist = cv::Mat::zeros(cv::Size(256, 256), CV_8UC1);


/*
decreasing_compare funtion for Qsort
*/
int cmp (const void *a, const void *b)
{
    const double *da = (const double *) a;
    const double *db = (const double *) b;
     
    return -((*da > *db) - (*da < *db));
}

/*
increasing_compare funtion for Qsort
*/
int cmp_increase (const void *a, const void *b)
{
    const double *da = (const double *) a;
    const double *db = (const double *) b;
     
    return ((*da > *db) - (*da < *db));
}

/* 
This funtion convert the pixel position in 2D format (i,j) to 1D array format.
*/
int ConvertFromIJ(int height, int width){
	
	int PixelPosition = 0;
		PixelPosition = height*640 + width;

	return PixelPosition;
}
/* End of function "ConvertFromIJ()" */

/* 
This funtion convert the pixel position in 1D array format to 2D (i,j) format.
*/
void ConvertToIJ(int PixelPosition, int *height,  int *width){

	*height = PixelPosition/640;
	*width = PixelPosition - (*height*640);

	return;
}
/* End of function "ConvertToIJ()" */


/*
complicated function for skin color detection. option 0 in config file for the param "ScinDetect_simple" 
*/
bool isSkin(const cv::Scalar& color) { 
	
	ellipse(skinCrCbHist, cv::Point(113, 155.6), cv::Size(23.4, 15.2), 43.0, 0.0, 360.0, cv::Scalar(255, 255, 255), -1); 
	
	cv::Mat input = cv::Mat(cv::Size(1, 1), CV_8UC3, color);
    cv::Mat output;

    cvtColor(input, output, CV_BGR2YCrCb);
	cv::Vec3b ycrcb = output.at<cv::Vec3b>(0, 0);
    
	return ((skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) > 0));
}
/*End of function isSkin */


/* 
This function convert depth results. it is usefull for convertCoordinatesDepthToColor(). Currently it is not used.
*/
float rawDepthToMeters(int depthValue) 
{
    if(depthValue < 2047) 
	{
		return (float)(1.0 / ((double)(depthValue)* -0.0030711016 + 3.3309495161));
    }
    return 0.0f;
  }
/* End of function rawDepthToMeters() */


/*
This function check if a point of the image is inside of a predefined hand contour 
*/
int isPoint_insideHandContour(CvPoint *contour_points, CvPoint2D32f Point, int n)
{
	int i = 0;
	int    cn = 0;    // the  crossing number counter

    for ( i=0; i<n; i++)
	{   
       if (((contour_points[i].y <= Point.y) && (contour_points[i+1].y >  Point.y))     /* an upward crossing */
        || ((contour_points[i].y >  Point.y) && (contour_points[i+1].y <= Point.y))) /* a downward crossing */
	   { 
            /* compute  the actual edge-ray intersect x-coordinate */
            float vt = (float)(Point.y  - contour_points[i].y) / (contour_points[i+1].y - contour_points[i].y);
            if (Point.x <  contour_points[i].x + vt * (contour_points[i+1].x - contour_points[i].x)) // P.x < intersect
			{ 
				cn++;   // a valid crossing of y=P.y right of P.x
			}
        }
    }
    return (cn%2);    // 0 if even (out), and 1 if  odd (in)
}
/*isPoint_insideHandContour() */
