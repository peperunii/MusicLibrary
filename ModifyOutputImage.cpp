/*****************************************************************************************
*																						 *
*	Author		  : Petar Bijev											                 *
*	year		  : 2013												                 *
*	Project title : VirtualGuitar												         *
*																				         *
*																	                     *
*	ModifyOuputImage.cpp														 *
*																						 *
*	This file contain functions, used to process the depth/color frames.				 *
*	The List of functions defined in this file (in order):								 *
*																						 *
*		- removeTOP																		 *
*		- removeBOTTOM																	 *
*		- CreateOuputImage																 *
*		- ProcessMIddlePartOfImage_findSkinPixels										 *
*		- FlashSquares_InitMode							di								 *
*		- FlashSquares_InitMode															 *
*		- ProcessGuitarImage															 *
*		- RotateGuitarImage																 *
*		- AddOverlayOutputImage															 *
*		- display																		 *
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
#include "ProcessFuncs_step2.h"
#include "config.h"
#include "main_structs.h"

#define NUM_FINGERS	7
#define NUM_DEFECTS	20
#define SHOW_HAND_CONTOUR 1



/*
This function remove some lines(defined in config file, from the top of the image 
*/
void remove_TOP(cv::Mat *OutputImage)
{
	int width, height;
	int PixCounter;
	for(PixCounter = 0; PixCounter < 640*ROI_REMOVE_TOP; PixCounter++)
	{
		ConvertToIJ(PixCounter,&height,&width);
		OutputImage.at<cv::Vec3b>(height,width)[0] = 0;
		OutputImage.at<cv::Vec3b>(height,width)[1] = 0;
		OutputImage.at<cv::Vec3b>(height,width)[2] = 0;
	}

	return;
}
/* End of function remove_TOP() */



/*
This function remove some lines(defined in config file, from the top of the image 
*/
void remove_BOTTOM(cv::Mat *OutputImage)
{
	int width, height;
	int PixCounter;
	for(PixCounter = (480-ROI_REMOVE_BOTTOM)*640; PixCounter < (640*480); PixCounter++)
	{
		ConvertToIJ(PixCounter,&height,&width);
		OutputImage.at<cv::Vec3b>(height,width)[0] = 0;
		OutputImage.at<cv::Vec3b>(height,width)[1] = 0;
		OutputImage.at<cv::Vec3b>(height,width)[2] = 0;
	}
	return;
}
/* End of function remove_BOTTOM() */



