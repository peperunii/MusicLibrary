﻿/*****************************************************************************************
*																						 *
*	Author		  : Petar Bijev											                 *
*	year		  : 2013												                 *
*	Project title : VirtualGuitar												         *
*																				         *
*																	                     *
*	ProcessFuncs_calledFromMain.cpp														 *
*																						 *
*	This file contain functions, used to process the depth/color frames.				 *
*	The List of functions defined in this file (in order):								 *
*																						 *
*		- DebugLayerCheck																 *
*		- FindMinDepthValue																 *
*		- AreHandsInSquares																 *
*		- skinColorDetect																 *
*		- convertCoordinatesDepthToColor												 *
*		- init_HandStructure															 *
*		- filter_and_threshold															 *
*		- find_contour																	 *
*		- find_convex_hull																 *
*		- find_convex_hull2																 *
*		- find_fingers																	 *
*		- find_fingers2																	 *
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
#include "ProcessFuncs_step2"
#include "config.h"
#include "main_structs.h"

#define NUM_FINGERS	7
#define NUM_DEFECTS	20
#define SHOW_HAND_CONTOUR 1



/*
This funtion return True if the current DebugLayer mode allows to print the current message
*/
bool DebugLayerCheck(int Message_layer){
	if(DEBUG_LAYER >= Message_layer) return true;
	else return false;
}
/*End of function DebugLayerChech*/



/*
This function calculate the minimum depth value from the depthmap(pDepth)
*/
int FindMinDepthValue(openni::DepthPixel* pDepth)
{
	int PixCounter = 0;
	int CurrentDepthValue = 0;
	int minDepthValue = 3000;

	for(PixCounter = 640*ROI_REMOVE_TOP; PixCounter < 640*(480- ROI_REMOVE_BOTTOM); PixCounter++)
	{
		CurrentDepthValue = pDepth[PixCounter];
		if (pDepth[PixCounter] == 0) continue;
		if (CurrentDepthValue < minDepthValue) minDepthValue = CurrentDepthValue;
	}

	return minDepthValue;
}
/*End of function FindMinDepthValue()*/




/* 
Check if both hands are in the squares. if true => exit initialization mode 
 Change color of the squares to indicate that the user is correctly placing his hands 
 */
void AreHandsInSquares(struct HandStructure *HandStructure, cv::Mat *OutputImage2, int *recognized_hands)
{
	if(HandStructure.Initialization_mode)
	{
		if(HandStructure.IsLeftHand_left)
		{
			if((HandStructure.hand_center_left.x >= 100) && (HandStructure.hand_center_left.x <=180) && (HandStructure.hand_center_left.y >= 200) && (HandStructure.hand_center_left.y <= 280))
			{
				cv::rectangle(*OutputImage2, cvPoint(100,200), cvPoint(180,280), cvScalar(0,128,64),CV_FILLED ,8,0);
				*recognized_hands+=1;
			}
			if((HandStructure.hand_center_right.x >= 400) && (HandStructure.hand_center_right.x <=480) && (HandStructure.hand_center_right.y >= 200) && (HandStructure.hand_center_right.y <= 280))
			{
				cv::rectangle(*OutputImage2, cvPoint(400,200), cvPoint(480,280), cvScalar(0,128,64),CV_FILLED ,8,0);
				*recognized_hands+=1;
			}
			if(*recognized_hands == 2) HandStructure.Initialization_mode = 0;
			else *recognized_hands = 0;
		}
		else
		{
			if((HandStructure.hand_center_right.x >= 100) && (HandStructure.hand_center_right.x <=180) && (HandStructure.hand_center_right.y >= 200) && (HandStructure.hand_center_right.y <= 280))
			{
				cv::rectangle(*OutputImage, cvPoint(100,200), cvPoint(180,280), cvScalar(0,128,64),CV_FILLED ,8,0);
				*recognized_hands+=1;
			}
			if((HandStructure.hand_center_left.x >= 400) && (HandStructure.hand_center_left.x <=480) && (HandStructure.hand_center_left.y >= 200) && (HandStructure.hand_center_left.y <= 280))
			{
				cv::rectangle(*OutputImage, cvPoint(400,200), cvPoint(480,280), cvScalar(0,128,64),CV_FILLED ,8,0);
				*recognized_hands+=1;
			}
			if(*recognized_hands == 2) { HandStructure.Initialization_mode = 0; }
			else *recognized_hands = 0;
		}
	}
	return;
}
/* End of function AreHandsInSquares() */



