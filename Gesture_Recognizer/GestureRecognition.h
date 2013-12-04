#pragma once
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/nonfree/features2d.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/ml/ml.hpp"
using namespace cv;
/*
This class implements the gesture recognition method
*/
class GestureRecognition
{
public:
	// constructor
	GestureRecognition(void);
	// constructor, load a classifier
	GestureRecognition(char * classifier_name, char * dictionary_name);
	// recognize one image
	int recognize(IplImage *frame);
	~GestureRecognition(void);
	bool is_active(Mat image, cv::Rect window);
private:
	// classifier
	CvSVM svm_model;
	// feature extractor
	Ptr<DescriptorMatcher> matcher;
	Ptr<FeatureDetector> detector;
	Ptr<DescriptorExtractor> extractor;
	BOWImgDescriptorExtractor *bowDE;
	// dictionary
	Mat dictionary;
};
