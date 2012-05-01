#ifndef BASETRACKER_H
#define BASETRACKER_H

#include <opencv2/core/core.hpp>
#include <vector>

struct Track
{
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
	};

	~BaseTracker(){};

	virtual void init() = 0;
	virtual std::vector<Track> trackFrame(cv::Mat &frame) = 0;

protected:
	
	int id;
	std::string tracker_name;
	// also add representative colour. [TODO]
};

#endif