/*
This funtion decide, based on the parameters defined in the config file, is one point part of the skin or not.
*/
bool skinColorDetect(int PixelPosition, openni::RGB888Pixel* pColor){
	
	cv::Scalar color = cv::Scalar( 94, 206, 165 );
	bool skin_detect_inPixel = false;
	int minColorValue = 255;
	int maxColorValue = 0;
	int width, height;

	cv::Scalar color = cv::Scalar( pColor[height*640+width].b, pColor[height*640+width].g, pColor[height*640+width].r );
	ConvertToIJ(PixelPosition,&width,&height);

	if(SKIN_COLOR_ALGO_SIMPLE == 1){
		/* simple algorithm for detecting if a certain pixel is with skin color or not.*/
		/*
			R > 95 AND G > 40 AND B > 20 AND
			max{R, G, B} – min{R, G, B} >15 AND
			|R – G| > 15 AND
			R > G AND R > B
		*/
		if((pColor[PixelPosition].r > 95) && (pColor[PixelPosition].g > SKIN_DETECT_SIMPLE_G) && (pColor[PixelPosition].b > SKIN_DETECT_SIMPLE_B))
		{
			if((pColor[PixelPosition].r > pColor[PixelPosition].g) && (pColor[PixelPosition].r > pColor[PixelPosition].b))
			{
				if(abs(pColor[PixelPosition].r - pColor[PixelPosition].g) > 15 )
				{
					maxColorValue = max(pColor[PixelPosition].r, pColor[PixelPosition].g);
					maxColorValue = max(maxColorValue,pColor[PixelPosition].b);

					minColorValue = min(pColor[PixelPosition].r, pColor[PixelPosition].g);
					minColorValue = min(minColorValue, pColor[PixelPosition].b);

					if((maxColorValue - minColorValue) > 15 ) skin_detect_inPixel = true;
				}

			}

		}
	}

	else
	{
		/* more complicated algorithm. //Future work */
		skin_detect_inPixel = isSkin(color);
	}

	return skin_detect_inPixel;
}
/* End of function "skinColorDetect()" */


/*
This function convert coordinates between color and depth images
*/
void convertCoordinatesDepthToColor( int height, int width, int depthValue, int *x, int *y) {
  
	double x_world,y_world,z_world;
	double x_rgb,y_rgb,z_rgb,x_rgb2;

    double fx_d = 1.0 / 5.9421434211923247e+02;
    double fy_d = 1.0 / 5.9104053696870778e+02;
    double cx_d = 3.3930780975300314e+02;
    double cy_d = 2.4273913761751615e+02;   
	double cx_rgb = 3.2894272028759258e+02;
	double cy_rgb = 2.6748068171871557e+02;

	double depth =  rawDepthToMeters(depthValue);//rawDepthToMeters(depthValue);

	x_world = (double)((width- cx_d) * depth * fx_d);
    y_world = (double)((height - cy_d) * depth * fy_d);
	z_world = (double)depth;
    
	x_rgb = x_world  * 0.99984628826577793 + y_world* ((double)-1.4779/1000) + z_world* 0.01747 + 0.0199852423;
	x_rgb2 = x_world * 0.999983  + y_world * (-0.005852) +  z_world * (-0.000832)  -0.025526;
	y_rgb = 0;

	return ;
  }
/* End of function convertCoordinatesDepthToColor*/




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


/*
Function which initialize the initial values of HandStructure struct.
*/
void init_HandStructure(struct HandStructure *HandStructure)
{
 
    HandStructure->contour_right = NULL;
	HandStructure->contour_left = NULL;
    HandStructure->contour_right_forFingers = NULL;
	HandStructure->contour_left_forFingers = NULL;

	HandStructure->num_defects_left = 0;
	HandStructure->hand_center_left = cvPoint(0, 0);
	HandStructure->num_defects_right = 0;
	HandStructure->hand_center_right = cvPoint(0, 0);

	HandStructure->thr_image = cvCreateImage(cvGetSize(HandStructure->image), 8, 1);
	HandStructure->temp_image1 = cvCreateImage(cvGetSize(HandStructure->image), 8, 1);
	HandStructure->temp_image3 = cvCreateImage(cvGetSize(HandStructure->image), 8, 3);
	HandStructure->kernel = cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT);//(9, 9, 4, 4, CV_SHAPE_RECT, NULL);
	HandStructure->temp_st = cvCreateMemStorage(0);

	HandStructure->contour_st_left = cvCreateMemStorage(0);
	HandStructure->defects_st_left = cvCreateMemStorage(0);
	HandStructure->hull_st_left = cvCreateMemStorage(0);
	HandStructure->fingers_left = (CvPoint*)calloc(50, sizeof(CvPoint));
	HandStructure->defects_left = (CvPoint*)calloc(50, sizeof(CvPoint));

	HandStructure->contour_st_right = cvCreateMemStorage(0);
	HandStructure->defects_st_right = cvCreateMemStorage(0);
	HandStructure->hull_st_right = cvCreateMemStorage(0);
	HandStructure->fingers_right = (CvPoint*)calloc(50 + 1, sizeof(CvPoint));
	HandStructure->defects_right = (CvPoint*)calloc(50, sizeof(CvPoint));
}
/* End of function init_HandStructure() */



