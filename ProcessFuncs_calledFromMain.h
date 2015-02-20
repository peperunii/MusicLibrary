/*****************************************************************************************
*																						 *
*	Author		  : Petar Bijev											                 *
*	year		  : 2013												                 *
*	Project title : VirtualGuitar												         *
*																				         *
*																	                     *
*	ProcessFuncs_calledFromMain.h														 *
*																						 *
*	This file contain declaration of functions, used to process the depth/color frames.  *
******************************************************************************************/


bool DebugLayerCheck(int);
void AreHandsInSquares(struct HandStructure *HandStructure, cv::Mat *OutputImage2, int *recognized_hands);
int FindMinDepthValue(openni::DepthPixel* pDepth);
bool skinColorDetect(int PixelPosition, openni::RGB888Pixel* pColor);
void convertCoordinatesDepthToColor(int height, int width, int depthValue, int *x, int *y);
void init_HandStructure(struct HandStructure *HandStructure);
void filter_and_threshold(struct HandStructure *HandStructure);
void find_contour(struct HandStructure *HandStructure);
void find_convex_hull(struct HandStructure *HandStructure);
void find_convex_hull2(struct HandStructure *HandStructure);
void find_fingers(struct HandStructure *HandStructure);
void find_fingers2(struct HandStructure *HandStructure);

