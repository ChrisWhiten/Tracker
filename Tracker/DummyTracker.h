#ifndef DUMMYTRACKER_H
#define DUMMYTRACKER_H

#include "BaseTracker.h"

class DummyTracker : public BaseTracker
{
public:
	DummyTracker(std::string name = "Dummy Tracker");
	~DummyTracker();
	void init();
	std::vector<Track> trackFrame(cv::Mat &frame);
	void addTracks(cv::Mat &I, const std::vector<Track> &t);
};

#endif