/*
This function process the middle part of the picture (without the TOP and BOTTOM part)
Check every pixel, if is in depth range between lowest depth point and a point defined in Config file and if the pixel 
have skin colored values => copy it to output image. Else write '0'
*/
void ProcessMIddlePartOfImage_findSkinPixels(cv::Mat *OutputImage, int minDepthValue, openni::DepthPixel* pDepth, openni::RGB888Pixel* pColor, FILE* fp_debug )
{
	bool skinColor;
	int height,width;
	int PixCounter = 0;
	int depthCalcHeight, depthCalcWidth;
	int NumberOfHandsPixels = 0;

	for(PixCounter = 640*ROI_REMOVE_TOP; PixCounter < 640*(480 - ROI_REMOVE_BOTTOM); PixCounter++)
	{			
		skinColor = false;
		/* skin color detect for every pixel in the range defined in the conf file */

		ConvertToIJ(PixCounter,&height,&width);
		if(pDepth[PixCounter]!=0) 
		{
			if((pDepth[PixCounter] >= (minDepthValue-DEPTH_TOLERANCE)) && (pDepth[PixCounter] <= (minDepthValue + HAND_DEPTH_SM*10)))
			{
				if(DEBUG_INFO_MODE && (DebugLayerCheck(4))) fprintf(fp_debug,"Process pixels in the Specific Depth range: %d - %d \n",minDepthValue,minDepthValue + HAND_DEPTH_SM*10);
				depthCalcWidth = 0;
				depthCalcHeight = 0;
							
				/* convert the coordinates from depth space to color space */
				convertCoordinatesDepthToColor(height, width, pDepth[PixCounter], &depthCalcHeight, &depthCalcWidth);
							
				/* check if the pixel with new color coordinates is skin colored */
				skinColor = skinColorDetect(depthCalcHeight*640 + depthCalcWidth, pColor);
				if(DEBUG_INFO_MODE && (DebugLayerCheck(4))) fprintf(fp_debug,"Save pixels which corresponding to skin color( everything else is set to 0\n");
							
				/* if the pixel is with skin color, copy RGB channels to the output image */
				if( skinColor == true )
				{
					OutputImage.at<cv::Vec3b>(height,width)[0] = pColor[depthCalcHeight*640+depthCalcWidth].b;  //80 , 100, 180
					OutputImage.at<cv::Vec3b>(height,width)[1] = pColor[depthCalcHeight*640+depthCalcWidth].g;
					OutputImage.at<cv::Vec3b>(height,width)[2] = pColor[depthCalcHeight*640+depthCalcWidth].r;

					NumberOfHandsPixels++;
				}
				else
				{
					OutputImage.at<cv::Vec3b>(height,width)[0] = 0;
					OutputImage.at<cv::Vec3b>(height,width)[1] = 0;
					OutputImage.at<cv::Vec3b>(height,width)[2] = 0;
				}
						
			}
			else
			{
				OutputImage.at<cv::Vec3b>(height,width)[0] = 0;
				OutputImage.at<cv::Vec3b>(height,width)[1] = 0;
				OutputImage.at<cv::Vec3b>(height,width)[2] = 0;
			}

		}
		else
		{
			OutputImage.at<cv::Vec3b>(height,width)[0] = 0;
			OutputImage.at<cv::Vec3b>(height,width)[1] = 0;
			OutputImage.at<cv::Vec3b>(height,width)[2] = 0;
		}
	}
	return;
}
/* End of function ProcessMIddlePartOfImage_findSkinPixels()*/



/*
This function fills the Matrix for the final Image (shown on screen) 
 The idea is to merge guitar image, hands image and overlays images(contours, fingers)
*/
void CreateOuputImage(cv::Mat *OutputImage, cv::Mat *OutputImage2)
{
	for(PixCounter = 0; PixCounter < 640*480; PixCounter++ )
	{
		ConvertToIJ(PixCounter,&height,&width);
		if(OutputImage.at<cv::Vec3b>(height,width)[1] != 0 && OutputImage.at<cv::Vec3b>(height,width)[2] != 0)
		{
		// TVA TRQBVA DA SE OPRAVI(KOMENTIRA)	
			if(HandStructure.Initialization_mode )
			{
				if(OutputImage2.at<cv::Vec3b>(height,width)[0] == 0 && OutputImage2.at<cv::Vec3b>(height,width)[1] == 0 && OutputImage2.at<cv::Vec3b>(height,width)[2] == 0)
				{
					OutputImage2.at<cv::Vec3b>(height,width)[0] = OutputImage.at<cv::Vec3b>(height,width)[0];
					OutputImage2.at<cv::Vec3b>(height,width)[1] = OutputImage.at<cv::Vec3b>(height,width)[1];
					OutputImage2.at<cv::Vec3b>(height,width)[2] = OutputImage.at<cv::Vec3b>(height,width)[2];
					//TVA SYSHTO
				}
			}
			else
			{
				OutputImage2.at<cv::Vec3b>(height,width)[0] = OutputImage.at<cv::Vec3b>(height,width)[0];
				OutputImage2.at<cv::Vec3b>(height,width)[1] = OutputImage.at<cv::Vec3b>(height,width)[1];
				OutputImage2.at<cv::Vec3b>(height,width)[2] = OutputImage.at<cv::Vec3b>(height,width)[2];
			}
		}
	/*	else 
		{
			OutputImage2.at<cv::Vec3b>(height,width)[0] = 255;
			OutputImage2.at<cv::Vec3b>(height,width)[1] = 255;
			OutputImage2.at<cv::Vec3b>(height,width)[2] = 255;
		}
	*/
	}
	/* End of cycle FOR - processing pixels of the ouput image */

	return;
}
/* End of function createOutputImage() */


