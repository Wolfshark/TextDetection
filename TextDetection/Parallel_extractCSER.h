#include <opencv2/text/text.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/features2d.hpp>

using namespace std;
using namespace cv;
using namespace cv::text;

//ERStat extraction is done in parallel for different channels
class Parallel_extractCSER : public cv::ParallelLoopBody
{
private:
	vector<Mat> &channels;
	vector< vector<ERStat> > &regions;
	vector< Ptr<ERFilter> > er_filter1;
	vector< Ptr<ERFilter> > er_filter2;

public:
	Parallel_extractCSER(vector<Mat> &_channels, vector< vector<ERStat> > &_regions,
		vector<Ptr<ERFilter> >_er_filter1, vector<Ptr<ERFilter> >_er_filter2)
		: channels(_channels), regions(_regions), er_filter1(_er_filter1), er_filter2(_er_filter2){}

	virtual void operator()(const cv::Range &r) const
	{
		for (int c = r.start; c < r.end; c++)
		{
			er_filter1[c]->run(channels[c], regions[c]);
			er_filter2[c]->run(channels[c], regions[c]);
		}
	}
	Parallel_extractCSER & operator=(const Parallel_extractCSER &a);
};