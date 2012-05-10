#ifndef BASETRACKER_H
#define BASETRACKER_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

struct Track
{
	Track(int _x, int _y, int h, int w, int i) : x(_x), y(_y), height(h), width(w), id(i){};
	Track(){};
	int x; // (x, y) from top-left corner
	int y;
	int height;
	int width;
	int id; // identify a track when there are multiple within the same scene.
};

class BaseTracker
{
public:
	BaseTracker(std::string name = "Default Tracker Name")
	{
		static int next_id = 0;
		id = next_id++;
		tracker_name = name;
		colour = CV_RGB(rand() % 255, rand() % 255, rand() % 255);
	};

	~BaseTracker(){};

	virtual void init() = 0;
	virtual std::vector<Track> trackFrame(cv::Mat &frame) = 0;
	virtual void addTracks(cv::Mat &I, const std::vector<Track> &t) = 0;

	std::string tracker_name;
	cv::Scalar colour;

protected:
	int id;	
};

#endif