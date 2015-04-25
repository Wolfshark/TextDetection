#include "SceneDetector.h"

using namespace OWL;

//-------------------------------------------------------------------------------------------------------
//MOUSE CLICK EVENT RETURNS COORDINATES
void mouseEvent(int event, int x, int y, int flags, void* param)
{
	IplImage* img = (IplImage*)param;
	if (event == EVENT_LBUTTONDOWN)
	{
		printf("%d,%d\n", x, y);
	}
}
//-------------------------------------------------------------------------------------------------------
double scale = 1.0;

std::vector<cv::Rect> detectLetters(cv::Mat img)
{
	std::vector<cv::Rect> boundRect;
	cv::Mat img_gray, img_sobel, img_threshold, element;
	cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
	cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
	cv::threshold(img_sobel, img_threshold, 0, 255, cv::THRESH_OTSU + cv::THRESH_BINARY);
	element = getStructuringElement(cv::MORPH_RECT, cv::Size(60, 60));
	cv::morphologyEx(img_threshold, img_threshold, cv::MORPH_CLOSE, element); //Does the trick
	std::vector< std::vector< cv::Point> > contours;
	cv::findContours(img_threshold, contours, 0, 1);
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());
	for (int i = 0; i < contours.size(); i++)
		if (contours[i].size()>100)
		{
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		cv::Rect appRect(boundingRect(cv::Mat(contours_poly[i])));
		if (appRect.width>appRect.height)
			boundRect.push_back(appRect);
		}
	return boundRect;
}

//-------------------------------------------------------------------------------------------------------
//REMOVING BACKGROUND BY CHECKING IF POINT IS IN REGION
Mat RemoveBackGround(Mat image, sceneTextDetector run, vector<vector<Point>>& coor)
{
	vector<Rect> boxes;
	boxes = run.nm_boxes;

	vector<Point> boxPoint;
	for (int i = 0; i < boxes.size(); i++)//Finding Corners of ALL Boxes
	{
		int a, b, c, d;
		a = boxes[i].x;
		b = boxes[i].y;
		c = a + boxes[i].width;
		d = b + boxes[i].height;
		boxPoint.push_back(Point(a, b));
		boxPoint.push_back(Point(a, d));
		boxPoint.push_back(Point(c, b));
		boxPoint.push_back(Point(c, d));

		coor.push_back(boxPoint);
		boxPoint.clear();
	}
	//ITERATE AND CHECK

	for (int i = 0; i < image.rows; i++)
	{
		Vec3b* pixel = image.ptr<Vec3b>(i);
		bool isIn = false;
		for (int j = 0; j < image.cols; j++)
		{
			for (int k = 0; k < coor.size(); k++)
			{
				if ((((j >= coor[k][0].x) && (j <= coor[k][2].x)) && ((i >= coor[k][0].y) && (i <= coor[k][1].y))))
				{
					isIn = true;
				}
			}
			if (!isIn)
			{
				pixel[j][0] = 0;
				pixel[j][1] = 0;
				pixel[j][2] = 0;
			}
			isIn = false;
		}
	}
	return image;
}

void FindBoxes(vector<vector<Point>>& coor, sceneTextDetector run)
{
	vector<Rect> boxes;
	boxes = run.nm_boxes;

	vector<Point> boxPoint;
	for (int i = 0; i < boxes.size(); i++)//Finding Corners of ALL Boxes
	{
		int a, b, c, d;
		a = boxes[i].x;
		b = boxes[i].y;
		c = a + boxes[i].width;
		d = b + boxes[i].height;
		boxPoint.push_back(Point(a, b));
		boxPoint.push_back(Point(a, d));
		boxPoint.push_back(Point(c, b));
		boxPoint.push_back(Point(c, d));

		coor.push_back(boxPoint);
		boxPoint.clear();
	}
}
//GENERATING AVERAGE BOX OF MULTIPLE BOX CORNERS
void createLargeBox(Mat image, vector<vector<Point>> coordinates, Rect& box)
{
	int x, y, a, b;
	int maxX, maxY, minX, minY;
	maxX = coordinates[0][0].x;
	maxY = coordinates[0][0].y;
	minX = coordinates[0][0].x;
	minY = coordinates[0][0].y;
	for (int i = 0; i < coordinates.size(); i++)
	{
		for (int j = 0;  j < coordinates[i].size(); j++)
		{
			x = coordinates[i][j].x;
			y = coordinates[i][j].y;
			a = coordinates[i][j].x;
			b = coordinates[i][j].y;
			if (x > maxX)
			{maxX = x;}
			if (y > maxY)
			{maxY = y;}
			if (a < minX)
			{minX = a;}
			if (b < minY)
			{minY = b;}
		}
	}
	box.x = minX - 0.01*image.cols;
	box.y = minY - 0.01*image.rows;
	box.width = (maxX - minX) + 0.01*image.cols;
	box.height = (maxY - minY) + 0.01*image.rows;
	//cout << minX << "," << minY << endl << maxX << "," << maxY  << endl;
	//cout << box.x << " " << box.y << " " << box.x + box.width << " " << box.y + box.height << endl;
}
//DRAW RECTANGLE OF AVERAGE BOX
void draw_MasterBox(Mat image, Rect box)
{
		rectangle(image, box.tl(), box.br(), Scalar(255, 255, 0), 1);
}

