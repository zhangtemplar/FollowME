#include "GestureRecognition.h"

GestureRecognition::GestureRecognition(void)
{
	//create a nearest neighbor matcher
	matcher=Ptr<DescriptorMatcher>(new FlannBasedMatcher);
	//create Sift feature point extracter
	detector=Ptr<FeatureDetector>(new SiftFeatureDetector());
	//create Sift descriptor extractor
	extractor=Ptr<DescriptorExtractor>(new SiftDescriptorExtractor);	
	//create BoF (or BoW) descriptor extractor
	bowDE=new BOWImgDescriptorExtractor(extractor,matcher);
}

GestureRecognition::~GestureRecognition(void)
{
	delete bowDE;
}

/*
load the classifier and dictionary from file
*/
GestureRecognition::GestureRecognition(char * classifier_name, char * dictionary_name)
{
	svm_model.load(classifier_name);
	FileStorage fs(dictionary_name, FileStorage::READ);
	fs["vocabulary"] >> dictionary;
	fs.release();	
	//create a nearest neighbor matcher
	matcher=Ptr<DescriptorMatcher>(new FlannBasedMatcher);
	//create Sift feature point extracter
	detector=Ptr<FeatureDetector>(new SiftFeatureDetector());
	//create Sift descriptor extractor
	extractor=Ptr<DescriptorExtractor>(new SiftDescriptorExtractor);	
	//create BoF (or BoW) descriptor extractor
	bowDE=new BOWImgDescriptorExtractor(extractor,matcher);
	//Set the dictionary with the vocabulary we created in the first step
	bowDE->setVocabulary(dictionary);
}

int GestureRecognition::recognize(IplImage *frame)
{
	vector<KeyPoint> keypoints;
	detector->detect(frame, keypoints);
	Mat descriptor;
	extractor->compute(frame, keypoints,descriptor);		
	Mat histogram;
	bowDE->compute(frame, keypoints, histogram);
	// apply the svm
	return (int) svm_model.predict(histogram);
}

bool GestureRecognition::is_active(Mat image, Rect window)
{
	//Mat window;
	//if (results.size()>0)
	//{
	//	// note, we manually expand the width of the image by half on both left and right
	//	int width=results[0].right-results[0].left;
	//	int left=results[0].left-width/2;
	//	int right=results[0].left+width*3/2;
	//	left=left>=0?left:0;
	//	right=right<image.cols?right:(image.cols-1);
	//	cv::Rect rect(left, results[0].top, right-left, results[0].bottom-results[0].top);
	//	cv::Mat(image, rect).copyTo(window);
	//}
	return true;
}