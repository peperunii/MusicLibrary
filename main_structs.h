/*****************************************************************************************
*																						 *
*	Author		  : Petar Bijev											                 *
*	year		  : 2013												                 *
*	Project title : VirtualGuitar												         *
*																				         *
*																	                     *
*	main_structs.h																		 *
*																					     *
*	This file contain the main struct used to store information for each hand            *
******************************************************************************************/

/*
This structure defines the main properties of the hands
 - contour
 - hand center
 - fingers
*/
typedef struct HandStructure {

	IplImage	*image;						/* Input image */
	IplImage	*output_image;				/* Input image */
	IplImage	*thr_image;					/* After filtering and thresholding */
	IplImage	*temp_image1;				/* Temporary image (1 channel) */
	IplImage	*temp_image3;				/* Temporary image (3 channels) */

	CvSeq		*contour_left;				/* Hand_left contour */
	CvSeq		*contour_left_forFingers;	/* Hand_left contour */
	CvSeq		*hull_left;					/* Hand_left convex hull */
	CvSeq		*contour_right;				/* Hand_Right contour */
	CvSeq		*contour_right_forFingers;	/* Hand_Right contour */
	CvSeq		*hull_right;				/* Hand_Right convex hull */

	CvPoint		hand_center_left;
	CvPoint		hand_center_right;
	CvPoint		*fingers_left;				/* Detected fingers positions */
	CvPoint		*fingers_right;				/* Detected fingers positions */
	CvPoint		*defects_left;				/* Convexity defects depth points */
	CvPoint		*defects_right;				/* Convexity defects depth points */

	CvMemStorage	*hull_st_left;
	CvMemStorage	*contour_st_left;
	CvMemStorage	*hull_st_right;
	CvMemStorage	*contour_st_right;
	CvMemStorage	*temp_st;
	CvMemStorage	*defects_st_left;
	CvMemStorage	*defects_st_right;

	IplConvKernel	*kernel;				/* Kernel for morph operations */

	int		num_fingers_left;
	int		hand_radius_left;
	int		num_defects_left;
	int		num_fingers_right;
	int		hand_radius_right;
	int		num_defects_right;
	int		Initialization_mode;
	bool    IsLeftHand_left;
	bool	RecognizedChord;				/* information flag if there is recognized chord position */

}HandStructure_T;
/* End of HandStruct_s */

/*
Struct contain information of the chord, which is ready to be recognized
Left Hand structure
*/
struct Chord_S{

	CvPoint FingerPosition_Index;
	CvPoint FingerPosition_Middle;
	CvPoint FingerPosition_Ring;
	CvPoint FingerPosition_Pinky;
 
	int FretPosition;
	bool IsCapo;
	int CapoFret

}Chords_T;
/*End of Chord_S */


/*
Struct containing information about right hand speed and position
Right Hand structure
*/
struct RightHand_S{
	
	bool isPlayingMode;
	int stringPlayed_LastFrame;
	int stringPlayed_CurrentFrame;
	
}RightHand_T;
/* End of RightHand_S */






//
//typedef struct Coordinates_S{
//
//	/*
//	int x;
//	int y;
//	*/
//	CvPoint Coordinate;
//
//}Coordinates_T;
//
////////////////////////
//typedef struct HandPosition_S{
//
//	Coordinates_T Palm;
//	Coordinates_T Thumb;
//	Coordinates_T Index;
//	Coordinates_T Middle;
//	Coordinates_T Ring;
//	Coordinates_T Pinky;
//
//}HandPosition_T;
//
/////////////////////////
//typedef struct HandProperties_S{
//
//	bool initialPositionDetected;
//	
//
//}HandProperties_T;
//
////////////////////
//typedef struct Hand_S{
//
//	HandProperties_T Properties;
//	HandPosition_T Position;
//	
//
//}Hand_T;
//

