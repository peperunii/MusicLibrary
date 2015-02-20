/**********************************************************************************************
*																							  *
*	Author		  : Petar Bijev																  *
*	year		  : 2013																	  *
*	Project title : VirtualGuitar															  *
*																							  *
*																							  *
*	Main idea and function of the project is to detect the hands of the user				  *
*	sitting in front of the Kinect device in the distance between (50 - 200)sm.				  *
*	Based on depth and color segmentation, the algorithm main task is to detect the position  *
*	and movement of the fingers and to identify finger patterns in order to play music.		  *
*																							  *
*	All comments are in /* * / format. The line comments ( // ) are for Debug				  *
*																							  *
***********************************************************************************************/		


#include <OpenNI.h>
#include <PS1080.h>

#include <stdexcept>
#include <opencv2\opencv.hpp>
#include <iostream>
#include <conio.h>
#include <math.h>
#include <ctime>

#include "config.h"
#include "ProcessFuncs_calledFromMain.h"
#include "ModifyOuputImage.h"
#include "main_structs.h"


#include "OniPlatform.h"
#include "OniProperties.h"
#include "OniEnums.h"

#include "OniCAPI.h"
#include "OniCProperties.h"



#define PI 3.14159265

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define LOG(msg) (std::cout<<msg<<std::endl);

/*global variables*/
    int DEBUG_INFO_MODE;
	int DEBUG_LAYER;
	int DEBUG_IN_FILE;
	int MAX_OBJECTS_TO_DETECT;
    int HAND_DEPTH_SM;
	int DEPTH_TOLERANCE;
    int SKIN_COLOR_ALGO_SIMPLE;
    int SKIN_DETECT_SIMPLE_R;
    int SKIN_DETECT_SIMPLE_G;
    int SKIN_DETECT_SIMPLE_B;
	int ROI_REMOVE_TOP;
	int ROI_REMOVE_BOTTOM;
	int STEP_CALC_FINGER_DETECTION;
	int STEP_DETECT_NEXT_FINGER;
	int FINGER_DETECTION_ALGO_VER;
	int CENTER_PALM_DETECTION_ALGO_VER;
	int CONTOUR_SIMPLE;
	int FILTERING_SIMPLE;
	int DISPLAY_FINGERS;
    int DISPLAY_CONTOURS;
    int DISPLAY_HAND_CENTER;
	int DISPLAY_FPS;
/*end of global variables*/



/* 
MAIN function of the program. 
*/