/* 
This function provide functionality of smoothing and thresholding the image
*/
void filter_and_threshold(struct HandStructure *HandStructure)
{
	/* Soften image */
	//if (FILTERING_SIMPLE == 0) cvSmooth(HandStructure->image, HandStructure->temp_image3, CV_GAUSSIAN, 11, 11, 0, 0);
	
	/* Remove some impulsive noise */
	if (FILTERING_SIMPLE == 0) cvSmooth(HandStructure->image, HandStructure->temp_image3, CV_MEDIAN, 11, 11, 0, 0);
	/*
	 * Apply threshold on RGB values
	 * Threshold values should be customized according to environment 
	 */
	cvInRangeS(HandStructure->image,
		   cvScalar(5, 5, 5),
		   cvScalar(255, 255, 255),
		   HandStructure->thr_image);

	/* Apply morphological opening */
	//cvMorphologyEx(HandStructure->thr_image, HandStructure->thr_image, NULL, HandStructure->kernel, CV_MOP_CLOSE, 1);
	//cvMorphologyEx(HandStructure->thr_image, HandStructure->thr_image, NULL, HandStructure->kernel,   CV_MOP_OPEN, 2);

	//if (FILTERING_SIMPLE == 0) cvMorphologyEx(HandStructure->thr_image, HandStructure->thr_image, NULL, HandStructure->kernel, CV_MOP_CLOSE, 2);
	//if (FILTERING_SIMPLE == 0) cvSmooth(HandStructure->thr_image, HandStructure->thr_image, CV_GAUSSIAN, 3, 3, 0, 0);
	
	return;
}
/* End of function filter_and_threshold() */


/*
This function search for all of the contours in the image and choose the biggest two among them
*/
void find_contour(struct HandStructure *HandStructure)
{
	double area, max_area = 0.0;
	double areas[15];
	int index = 0;
	int counter = 0;
	CvSeq *contours, *tmp, *contour_left = NULL, *contour_right = NULL;
	HandStructure->contour_left = NULL;
	HandStructure->contour_left_forFingers = NULL;
	HandStructure->contour_right = NULL;
	HandStructure->contour_right_forFingers = NULL;
	
	CvPoint *r=NULL;

	/* cvFindContours modifies input image, We use a copy */
	cvCopy(HandStructure->thr_image, HandStructure->temp_image1, NULL);
	if(CONTOUR_SIMPLE == 0)
	{
		cvFindContours(HandStructure->temp_image1, HandStructure->temp_st, &contours, sizeof(CvContour), CV_RETR_EXTERNAL,
											CV_CHAIN_APPROX_NONE , cvPoint(0, 0));
	}
	else 
	{
		cvFindContours(HandStructure->temp_image1, HandStructure->temp_st, &contours, sizeof(CvContour), CV_RETR_EXTERNAL,
											CV_CHAIN_APPROX_SIMPLE , cvPoint(0, 0));
	}

	/* Select contour having greatest area */
	for (tmp = contours; tmp; tmp = tmp->h_next) {
		counter = tmp->total;

		area = fabs(cvContourArea(tmp, CV_WHOLE_SEQ, 0));
		areas[index++]=area;
	}
	
	/* Sort the areas and take the one with the highest value */
	if(index > 0) qsort(areas,index, sizeof(areas[0]), cmp);
	
	for (tmp = contours; tmp; tmp = tmp->h_next) {
		area = fabs(cvContourArea(tmp, CV_WHOLE_SEQ, 0));
		if (index!=0) { if(area == areas[0]) contour_left = tmp;}
		if(index!=0 && index!=1) { if(area == areas[1]) contour_right = tmp; }
	}

	/* Approximate contour with poly-line */
	if (contour_left!=NULL) {
		HandStructure->contour_left_forFingers = contour_left;
		contour_left = cvApproxPoly(contour_left, sizeof(CvContour),
				       HandStructure->contour_st_left, CV_POLY_APPROX_DP, 2,
				       1);
		HandStructure->contour_left = contour_left;
	}

	if (contour_right!=NULL) {
		HandStructure->contour_right_forFingers = contour_right;
		contour_right = cvApproxPoly(contour_right, sizeof(CvContour),
				       HandStructure->contour_st_right, CV_POLY_APPROX_DP, 2,
				       1);
		HandStructure->contour_right = contour_right;
	}

	return;
}
/* End of function find_contour() */

