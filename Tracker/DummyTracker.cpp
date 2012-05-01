#include "DummyTracker.h"

DummyTracker::DummyTracker(std::string name)
{
}

DummyTracker::~DummyTracker()
{
}

void DummyTracker::init()
{
}

std::vector<Track> DummyTracker::trackFrame(cv::Mat &frame)
{
	std::vector<Track> ret;

	Track t;
	t.x = 5;
	t.y = 20;
	t.height = 50;
	t.width = 5;
	t.id = this->id;

	ret.push_back(t);
	return ret;
}