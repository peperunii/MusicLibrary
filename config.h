/*****************************************************************************************
*																						 *
*	Author		  : Petar Bijev											                 *
*	year		  : 2013												                 *
*	Project title : VirtualGuitar												         *
*																				         *
*																	                     *
*	config.h																		     *
*																						 *
*	This file contain declaration of some variables used for the algorithm.				 *
******************************************************************************************/


/* parameters defined in config file. Visible to all functions */
extern int DEBUG_INFO_MODE;
extern int DEBUG_LAYER;
extern int DEBUG_IN_FILE;
extern int MAX_OBJECTS_TO_DETECT;
extern int HAND_DEPTH_SM;
extern int DEPTH_TOLERANCE;
extern int SKIN_COLOR_ALGO_SIMPLE;
extern int SKIN_DETECT_SIMPLE_R;
extern int SKIN_DETECT_SIMPLE_G;
extern int SKIN_DETECT_SIMPLE_B;
extern int ROI_REMOVE_TOP;
extern int ROI_REMOVE_BOTTOM;
extern int STEP_CALC_FINGER_DETECTION;
extern int STEP_DETECT_NEXT_FINGER;
extern int FINGER_DETECTION_ALGO_VER;
extern int CENTER_PALM_DETECTION_ALGO_VER;
extern int CONTOUR_SIMPLE;
extern int FILTERING_SIMPLE;
extern int DISPLAY_FINGERS;
extern int DISPLAY_CONTOURS;
extern int DISPLAY_HAND_CENTER;
extern int DISPLAY_FPS;

/* end param definition */

void ReadConfigFile(void);
 