/* 
The squares will start to flash for several frames if both hands are correctly placed 
*/
void FlashSquares_InitMode(struct HandStructure *HandStructure, cv::Mat *OutputImage2, int InitializationFrameCounter)
{
	if(!HandStructure.Initialization_mode && InitializationFrameCounter > 0)
	{	
		if(InitializationFrameCounter%2 == 0)
		{
			cv::rectangle(OutputImage2, cvPoint(100,200), cvPoint(180,280), cvScalar(0,255,0),CV_FILLED ,8,0);
			cv::rectangle(OutputImage2, cvPoint(400,200), cvPoint(480,280), cvScalar(0,255,0),CV_FILLED ,8,0);
		}
		InitializationFrameCounter--;
	}
	return;
}
/* end of function FlashSquares_InitMode() */



/* 
Processing guitar image. Rotate and scale depending on the position of the hands.
Process only if both hands centers are in the predefined squares (initialization mode ) 
*/
void ProcessGuitarImage(struct HandStructure *HandStructure, cv::Mat *OutputImage2, int minDepthValue, cv::Mat GuitarImage_src, FILE* fp_debug){
	double GuitarAngle=0;
	double GuitarScale=1;
	int distanceX_Hands=0;
	int distanceY_Hands=0;
	cv::Point GuitarCenter;	
	double distanceHipotenuze=0;
	int StartPointX=0,StartPointY=0;

	if(!HandStructure.Initialization_mode)
	{
		if(DEBUG_INFO_MODE && (DebugLayerCheck(4))) fprintf(fp_debug,"Calculating Angle, Scale and Center for the guitar image\n");
					
		distanceX_Hands = abs(HandStructure.hand_center_left.x - HandStructure.hand_center_right.x);
		distanceY_Hands = abs(HandStructure.hand_center_left.y - HandStructure.hand_center_right.y);
		distanceHipotenuze = sqrt(pow((long double)(distanceX_Hands),2)  + pow((long double)(distanceY_Hands),2));
		GuitarAngle = atan((double)distanceY_Hands/distanceX_Hands)*180/PI;
					
		if(HandStructure.hand_center_left.x < HandStructure.hand_center_right.x)
		{
			if(HandStructure.hand_center_left.y < HandStructure.hand_center_right.y) { if (GuitarAngle > 0) GuitarAngle = -GuitarAngle; }
			if(HandStructure.hand_center_left.y > HandStructure.hand_center_right.y) { if (GuitarAngle < 0) GuitarAngle = -GuitarAngle; }
		}
		if(HandStructure.hand_center_left.x > HandStructure.hand_center_right.x)
		{
			if(HandStructure.hand_center_left.y < HandStructure.hand_center_right.y) { if (GuitarAngle < 0) GuitarAngle = -GuitarAngle; }
			if(HandStructure.hand_center_left.y > HandStructure.hand_center_right.y) { if (GuitarAngle > 0) GuitarAngle = -GuitarAngle; }
		}
					
		/* compute the scale of the guitar image */
		/* ver1 */
		//GuitarScale = (double)distanceHipotenuze/380; //380 is the approixmately size of the guitar in the default image.
					
		/* ver2 */
		GuitarScale = 1.5;
			
		if( minDepthValue >=400 && minDepthValue < 900)
		{
			GuitarScale = GuitarScale - ((double)abs( minDepthValue - 400 )/1000);
		}
		else 
		{
			GuitarScale = GuitarScale - ((double)abs( minDepthValue - 900 )/3100);
		}
		StartPointX = min(HandStructure.hand_center_left.x, HandStructure.hand_center_right.x);
		if(HandStructure.hand_center_left.x == StartPointX) 
		{
			StartPointY = HandStructure.hand_center_left.y;
		}
		else 
		{
			StartPointY = HandStructure.hand_center_right.y;
		}
		//StartPointY = min(HandStructure.hand_center_left.y, HandStructure.hand_center_right.y);
		GuitarCenter = cv::Point(StartPointX,StartPointY); // + distanceX_Hands/2 ,StartPointY + distanceY_Hands/2);
		if(DEBUG_INFO_MODE && (DebugLayerCheck(4))) fprintf(fp_debug,"Guitar Parameters: \n1: letf_hand_center_x: %d: \n2: letf_hand_center_y: %d\n3: right_hand_center_x: %d\n4: right_hand_center_y %d\n5: distanceX: %d\n6: distanceY: %d\n7: distanceHipotenuze: %lf\n8:Angle: %lf\n9: StartPointX: %d\n10: StartPointY: %d", HandStructure.hand_center_left.x,HandStructure.hand_center_left.y,HandStructure.hand_center_right.x,HandStructure.hand_center_right.y, distanceX_Hands,distanceY_Hands, distanceHipotenuze, GuitarAngle,StartPointX, StartPointY);
				
		/* Rotate and scale guitar image with respect to Guitar parameters */
		if(DEBUG_INFO_MODE && (DebugLayerCheck(3))) fprintf(fp_debug,"Process guitar image with the current guitar parameters\n");
		RotateGuitarImage(GuitarImage_src, &GuitarImage_dst, GuitarAngle, GuitarScale, GuitarCenter, &HandStructure );
				
		/* Add guitar as a background for the current processed output image (hands) */
		if(DEBUG_INFO_MODE && (DebugLayerCheck(4))) fprintf(fp_debug,"Add Guitar Image over the hands image\n");
		AddOverlayOutputImage(&OutputImage2, GuitarImage_dst);
	}
	
	return;
}
/* End of function ProcessGuitarImage() */




