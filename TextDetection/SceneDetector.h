#include <opencv2/text/text.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/photo.hpp>
#include <opencv/cv.hpp>

#include <baseapi.h>
#include <allheaders.h>

#include<iostream>
#include<fstream>


#ifndef OWL_Scene_Text_Detector_H
#define OWL_Scene_Text_Detector_H

using namespace std;
using namespace cv;
using namespace cv::text;

namespace OWL
{
	class sceneTextDetector
	{
	private:
		Ptr<ERFilter> er_filter1, er_filter2;
		vector< Ptr<ERFilter> > er_filters1;
		vector< Ptr<ERFilter> > er_filters2;
		vector< vector<Vec2i> > nm_region_groups;
		
		vector<Mat> channels;
		tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
		


	public:
		Mat outputImage;
		vector<Rect> nm_boxes;
		sceneTextDetector();
		void set_Filter();
		void set_Channels_Group(Mat image, int mode, int type, string xm, double prob);
		void draw_Boxes(Mat image);
		void draw_BoxesOriginal(Mat image, double scale);
		void show_OutputImage();
		void start_TessEngine(Mat image);
		void run_OCR(double scale, Rect box);
		void read_WholeImage();
		
		vector<Rect> returnBoxes();
	};
}

#endif