int main()//int argc, char **argv)
{
	using namespace openni;
	int zpd;
    double zpps;
	HandStructure_T HandStructure;
	int numberOfskinColoredPixelsInFrame = 0;

	FILE *fp_debug = NULL;

	if(DEBUG_INFO_MODE) fprintf(fp_debug,"Initializing structure: ");
	HandStructure.image = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
	init_HandStructure(&HandStructure);
	if(DEBUG_INFO_MODE) fprintf(fp_debug,"successfull\n");

	/* 
	First we have to:	-  initialize openNi
						-  to check if there is connected and working Kinect device
						-  to start depth/color streams
	*/

	/* read and parse parameter values from the conig file */
	ReadConfigFile(); 

	fp_debug = fopen("DebugMessages.txt","wt");
	if(fp_debug==NULL) { fprintf(fp_debug,"problem creating debuging file\n"); return 0; }

	if(DEBUG_INFO_MODE) fprintf(fp_debug,"Reading Config file: ");
	if(DEBUG_INFO_MODE) fprintf(fp_debug,"DONE\n");
	if(DEBUG_INFO_MODE && (DebugLayerCheck(3))) { fprintf(fp_debug,"DEBUG_INFO_MODE: %d\nDEBUG_LAYER: %d\nMAX_OBJECTS_TO_DETECT: %d\nv4: %d\nV5: %d\nv6: %d\nv7: %d\nv8: %d\nv9: %d \nv10: %d\nv11: %d\n", DEBUG_INFO_MODE, DEBUG_LAYER, MAX_OBJECTS_TO_DETECT, HAND_DEPTH_SM, DEPTH_TOLERANCE, SKIN_COLOR_ALGO_SIMPLE,SKIN_DETECT_SIMPLE_R, SKIN_DETECT_SIMPLE_G, SKIN_DETECT_SIMPLE_B, ROI_REMOVE_TOP, ROI_REMOVE_BOTTOM); }
	
	/* check the param DEBUG_IN_FILE in the config file */
	if(!DEBUG_IN_FILE)
	{  
		fp_debug = stdout;
	}
	 
	/* OpenNi functionality for starting the camera and depth/color streams */
	try
	{
		if(DEBUG_INFO_MODE) fprintf(fp_debug,"Initializing OpenNI\n");
		
		if(OpenNI::initialize()!=STATUS_OK)
			throw std::runtime_error(std::string("OpenNI2 Initialization Failed: ")+OpenNI::getExtendedError());
		
		Device device;
		VideoStream depth, color;
		
		if (device.open(openni::ANY_DEVICE) != STATUS_OK)
			throw std::runtime_error(std::string("Couldn't open device ")+ OpenNI::getExtendedError());
		
		/* The HandTracker expects registered RGB and Depth images. */
		/* Nice function, doesn't seem to work correctly. That's why convert coordinates from deth to Color is needed */
		if(DEBUG_INFO_MODE) fprintf(fp_debug,"Register depth and color images\n");
		device.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);

		if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
		{
			Status rc = depth.create(device, SENSOR_DEPTH);
			if (rc == STATUS_OK)
			{
				if(depth.start()!= STATUS_OK)
				{
					throw std::runtime_error(std::string("Couldn't start the color stream: ")+ OpenNI::getExtendedError());
				}
			}
			else
			{
				throw std::runtime_error(std::string("Couldn't create depth stream: ")+ OpenNI::getExtendedError());
			}
		}

		if (device.getSensorInfo(SENSOR_COLOR) != NULL)
		{
			Status rc = color.create(device, SENSOR_COLOR);
			if (rc == STATUS_OK)
			{
				if(color.start() != STATUS_OK)
				{
					throw std::runtime_error(std::string("Couldn't start the color stream")+ OpenNI::getExtendedError());
				}
			}
			else
			{
				throw std::runtime_error(std::string("Couldn't create color stream")+ OpenNI::getExtendedError());
			}
		}
		depth.getProperty(XN_STREAM_PROPERTY_ZERO_PLANE_DISTANCE,&zpd);
		depth.getProperty(XN_STREAM_PROPERTY_ZERO_PLANE_PIXEL_SIZE,&zpps);

		/**************************************************************

		
		End of inizialization. Start processing every color/depth frame 
		

		***************************************************************/
		if(DEBUG_INFO_MODE) fprintf(fp_debug,"Start processing every frame\n");
		
		int key = -1;
		int status = -2;
		int middleIndex;
		int PixCounter = 0;
		bool tracking = false;
		bool skinColor = false;
		long NumberOfHandsPixels;
		int recognized_hands = 0;
		int processedFrameCounter = 0;
		int InitializationFrameCounter = 45;
		int depthCalcHeight, depthCalcWidth;
		VideoFrameRef depthFrame,colorFrame;
		VideoStream* streams[] = {	&depth , &color};
		
		cv::Mat GuitarImage_src = cv::Mat(480,640,CV_8UC3);

		GuitarImage_src = cvLoadImage("GuitarForProject2.jpg");
		if(DEBUG_INFO_MODE && (DebugLayerCheck(2))) fprintf(fp_debug,"Guitar image loaded successful\n");
		if(DEBUG_INFO_MODE && (DebugLayerCheck(1))) fprintf(fp_debug,"Device initialized\n depth ZPD: %d\n depth ZPPS: %lf\n", zpd, zpps);

		cv::namedWindow( "VirtualGuitar", CV_WINDOW_AUTOSIZE );		
		HandStructure.Initialization_mode = 1;

		/* cycle WHILE for processing every frame. The program exits if the 'Q'- key is pressed. */
		while((key=(cv::waitKey(33)&0x000000FF))!='q')
		{  
			if(DEBUG_INFO_MODE && (DebugLayerCheck(4))) fprintf(fp_debug,"Process Frame\n");
			NumberOfHandsPixels = 0;
			cv::Mat OutputImage = cv::Mat(480,640,CV_8UC3);
			cv::Mat OutputImage_color = cv::Mat(480,640,CV_8UC3);
			cv::Mat OutputImage2 = cv::Mat(480,640,CV_8UC3);
			cv::Mat GuitarImage_dst = cv::Mat(480,640,CV_8UC3);
			numberOfskinColoredPixelsInFrame = 0;
			cvSet(OutputImage.data, cvScalar(0,0,0));
			depth.readFrame(&depthFrame);
			
			color.readFrame(&colorFrame);	
			
			/* Start processing only if there are available both color and depth frames */
			if(depthFrame.isValid() && colorFrame.isValid())
			{ 
				if(DEBUG_INFO_MODE && (DebugLayerCheck(4))) fprintf(fp_debug,"Both the color/depth frames are valid\n");
				 
				DepthPixel* pDepth;
				RGB888Pixel* pColor;
				double GuitarAngle=0;
				double GuitarScale=1;
				int distanceX_Hands=0;
				int distanceY_Hands=0;
				cv::Point GuitarCenter;
				int minDepthValue = 3000;
				int width = 0, height = 0;
				int CurrentDepthValue = 0;
				double distanceHipotenuze=0;
				int StartPointX=0,StartPointY=0;

				/* capture frame data and store it in pDepth and pColor */
				pDepth = (DepthPixel*)depthFrame.getData();
				pColor = (RGB888Pixel*)colorFrame.getData();
				
				/* Initializing OutputImage2 (the final image); */
				OutputImage2.setTo(cv::Scalar(0,0,0));

				/* Find the lowest depth value */
				if(DEBUG_INFO_MODE && (DebugLayerCheck(2))) fprintf(fp_debug,"The lowest depth value in the image is: ");
				
				minDepthValue = FindMinDepthValue(pDepth)
				
				if(DEBUG_INFO_MODE && (DebugLayerCheck(2))) fprintf(fp_debug,"%d \n",minDepthValue);
				
				/* We are only interested in pixels in ROI defined in config file. (remove some lines from top and bottom) 
				   Bottom part of the image is not processed -  filled with zeros										*/
				remove_TOP(&OutputImage);

				/* Process only pixes in the range between RemoveRoi's defined in config file */
				if(DEBUG_INFO_MODE && (DebugLayerCheck(4))) fprintf(fp_debug,"Process pixels in the Yrange: %d - %d \n",ROI_REMOVE_TOP,640-ROI_REMOVE_BOTTOM);
				
				/* Process middle part of image. 
				   Check every pixel, if is in depth range between lowest depth point and a point defined in Config file and if the pixel have skin colored values */
				ProcessMIddlePartOfImage_findSkinPixels(&OutputImage, minDepthValue, pDepth, pColor. fp_debug);
				
				/* Bottom part of the image is not processed -  filled with zeros */
				remove_BOTTOM(&OutputImage);

				/* Cycle which process the segmented pixels from both hands and initialize structures for the hands. */
			
				/*		// (eng_cast) i tuk nqkakva ideq sym imal.. zasega nema da go triq.
				for(PixCounter = 640*ROI_REMOVE_TOP; PixCounter < 640*480-ROI_REMOVE_BOTTOM*640; PixCounter++)
				{
					if( OutputImage.at<cv::Vec3b>(height,width)[0] != 0 )
					{
						NumberOfHandsPixels--;
					}
					if(!NumberOfHandsPixels)break;
				}
				*/
				
				/*  Filling structure. Initializing image*/
				HandStructure.image->imageData = (char*)OutputImage.data;

				/* Processing guitar image. Rotate and scale depending on the position of the hands.
				   Process only if both hands centers are in the predefined squares (initialization mode ) */
				ProcessGuitarImage(&HandStructure, &OutputImage2, minDepthValue, GuitarImage_src, fp_debug);
			
				/* If the program is in initalization mode, draw two squares in the image frame. To exit from this mode, the user have to place both hands in the squares */
				if(HandStructure.Initialization_mode)
				{
					HandStructure.RecognizedChord = false;
					recognized_hands = 0;
					/* Frames after initialization of the hands in which the green squares will flash*/
					InitializationFrameCounter = 10; 
					cv::rectangle(OutputImage2, cvPoint(100,200), cvPoint(180,280), cvScalar(0,0,255),3 ,8, 0);
					cv::rectangle(OutputImage2, cvPoint(400,200), cvPoint(480,280), cvScalar(0,0,255),3 ,8, 0);
					WriteOnScreen(HandStructure, "Place both of your hands in the squares drawn on the screen. ", cvPoint(300,300), cvScalar(255,255,0));
					WriteOnScreen(HandStructure, "Place first Right Hand and wait for 3 seconds to restart playing initialization.", cvPoint(300,300), cvScalar(255,255,0));
				}

				/* Start filling the HandsStruct, processing only the hand pixels */
				if(DEBUG_INFO_MODE && (DebugLayerCheck(2))) fprintf(fp_debug,"Start Detecting and tracking hands\n");
				
				/* Filter and threshold the image */
				filter_and_threshold(&HandStructure);
				
				/* Find contour of the hands, 
				   The algorithm assume that the hands are infront of the body and there are no other objects bwteen the hands and the camera */
				find_contour(&HandStructure);
				
				/* The first found contour (the biggest one) is assigned to the parameter HandStructure.contour_left, if no left contour is found, the frame is skipped */
				if(!HandStructure.contour_left) continue;
				if(DEBUG_INFO_MODE && (DebugLayerCheck(3)) && HandStructure.contour_left && HandStructure.contour_right) fprintf(fp_debug,"Contours for both hands detected\n");
				
				/* Check the parameters provided in the config file. If an appropriate algorithm is chosen, execute the function */
				if(CENTER_PALM_DETECTION_ALGO_VER == 1)		 find_convex_hull(&HandStructure);  /* This function detect center of the palm (algo_1) */
				else
				{
					if(CENTER_PALM_DETECTION_ALGO_VER == 2)  find_convex_hull2(&HandStructure); /* This function detect center of the palm (algo_2) */
				}
				if(FINGER_DETECTION_ALGO_VER == 1)			 find_fingers(&HandStructure);      /* This function detect fingers based onthe contour (algo_1) */
				else 
				{
					if(FINGER_DETECTION_ALGO_VER == 2)		 find_fingers2(&HandStructure);		/* This function detect fingers based onthe contour (algo_2) */
				}
				
				/* This function add Overlay to the output image. 
					- Hand contours
					- finger tips
					- center of the palm					  */
				display(&HandStructure, processedFrameCounter);

				/* Check if the initialized left hand is actually LEFT hand, by comparing hand center of both hands */
				if (HandStructure.hand_center_left.x > HandStructure.hand_center_right.x) HandStructure.IsLeftHand_left = false;
				else HandStructure.IsLeftHand_left = true;
				
				/* Check if both hands are in the squares. if true => exit initialization mode 
				   Change color of the squares to indicate that the user is correctly placing his hands */
				AreHandsInSquares( &HandStructure, &OutputImage2, &recognized_hands);

				/* The squares will start to flash for several frames if both hands are correctly placed  */
				FlashSquares_InitMode(HandStructure, &OutputImage2, InitializationFrameCounter);
				
				/* Create the final output image */
				CreateOutputImage(&OutputImage2, &OutputImage);

				processedFrameCounter ++;  //  (eng_cast) Veche i ideq si nemam kakva mu beshe ideqta na tva.. ama ne prechi :)

				//imshow("VirtualGuitar2",OutputImage2);
				if(DEBUG_INFO_MODE && (DebugLayerCheck(3))) fprintf(fp_debug,"End of processing the frame ...\n");
			
			}
			/* End of IF - process only if depth and color frames are available */
				
			else
			{
				if(DEBUG_INFO_MODE && (DebugLayerCheck(1))) fprintf(fp_debug,"Frame Not Available\n");
			}
		} 
		/* End of cycle (While) for reading and processing every frame. */
		if(DEBUG_INFO_MODE && (DebugLayerCheck(3))) fprintf(fp_debug,"End of processing ...\n");
				
		/* Release the images and depth/color streams */
		if(DEBUG_INFO_MODE && (DebugLayerCheck(2))) fprintf(fp_debug,"Stopping depth and color streams\n");
		depth.stop();
		color.stop();
		depth.destroy();
		color.destroy();
		device.close();
		if(DEBUG_INFO_MODE && (DebugLayerCheck(1))) fprintf(fp_debug,"Shutting down OpenNI\n");
		OpenNI::shutdown();

		if(DEBUG_INFO_MODE && (DebugLayerCheck(1))) fprintf(fp_debug,"Destroying HandTracker.\n");
		
	}
	catch(std::exception &e)
	{
		LOG("Exception: " << e.what());
		getch();
	}
	catch(...)
	{
		LOG("Unknown Error");
	}
	if(DEBUG_INFO_MODE && (DebugLayerCheck(1))) fprintf(fp_debug,"Shutdown complete\n");
	
	fclose(fp_debug);
	
	printf("Nooo Nooo. Don't shut me down :(...\n");
	cv::waitKey(1100);

	return 0;
}


/* END of MAIN function */