void create_border(Mat& image, Mat& return_image)
{
	int top, bottom, left, right;
	Scalar value;
	top = (int)(0.01*image.rows);
	bottom = (int)(0.01*image.rows);
	left = (int)(0.01*image.cols);
	right = (int)(0.01*image.cols);

	return_image = image;

	value = Scalar(0, 0, 0);
	copyMakeBorder(image, return_image, top, bottom, left, right, BORDER_CONSTANT, value);
}

//-------------------------------------------------------------------------------------------------------
void main()
{
	sceneTextDetector run;
	//-------------------------------------------------------------------------------------------------------
	Mat image;
	Mat out_image, smallim;
	image = imread("C:/Users/Luu/Desktop/TestImages/signs.jpg");
	resize(image, smallim, Size(scale * image.cols, scale * image.rows), 3, 3, INTER_LINEAR);
	namedWindow("Original Image", WINDOW_NORMAL);
	namedWindow("Removed Background", WINDOW_NORMAL);
	namedWindow("NM Algorithm", WINDOW_NORMAL);
	namedWindow("Master Box", WINDOW_NORMAL);
	namedWindow("Bordered Image", WINDOW_NORMAL);

	imshow("Original Image", smallim);
	Mat borderImage;
	create_border(smallim, borderImage);
	imshow("Bordered Image", borderImage);
	run.set_Filter();
	run.set_Channels_Group(borderImage, ERFILTER_NM_IHSGrad, ERGROUPING_ORIENTATION_ANY, "trained_classifier_erGrouping.xml", 0.5);
	borderImage.copyTo(out_image);
	run.draw_Boxes(out_image);
	imshow("NM Algorithm", out_image);

	vector<vector<Point>> coor;
	Rect finalBox;
	//smallim = RemoveBackGround(smallim, run, coor);
	FindBoxes(coor, run);

	if (coor.size() == 0)
	{
		cout << "Nothing Detected. Exiting." << endl;
		return;
	}

	createLargeBox(smallim,coor, finalBox);
	imshow("Removed Background", borderImage);

	draw_MasterBox(borderImage, finalBox);
	imshow("Master Box", borderImage);

	/*----------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------*/

	/*Rect ROI(finalBox.x, finalBox.y, finalBox.width, finalBox.height);
	Mat finalImage = image(ROI);
	Mat croppedImage;
	finalImage.copyTo(croppedImage);
	
	std::vector<cv::Rect> letterBBoxes1 = detectLetters(croppedImage);

	for (int i = 0; i< letterBBoxes1.size(); i++)
		cv::rectangle(croppedImage, letterBBoxes1[i], cv::Scalar(0, 255, 0), 3, 8, 0);

	imshow("Cropped Image", croppedImage);*/

	setMouseCallback("Removed Background", mouseEvent, &smallim);	
	
	run.start_TessEngine(borderImage);
	run.run_OCR(1.0, finalBox);
	//run.read_WholeImage();
	waitKey(0); 
	//-------------------------------------------------------------------------------------------------------

	/* cam_idx = 0;
	Mat frame;
	VideoCapture cap(cam_idx);
	cap.set(CAP_PROP_FRAME_WIDTH, 300);
	cap.set(CAP_PROP_FRAME_HEIGHT, 300);

	namedWindow("Capture", WINDOW_AUTOSIZE);

	if (!cap.isOpened())
	{
		cout << "ERROR: Cannot open default camera (0)." << endl;
	}
	Mat image;
	run.set_Filter();
	while (cap.read(image))
	{
		Mat out_image;
		image.copyTo(out_image);
		run.set_Channels_Group(image, ERFILTER_NM_IHSGrad, ERGROUPING_ORIENTATION_HORIZ, "trained_classifier_erGrouping.xml", 0.5);
		run.draw_Boxes(out_image);
		imshow("Capture", out_image);
		//waitKey(0);
		//run.start_TessEngine(image);
		//run.run_OCR();
		int key = waitKey(30);
		if (key == 27)
		{
			cout << "esc key pressed" << endl;
			break;
		}
	

	}*/

}