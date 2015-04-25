#include "SceneDetector.h"
#include "Parallel_extractCSER.h"

namespace OWL
{
	sceneTextDetector::sceneTextDetector()
	{}

	
	void sceneTextDetector::set_Filter()
	{
		for (int i = 0; i<2; i++)
		{
			Ptr<ERFilter> er_filter1 = createERFilterNM1(loadClassifierNM1("trained_classifierNM1.xml"), 7, 0.00015f, 0.13f, 0.2f, true, 0.02f);
			Ptr<ERFilter> er_filter2 = createERFilterNM2(loadClassifierNM2("trained_classifierNM2.xml"), 0.2);
			er_filters1.push_back(er_filter1);
			er_filters2.push_back(er_filter2);
		}
	}
	void sceneTextDetector::set_Channels_Group(Mat image, int mode, int type, string xm, double prob)
	{
		//-----------------------------------------------------------------------------CSER
		/*computeNMChannels(image, channels, mode);

		int cn = (int)channels.size();
		for (int c = 0; c < cn - 1; c++)
			channels.push_back(255 - channels[c]);

		

		for (int c = 0; c<(int)channels.size(); c++)
		{
			er_filter1->run(channels[c], regions[c]);
			er_filter2->run(channels[c], regions[c]);
		}*/
		Mat grey;
		vector<vector<ERStat> > regions(2);
		cvtColor(image, grey, COLOR_RGB2GRAY);
		channels.clear();
		channels.push_back(grey);
		channels.push_back(255 - grey);
		regions[0].clear();
		regions[1].clear();
		nm_region_groups.clear();
		nm_boxes.clear();
		parallel_for_(cv::Range(0, (int)channels.size()), Parallel_extractCSER(channels, regions, er_filters1, er_filters2));
		//-----------------------------------------------------------------------------MSER
	/*	vector<vector<Point> > contours;
		vector<Rect> bboxes;
		Mat grey;
		cvtColor(image, grey, COLOR_RGB2GRAY);
		channels.clear();
		channels.push_back(grey);
		channels.push_back(255 - grey);
		regions.resize(channels.size());
		nm_region_groups.clear();
		nm_boxes.clear();
		
		Ptr<MSER> mser = MSER::create(21, (int)(0.00002*grey.cols*grey.rows), (int)(0.05*grey.cols*grey.rows), 1, 0.7);
		mser->detectRegions(grey, contours, bboxes);
		if (contours.size() > 0)
			MSERsToERStats(grey, contours, regions);*/
		//-----------------------------------------------------------------------------
		erGrouping(image, channels, regions, nm_region_groups, nm_boxes, type, xm, prob);
	}
	void sceneTextDetector::draw_Boxes(Mat image)
	{
		for (int i = 0; i<(int)nm_boxes.size(); i++)
		{
			rectangle(image, nm_boxes[i].tl(), nm_boxes[i].br(), Scalar(255, 255, 0), 1);
		}
		
	}

	void sceneTextDetector::draw_BoxesOriginal(Mat image, double scale)
	{
		for (int i = 0; i<(int)nm_boxes.size(); i++)
		{
			rectangle(image, scale*nm_boxes[i].tl(), scale*nm_boxes[i].br(), Scalar(255, 255, 0), 1);
		}
	}

	void sceneTextDetector::show_OutputImage()
	{
		imshow("Boxed Images", outputImage);
	}
	void sceneTextDetector::start_TessEngine(Mat image)
	{
		api->Init(NULL, "eng");
		api->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
		Mat gray;
		cvtColor(image, gray, COLOR_RGB2GRAY);
		api->SetImage((uchar*)gray.data, gray.cols, gray.rows, 1, gray.cols);
	}
	void sceneTextDetector::run_OCR(double scale, Rect box)
	{
		fstream myFile;
		myFile.open("OCR1.txt", ios::in | ios::app);
		api->SetRectangle(scale*(box.x), scale* (box.y), scale * (box.width), scale * (box.height));
		char* ocrResult = api->GetUTF8Text();
		//cout << "Scale" << ocrResult << " " << nm_boxes[i].x << " " << nm_boxes[i].y << " " << nm_boxes[i].width << " " << nm_boxes[i].height << endl;

		//cout << ocrResult << " " << scale* nm_boxes[i].x << " " << scale*nm_boxes[i].y << " " << scale*nm_boxes[i].width << " " << scale*nm_boxes[i].height << endl;
		cout << ocrResult;
		//myFile << ocrResult << " " << nm_boxes[i].x  << " " << nm_boxes[i].y << " " << nm_boxes[i].width << " " << nm_boxes[i].height  << endl;
		myFile.close();
	}
	vector<Rect> sceneTextDetector::returnBoxes()
	{
		return nm_boxes;
	}
	void sceneTextDetector::read_WholeImage()
	{
		char* out = api->GetUTF8Text();
		cout << out << endl;
	}

}