/*
This function calculate a convex hull for the contour. It is used in calculation for finding the center of the palm
*/
void find_convex_hull(struct HandStructure *HandStructure)
{
	CvSeq *defects_left, *defects_right;
	CvConvexityDefect *defect_array_left, *defect_array_right;
	int i;
	int x = 0, y = 0;
	double dist = 0;

	HandStructure->hull_left = NULL;
	HandStructure->hull_right = NULL;
	
	/* tozi red .. .. zasega.. ne mi trqbva */
	if (!HandStructure->contour_left  )//|| !HandStructure->contour_right)
		return;

	HandStructure->hull_left = cvConvexHull2(HandStructure->contour_left, HandStructure->hull_st_left, CV_CLOCKWISE, 0); 
	if(HandStructure->contour_right){ HandStructure->hull_right = cvConvexHull2(HandStructure->contour_right, HandStructure->hull_st_right, CV_CLOCKWISE, 0); }

	/* Process the Left hand; */
	if (HandStructure->hull_left!=NULL) {
		
		/* Get convexity defects of contour w.r.t. the convex hull */
		defects_left = cvConvexityDefects(HandStructure->contour_left, HandStructure->hull_left, HandStructure->defects_st_left);

		if (defects_left && defects_left->total) {
			defect_array_left = (CvConvexityDefect*)calloc(defects_left->total,
					      sizeof(CvConvexityDefect));
			/* convert contour to array, so that each element can be accessed easily */
			cvCvtSeqToArray(defects_left, defect_array_left, CV_WHOLE_SEQ);

		//	fprintf(fp_debug,"defects: %d\n",defects_left->total);
			/* Average depth points to get hand center */
			for (i = 0; i < defects_left->total && i < NUM_DEFECTS; i++) {
				x += defect_array_left[i].depth_point->x;
				y += defect_array_left[i].depth_point->y;

				HandStructure->defects_left[i] = cvPoint(defect_array_left[i].depth_point->x,
							  defect_array_left[i].depth_point->y);
			}

			x /= defects_left->total;
			y /= defects_left->total;

			HandStructure->num_defects_left = defects_left->total;
			HandStructure->hand_center_left = cvPoint(x, y);
			
			/* Compute hand radius as mean of distances of
			   defects' depth point to hand center */
			for (i = 0; i < defects_left->total; i++) {
				double d = (x - defect_array_left[i].depth_point->x) *
					(x - defect_array_left[i].depth_point->x) +
					(y - defect_array_left[i].depth_point->y) *
					(y - defect_array_left[i].depth_point->y);

				dist += sqrt(d);
			}
			HandStructure->hand_radius_left = dist / defects_left->total;
			free(defect_array_left);
		}
	}
	/* End Processing the left hand */

	i = 0;
	x = 0;
	y = 0;
	dist = 0;
	/* Process the right hand */
	if (!HandStructure->contour_right  )//|| !HandStructure->contour_right)
		return;
	if (HandStructure->hull_right!=NULL) {
		
		/* Get convexity defects of contour w.r.t. the convex hull */
		defects_right = cvConvexityDefects(HandStructure->contour_right, HandStructure->hull_right, HandStructure->defects_st_right);

		if (defects_right && defects_right->total) {
			
			defect_array_right = (CvConvexityDefect*)calloc(defects_right->total,
					      sizeof(CvConvexityDefect));
			/* convert contour to array, so that each element can be accessed easily */
			cvCvtSeqToArray(defects_right, defect_array_right, CV_WHOLE_SEQ);

			/* Average depth points to get hand center */
			for (i = 0; i < defects_right->total && i < NUM_DEFECTS; i++) {
				x += defect_array_right[i].depth_point->x;
				y += defect_array_right[i].depth_point->y;

				HandStructure->defects_right[i] = cvPoint(defect_array_right[i].depth_point->x,
							  defect_array_right[i].depth_point->y);
			}

			x /= defects_right->total;
			y /= defects_right->total;

			HandStructure->num_defects_right = defects_right->total;
			HandStructure->hand_center_right = cvPoint(x, y);
			
			/* Compute hand radius as mean of distances of
			   defects' depth point to hand center */
			for (i = 0; i < defects_right->total; i++) {
				double d = (x - defect_array_right[i].depth_point->x) *
					(x - defect_array_right[i].depth_point->x) +
					(y - defect_array_right[i].depth_point->y) *
					(y - defect_array_right[i].depth_point->y);

				dist += sqrt(d);
			}
			HandStructure->hand_radius_right = dist / defects_right->total;
			free(defect_array_right);
		}
	}
	/* End processing the right hand */

	return;
}
/* End of function find_convex_hull() */


