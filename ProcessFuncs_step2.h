/*****************************************************************************************
*																						 *
*	Author		  : Petar Bijev											                 *
*	year		  : 2013												                 *
*	Project title : VirtualGuitar												         *
*																				         *
*																	                     *
*	ProcessFuncs_step2.h																 *
*																						 *
*	This file contain declaration of functions, used to process the depth/color frames.  *
******************************************************************************************/


int isPoint_insideHandContour(CvPoint *, CvPoint2D32f, int);
int ConvertFromIJ(int height, int width);
void ConvertToIJ(int PixelPosition,int *height, int *width);
float rawDepthToMeters(int depthValue);
int cmp(const void *a, const void *b);
int cmp_increase (const void *a, const void *b);
bool isSkin(const cv::Scalar& color);