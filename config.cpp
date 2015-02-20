/*****************************************************************************************
*																						 *
*	Author		  : Petar Bijev											                 *
*	year		  : 2013												                 *
*	Project title : VirtualGuitar												         *
*																				         *
*																	                     *
*	config.cpp																		     *
*																						 *
*	This file contain method which read and parse the config file		                 *
******************************************************************************************/

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"




/*
This function read config file line by line and assign values to the parameters used in all functions. 
*/
/* Called by MAIN function only*/
void ReadConfigFile(void){

	

	FILE *Conf;
	int fileReadCheck;
	char unused_ReadString[255];
	Conf = fopen("conf.txt","rt");
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&DEBUG_INFO_MODE);
	if(!fileReadCheck) {printf("There was an error reading config file for value DEBUG_INFO_MODE\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&DEBUG_LAYER);
	if(!fileReadCheck) {printf("There was an error reading config file for value DEBUG_LAYER\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&DEBUG_IN_FILE);
	if(!fileReadCheck) {printf("There was an error reading config file for value DEBUG_IN_FILE\n"); getch(); }

	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&MAX_OBJECTS_TO_DETECT);
	if(!fileReadCheck) {printf("There was an error reading config file for value MAX_OBJECTS_TO_DETECT\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&HAND_DEPTH_SM);
	if(!fileReadCheck) {printf("There was an error reading config file for value HAND_DEPTH_SM\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&DEPTH_TOLERANCE);
	if(!fileReadCheck) {printf("There was an error reading config file for value skin_COLOR_ALGO_SIMPLE\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&SKIN_COLOR_ALGO_SIMPLE);
	if(!fileReadCheck) {printf("There was an error reading config file for value skin_COLOR_ALGO_SIMPLE\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&SKIN_DETECT_SIMPLE_R);
	if(!fileReadCheck) {printf("There was an error reading config file for value skin_DETECT_SIMPLE_R\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&SKIN_DETECT_SIMPLE_G);
	if(!fileReadCheck) {printf("There was an error reading config file for value skin_DETECT_SIMPLE_G\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&SKIN_DETECT_SIMPLE_B);
	if(!fileReadCheck) {printf("There was an error reading config file for value skin_DETECT_SIMPLE_B\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&ROI_REMOVE_TOP);
	if(!fileReadCheck) {printf("There was an error reading config file for value ROI_REMOVE_TOP\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&ROI_REMOVE_BOTTOM);
	if(!fileReadCheck) {printf("There was an error reading config file for value ROI_REMOVE_BOTTOM\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&STEP_CALC_FINGER_DETECTION);
	if(!fileReadCheck) {printf("There was an error reading config file for value STEP_CALC_FINGER_DETECTION\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&STEP_DETECT_NEXT_FINGER);
	if(!fileReadCheck) {printf("There was an error reading config file for value STEP_DETECT_NEXT_FINGER\n"); getch(); }

	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&FINGER_DETECTION_ALGO_VER);
	if(!fileReadCheck) {printf("There was an error reading config file for value FINGER_DETECTION_ALGO_VER\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&CENTER_PALM_DETECTION_ALGO_VER);
	if(!fileReadCheck) {printf("There was an error reading config file for value CENTER_PALM_DETECTION_ALGO_VER\n"); getch(); }

	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&CONTOUR_SIMPLE);
	if(!fileReadCheck) {printf("There was an error reading config file for value CONTOUR_SIMPLE\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&FILTERING_SIMPLE);
	if(!fileReadCheck) {printf("There was an error reading config file for value FILTERING_SIMPLE\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&DISPLAY_FINGERS);
	if(!fileReadCheck) {printf("There was an error reading config file for value DISPLAY_FINGERS\n"); getch(); }

	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&DISPLAY_CONTOURS);
	if(!fileReadCheck) {printf("There was an error reading config file for value DISPLAY_CONTOURS\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&DISPLAY_HAND_CENTER);
	if(!fileReadCheck) {printf("There was an error reading config file for value DISPLAY_HAND_CENTER\n"); getch(); }
	
	fileReadCheck = fscanf(Conf,"%s %d",unused_ReadString,&DISPLAY_FPS);
	if(!fileReadCheck) {printf("There was an error reading config file for value DISPLAY_FPS\n"); getch(); }
	
	return;
}
/* End of function "ReadConfigFile()" */