/* 
Complicated function for finding the center of the palms
*/
void find_convex_hull2(struct HandStructure *HandStructure)
{
	//CvSeq *defects_left, *defects_right;
	//CvConvexityDefect *defect_array_left, *defect_array_right;
	long int i;
	int n;
	int inner_counter = 0;
	int x = 0, y = 0;
	double dist_max = 0;
	double dist_cur = 0;
	int result_dist = 0;
	int width = 0, height = 0;
	CvPoint *points_left = NULL, *points_right = NULL;
	CvPoint *points_inner_left = NULL, *points_inner_right = NULL;
	CvPoint *everything = NULL;
	double side_x, side_y, dist_strana;
	int j;
	long int nomer = 0, nomer2 =0;
	double *MasivTochkiRazstoqniq = (double*)calloc(640*480, sizeof(double));
	/* tozi red .... zasega.. ne mi trqbva */
	if (!HandStructure->contour_left_forFingers  )//|| !HandStructure->contour_right)
		return;

	n = HandStructure->contour_left->total;
	points_left = (CvPoint*)calloc(n, sizeof(CvPoint));
	points_inner_left = (CvPoint*)calloc(640*480, sizeof(CvPoint));
	everything = (CvPoint*)calloc(640*480, sizeof(CvPoint));
	/* convert contour to array, so that each element can be accessed easily */
	cvCvtSeqToArray(HandStructure->contour_left, points_left, CV_WHOLE_SEQ);
	/* convert between two point types - for use in isPoint_insideHandContour() function */
	CvPoint2D32f point_s = cvPoint2D32f((double)x, (double)y);
	
	
	nomer = 0;
	for(i = ROI_REMOVE_TOP*640; i < 640*ROI_REMOVE_BOTTOM;)
	{  

		nomer = 0;
		double *MasivRazstoqniq = (double*)calloc(n, sizeof(double));
		ConvertToIJ(i,&height, &width);

		point_s.x = (float)width;
		point_s.y = (float)height;

		result_dist = isPoint_insideHandContour( points_left, point_s, n);
		if (result_dist > 0) 
		{ 
			for(j=0; j<n; j++)
			{
				side_x = abs(points_left[j].x - everything[i].x);
				side_y = abs(points_left[j].y - everything[i].y);
				dist_strana = sqrt((long double)(pow((long double)side_x,2) + pow((long double)side_y,2)));
				if(dist_strana == 0) continue;
				MasivRazstoqniq[nomer++] = dist_strana;
				j+=2;
			}

			qsort(MasivRazstoqniq,nomer, sizeof(MasivRazstoqniq[0]), cmp_increase);
			MasivTochkiRazstoqniq[nomer2++] = MasivRazstoqniq[0];
			i+=5;   /* This numbers should be computed every time depending on the size in point of the contour */
		}
		else i+=10; /* This numbers should be computed every time depending on the size in point of the contour */

	}

	/* Sort all the distances, choose the biggest one */
	qsort(MasivTochkiRazstoqniq,nomer2, sizeof(MasivTochkiRazstoqniq[0]), cmp);
	HandStructure->hand_radius_left = (double)MasivTochkiRazstoqniq[0];

	nomer = 0;
	nomer2 = 0;
	for(i = ROI_REMOVE_TOP*640; i < 640*ROI_REMOVE_BOTTOM;)
	{  
		nomer = 0;
		double *MasivRazstoqniq = (double*)calloc(n, sizeof(double));
		ConvertToIJ(i,&height, &width);

		point_s.x = (float)width;
		point_s.y = (float)height;
		
		result_dist = isPoint_insideHandContour( points_left, point_s, n);
		if (result_dist > 0) 
		{ 
			for(j=0; j<n; j++)
			{
				side_x = abs(points_left[j].x - everything[i].x);
				side_y = abs(points_left[j].y - everything[i].y);
				dist_strana = sqrt((long double)(pow((long double)side_x,2) + pow((long double)side_y,2)));
				if(dist_strana == 0) continue;
				MasivRazstoqniq[nomer++] = dist_strana;
				j+=2;
			}

			/* Sort the distances. Choose the smallest one */
			qsort(MasivRazstoqniq,nomer, sizeof(MasivRazstoqniq[0]), cmp_increase);
			if(MasivRazstoqniq[0] == MasivTochkiRazstoqniq[0]) { HandStructure->hand_center_left.x = width; HandStructure->hand_center_left.y = height; break;}
			i+=5;    /* This numbers should be computed every time depending on the size in point of the contour */
		}
		else i+=10;  /* This numbers should be computed every time depending on the size in point of the contour */
	}

	if(HandStructure->contour_right_forFingers  )
	{
		/* The same processing as for the left hand */
	}

	return;
}
/* End of function find_convex_hull2() */