/*
This function Rotate the guitar input image, so that it can be displayed according the position of the hands.
*/
void RotateGuitarImage(cv::Mat src, cv::Mat* dst, double angle, double scale, cv::Point center, struct HandStructure *HandStructure){

  cv:: Mat rot_mat( 2, 3, CV_32FC1 );
  cv::Mat warp_dst;

  warp_dst = cv::Mat::zeros( src.rows, src.cols, src.type() );
  // Compute a rotation matrix with respect to the center of the image
  // cv::Point center = cv::Point( warp_dst.cols/2, warp_dst.rows/2 );

  // Get the rotation matrix with the specifications above
  if( HandStructure->RecognizedChord == true ) angle = 0;
  rot_mat = cv::getRotationMatrix2D( center, angle, scale );

  // Rotate the warped image
  warpAffine( src, *dst, rot_mat, warp_dst.size() );

  return;
}
/* End of function RotateGuitarImage() */


/*
This function add some featurres to the output image, like guitar 
*/
void AddOverlayOutputImage(cv::Mat *OutputImage2, cv::Mat GuitarImage){

	int Index;
	int width, height;

	for(Index = 0; Index < 640 * 480; Index++)
	{
		ConvertToIJ(Index, &height, &width);
		if(GuitarImage.at<cv::Vec3b>(height,width)[0] != 0)
		{
			OutputImage2->at<cv::Vec3b>(height,width)[0] = GuitarImage.at<cv::Vec3b>(height,width)[0];
			OutputImage2->at<cv::Vec3b>(height,width)[1] = GuitarImage.at<cv::Vec3b>(height,width)[1];
			OutputImage2->at<cv::Vec3b>(height,width)[2] = GuitarImage.at<cv::Vec3b>(height,width)[2];
		}
	}

	return;
}
/* End of function AddOverlayOutputImage() */


/* 
This function write on the screen creating a cvFont.
*/

void WriteOnScreen(struct HandStructure *HandStructure, char* tekst, cvPoint position, cvScalar color)
{
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4, 0, 1, 8);
	double hScale=1.0;
	double vScale=1.0;
	int    lineWidth=1;
	
	cvPutText (HandStructure->image,tekst,position, &font, color);
	
    return;
}

/* */



