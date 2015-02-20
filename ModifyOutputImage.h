/*****************************************************************************************
*																						 *
*	Author		  : Petar Bijev											                 *
*	year		  : 2013												                 *
*	Project title : VirtualGuitar												         *
*																				         *
*																	                     *
*	ModifyOutputImage.h																	 *
*																						 *
*	This file contain declaration of functions, used to process the ouputImage.		     *
******************************************************************************************/


void CreateOuputImage(cv::Mat *OutputImage, cv::Mat *OutputImage2);
void remove_TOP(cv::Mat *OutputImage);
void remove_BOTTOM(cv::Mat *OutputImage);
void ProcessMIddlePartOfImage_findSkinPixels(cv::Mat *OutputImage, int minDepthValue, openni::DepthPixel* pDepth, openni::RGB888Pixel* pColor, FILE* fp_debug  );
void ProcessGuitarImage(struct HandStructure *HandStructure, cv::Mat *OutputImage2, int minDepthValue, cv::Mat GuitarImage_src, FILE* fp_debug);
void RotateGuitarImage(cv::Mat src, cv::Mat* dst, double angle, double scale, cv::Point center, struct HandStructure *HandStructure);
void AddOverlayOutputImage(cv::Mat *OutputImage, cv::Mat GuitarImage);
void FlashSquares_InitMode(struct HandStructure *HandStructure, cv::Mat *OutputImage, int InitializationFrameCounter);
void display(struct HandStructure *HandStructure, int frCounter);