/*
This function is trying find 5 finger candidates for each hand 
*/
void find_fingers(struct HandStructure *HandStructure)
{
	int n;
	int i;
	CvPoint *points;
	CvPoint max_point;
	int dist1 = 0, dist2 = 0;
	int finger_distance[NUM_FINGERS + 1];
	HandStructure->num_fingers_left = 0;

	if (!HandStructure->contour_left )
		return;

	n = HandStructure->contour_left->total;
	points = (CvPoint*)calloc(n, sizeof(CvPoint));
    
	/* convert contour to array, so that each element can be accessed easily */
	cvCvtSeqToArray(HandStructure->contour_left, points, CV_WHOLE_SEQ);
	/*
	 * Fingers are detected as points where the distance to the center
	 * is a local maximum
	 */
	for (i = 0; i < n; i++) 
	{
		int dist;
		int cx = HandStructure->hand_center_left.x;
		int cy = HandStructure->hand_center_left.y;

		dist = (cx - points[i].x) * (cx - points[i].x) +
		    (cy - points[i].y) * (cy - points[i].y);

		if (dist < dist1 && dist1 > dist2 && max_point.x != 0
		    && max_point.y < cvGetSize(HandStructure->image).height - 10)
		{

			finger_distance[HandStructure->num_fingers_left] = dist;
			HandStructure->fingers_left[HandStructure->num_fingers_left++] = max_point;
			if (HandStructure->num_fingers_left >= NUM_FINGERS + 1)
				break;
		}

		dist2 = dist1;
		dist1 = dist;
		max_point = points[i];
	}
	free(points);
	return;
}
/* End of function find_fingers() */