/*
This function display Contours, convex hull, detected fingers and center of the palm for each hand over the output image
*/
void display(struct HandStructure *HandStructure, int frCounter)
{
int i;
/* Display FPS in the upper right corner */
if(DISPLAY_FPS)
{
	char spri[15];
	
	sprintf(spri,"Frame number_%d",frCounter);
	WriteOnScreen(HandStructure, spri, cvPoint(50,50), cvScalar(255,255,0));
	
}

	if ((HandStructure->hand_center_left.x != 0 ) )// && (HandStructure->contour_left!=NULL)){
	{
		if(DISPLAY_CONTOURS)
			cvDrawContours(HandStructure->image, HandStructure->contour_left_forFingers, CV_RGB(0,0,255), CV_RGB(0,255,0), 0, 1, CV_AA, cvPoint(0,0));
		if(DISPLAY_HAND_CENTER)
		{
			cvCircle(HandStructure->image, HandStructure->hand_center_left, 5, CV_RGB(255, 0, 255), 1, CV_AA, 0);
			cvCircle(HandStructure->image, HandStructure->hand_center_left, HandStructure->hand_radius_left, CV_RGB(255, 0, 0), 1, CV_AA, 0);
		}
		if(DISPLAY_FINGERS)
		{
			for (i = 0; i < HandStructure->num_fingers_left; i++) 
			{
				cvCircle(HandStructure->image, HandStructure->fingers_left[i], 10, CV_RGB(0, 255, 0), 3, CV_AA, 0);
				if(DISPLAY_HAND_CENTER) cvLine(HandStructure->image, HandStructure->hand_center_left, HandStructure->fingers_left[i], CV_RGB(255,255,0), 1, CV_AA, 0);
			}
		}
	/*	for (i = 0; i < HandStructure->num_defects_left; i++) {
			cvCircle(HandStructure->image, HandStructure->defects_left[i], 2,
				 CV_RGB(200, 200, 200), 2, CV_AA, 0);
		}
	*/
	}

	if ((HandStructure->hand_center_right.x != 0 )  )//&& (HandStructure->contour_right!=NULL) ) {
	{ 
		if(DISPLAY_CONTOURS) 
			cvDrawContours(HandStructure->image, HandStructure->contour_right, CV_RGB(0,0,255), CV_RGB(0,255,0), 0, 1, CV_AA, cvPoint(0,0));
		if(DISPLAY_HAND_CENTER)
		{
			cvCircle(HandStructure->image, HandStructure->hand_center_right, 5, CV_RGB(255, 0, 255), 1, CV_AA, 0);
			cvCircle(HandStructure->image, HandStructure->hand_center_right, HandStructure->hand_radius_right, CV_RGB(255, 0, 0), 1, CV_AA, 0);
		}
		if(DISPLAY_FINGERS)
		{
			for (i = 0; i < HandStructure->num_fingers_right; i++) 
			{
				cvCircle(HandStructure->image, HandStructure->fingers_right[i], 10, CV_RGB(0, 255, 0), 3, CV_AA, 0);
				if(DISPLAY_HAND_CENTER) cvLine(HandStructure->image, HandStructure->hand_center_right, HandStructure->fingers_right[i], CV_RGB(255,255,0), 1, CV_AA, 0);
			}
		}
	/*	for (i = 0; i < HandStructure->num_defects_right; i++)
		{
			cvCircle(HandStructure->image, HandStructure->defects_right[i], 2,
				 CV_RGB(200, 200, 200), 2, CV_AA, 0);
		}
	*/
	}
	cvShowImage("output", HandStructure->image);

	return;
}
/* End of function display() */




/*
This function prints DEBUG messages  - izlishna funkciq
*/
/*
void PrintDebug(int printNumber, FILE *fp)
{

    char DebugMessages[100][200] = {
								"Reading Config file: \n",
								".....................DONE\n",
								"Config file: \n"
								//" - DEBUG_INFO_MODE: %d\nDEBUG_LAYER: %d\nMAX_OBJECTS_TO_DETECT: %d\nv4: %d\nV5: %d\nv6: %d\nv7: %d\nv8: %d\nv9: %d \nv10: %d\nv11: %d\n", DEBUG_INFO_MODE, DEBUG_LAYER, MAX_OBJECTS_TO_DETECT, HAND_DEPTH_SM, DEPTH_TOLERANCE, SKIN_COLOR_ALGO_SIMPLE,SKIN_DETECT_SIMPLE_R, SKIN_DETECT_SIMPLE_G, SKIN_DETECT_SIMPLE_B, ROI_REMOVE_TOP, ROI_REMOVE_BOTTOM),
								"Initializing OpenNI\n"

								};
	

	fputs(DebugMessages[printNumber],fp);//fprintf(fp,DebugMessages[printNumber]);

	return;
}
*/
/*End of function PrintDebug */