/* 
Complicated function for finding fingers, using calculations for angle between points from the contour
*/
void find_fingers2(struct HandStructure *HandStructure)
{
	
	int n;
	int i;
	int i_koetoDaBaram = 0;
	
	CvPoint *points_left = NULL, *points_right = NULL;
	CvPoint max_point_left, max_point_right;
	int dist1 = 0, dist2 = 0;
	double finger_distance_left[100];
	double finger_distance_right[100];
	int FingerovBroqch = 0;
	HandStructure->num_fingers_left = 0;
	HandStructure->num_fingers_right = 0;

	//HandStructure->fingers_left = NULL;
	//HandStructure->fingers_right = NULL;
	int cx = HandStructure->hand_center_left.x;
	int cy = HandStructure->hand_center_left.y;

	int countFingers = 0;
	CvPoint *fingersleft = (CvPoint*)calloc(500, sizeof(CvPoint));
	CvPoint *fingersRight = (CvPoint*)calloc(500, sizeof(CvPoint));

	if (!HandStructure->contour_left_forFingers )
		return;

	// Process the left hand
	n = HandStructure->contour_left_forFingers->total;
	if( n == 0 ) return;
	points_left = (CvPoint*)calloc(n, sizeof(CvPoint));

	/* convert contour to array, so that each element can be accessed easily */
	cvCvtSeqToArray(HandStructure->contour_left_forFingers, points_left, CV_WHOLE_SEQ);
	/*
	 * ///Fingers are detected as points where the distance to the center
	 * ///is a local maximum
	 */
	
	for (i = 0; i < n; i++) 
	{
		double dist_strana1, dist_strana2, dist_strana3;
		int position1, position2;
		int side_x = 0,  side_y = 0;
		float cosAlfa = 0;
		float ygyl_Alfa = 0;
		
		/* computing the first side of the triangle */
		position1 = i - STEP_CALC_FINGER_DETECTION;
		if(i < STEP_CALC_FINGER_DETECTION) { position1 = n-1 + i - STEP_CALC_FINGER_DETECTION;  }
		if(position1 >=n || position1 < 0) continue;

		if(points_left[position1].x < 0 || points_left[position1].y < 0) continue;
		side_x = abs(points_left[i].x - points_left[position1].x);
		side_y = abs(points_left[i].y - points_left[position1].y);
		dist_strana1 = sqrt((long double)(pow((long double)side_x,2) + pow((long double)side_y,2)));

		/* computing the second side of the triangle */
		position2 = i + STEP_CALC_FINGER_DETECTION;
		if(i + STEP_CALC_FINGER_DETECTION >= n) { position2 = STEP_CALC_FINGER_DETECTION +1 - n + i; }
		
		if(position2 >=n || position2<0) continue;
		if(points_left[position2].x < 0 || points_left[position2].y < 0) continue;
		side_x = abs(points_left[i].x - points_left[position2].x);
		side_y = abs(points_left[i].y - points_left[position2].y);
		dist_strana2 = sqrt((long double)(pow((long double)side_x,2) + pow((long double)side_y,2)));

		/* computing the third side of the triangle */
		side_x = abs(points_left[position1].x - points_left[position2].x);
		side_y = abs(points_left[position1].y - points_left[position2].y);
		dist_strana3 = sqrt((long double)(pow((long double)side_x,2) + pow((long double)side_y,2)));
		
		/* Cosinus Theorem for finding the angle at the vertex of the triangle in point 'i' */ 
		cosAlfa = (pow(dist_strana1,2) + pow(dist_strana2,2) - pow(dist_strana3,2))/(2*dist_strana1*dist_strana2);
		ygyl_Alfa = acos(cosAlfa)*180/3.1415927;

		/* The angle compared with alfa should be determined with Euristics */
		if(ygyl_Alfa < 80 )
		{
			if(!(points_left[i].x <= 0))
			{
				if(!(points_left[i].y) <= 0)
				{
					fingersleft[countFingers++] = points_left[i];
					i+=STEP_DETECT_NEXT_FINGER;
				}
			}
		} 
	}

	int side_x, side_y;
	double dist;
	
	for(i = 0; i <100; i++)
	{
		finger_distance_left[i] = 0;
	}

	for(i = 0; i < countFingers; i++)
	{
		/* computing a distance between each point in array to the center. We are interested in the first five (biggest) results */
		side_x = abs(cx - fingersleft[i].x); 
		side_y = abs(cy - fingersleft[i].y);
		dist = sqrt((long double)(pow((long double)side_x,2),pow((long double)side_y,2)));
		finger_distance_left[i] = dist;
	}
	if(countFingers!=0) qsort(finger_distance_left,countFingers, sizeof(finger_distance_left[0]), cmp);

	for(i = 0; i < countFingers; i++){
		side_x = abs(cx - fingersleft[i].x);
		side_y = abs(cy - fingersleft[i].y); 
		dist = sqrt((long double)(pow((long double)side_x,2),pow((long double)side_y,2)));
		
		if(dist == finger_distance_left[0]) { HandStructure->fingers_left[0] = fingersleft[i]; FingerovBroqch++; }
			if(dist == finger_distance_left[1]){ HandStructure->fingers_left[1] = fingersleft[i]; FingerovBroqch++; }  
			if(dist == finger_distance_left[2]){ HandStructure->fingers_left[2] = fingersleft[i]; FingerovBroqch++; }  
			if(dist == finger_distance_left[3]){ HandStructure->fingers_left[3] = fingersleft[i]; FingerovBroqch++; }  
			if(dist == finger_distance_left[4]){ HandStructure->fingers_left[4] = fingersleft[i]; FingerovBroqch++; }  
	}
	if(points_left!=NULL) free(points_left);

	HandStructure->num_fingers_left = FingerovBroqch;
	/* End of Left hand processing */

	if (!HandStructure->contour_right_forFingers)
		return;	
	n = 0;
	i = 0;
	dist1 = 0;
	dist2 = 0;

	/* Process the right hand */
	n = HandStructure->contour_right_forFingers->total;

	cx = HandStructure->hand_center_right.x;
	cy = HandStructure->hand_center_right.y;
	points_right = (CvPoint*)calloc(n, sizeof(CvPoint));

	/* convert contour to array, so that each element can be accessed easily */
	cvCvtSeqToArray(HandStructure->contour_right_forFingers, points_right, CV_WHOLE_SEQ);
	countFingers = 0;
	FingerovBroqch = 0;
	for (i = 0; i < n; i++)
	{
		double dist_strana1 = 0, dist_strana2 = 0, dist_strana3 = 0;
		int position1, position2;
		int side_x=0,  side_y=0;
		float cosAlfa=0;
		float ygyl_Alfa=0;
		
		/* computing the first side of the triangle */
		position1 = i - STEP_CALC_FINGER_DETECTION;
		if(i < STEP_CALC_FINGER_DETECTION) position1 = n-1 + i - STEP_CALC_FINGER_DETECTION;
		if(position1 >=n || position1 <0) continue;
		if(points_right[position1].x < 0 || points_right[position1].y < 0) continue;
		side_x = abs(points_right[i].x - points_right[position1].x);
		if(side_x < 0) continue;

		side_y = abs(points_right[i].y - points_right[position1].y);
		if(side_y < 0) continue;
		dist_strana1 = sqrt((long double)(pow((long double)side_x,2) + pow((long double)side_y,2)));
		if(dist_strana1 <= 0) continue;

		/* computing the second side of the triangle */
		position2 = i + STEP_CALC_FINGER_DETECTION;
		if(i + STEP_CALC_FINGER_DETECTION >= n) position2 = STEP_CALC_FINGER_DETECTION - n + i+1;
		
		if(position2 >=n || position2 <0) continue;
		if(points_right[position2].x < 0 || points_right[position2].y < 0) continue;
		side_x = abs(points_right[i].x - points_right[position2].x);
		if(side_x < 0) continue;
		
		side_y = abs(points_right[i].y - points_right[position2].y);
		if(side_y < 0) continue;
		dist_strana2 = sqrt((long double)(pow((long double)side_x,2) + pow((long double)side_y,2)));
		if(dist_strana2 <= 0) continue;

		/* computing the third side of the triangle */
		side_x = abs(points_right[position1].x - points_right[position2].x);
		if(side_x < 0) continue;
		
		side_y = abs(points_right[position1].y - points_right[position2].y);
		if(side_y < 0) continue;
		dist_strana3 = sqrt((long double)(pow((long double)side_x,2) + pow((long double)side_y,2)));
		if(dist_strana3 <= 0) continue;
		
		/* Cosinus Theorem for finding the angle at the vertex of the triangle in point 'i' */ 
		cosAlfa = (pow(dist_strana1,2) + pow(dist_strana2,2) - pow(dist_strana3,2))/(2*dist_strana1*dist_strana2);
		ygyl_Alfa = acos(cosAlfa)*180/3.1415927;

		/* The angle compared with alfa should be determined with Euristics */
		if(ygyl_Alfa < 80 )
		{
			if(!(points_right[i].x <= 0))
			{
				if(!(points_right[i].y) <= 0)
				{
					fingersRight[countFingers++] = points_right[i];
					i+=STEP_DETECT_NEXT_FINGER;
				}
			}
		}
	}
	
	for(i = 0; i <100; i++)
	{
		finger_distance_right[i] = 0;
	}
	for(i = 0; i < countFingers; i++)
	{
		/* computing a distance between each point in array to the center. We are interested in the first five (biggest) results */
		side_x = abs(cx - fingersRight[i].x);
		side_y = abs(cy - fingersRight[i].y);
		dist = sqrt((long double)(pow((long double)side_x,2),pow((long double)side_y,2)));
		finger_distance_right[i] = dist;
	}
	if(countFingers!=0) qsort(finger_distance_right,countFingers, sizeof(finger_distance_right[0]), cmp);

	for(i = 0; i < countFingers; i++)
	{
		side_x = abs(cx - fingersRight[i].x);
		side_y = abs(cy - fingersRight[i].y);
		dist = sqrt((long double)(pow((long double)side_x,2),pow((long double)side_y,2)));
		if(dist == finger_distance_right[0]) { HandStructure->fingers_right[0] = fingersRight[i]; FingerovBroqch++;}
			if(dist == finger_distance_right[1]) { HandStructure->fingers_right[1] = fingersRight[i]; FingerovBroqch++;}
			if(dist == finger_distance_right[2]) { HandStructure->fingers_right[2] = fingersRight[i]; FingerovBroqch++;}
			if(dist == finger_distance_right[3]) { HandStructure->fingers_right[3] = fingersRight[i]; FingerovBroqch++;}
			if(dist == finger_distance_right[4]) { HandStructure->fingers_right[4] = fingersRight[i]; FingerovBroqch++;}
	}
	HandStructure->num_fingers_right = FingerovBroqch;

	if(points_right!=NULL) free(points_right);
	/* end of processing right hand */

	return;
}
/* End of function find_fingers2() */

