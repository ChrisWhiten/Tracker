#ifndef DETECTOR_H
#define DETECTOR_H

#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\core\core.hpp>
#include <iostream>
#include <stdio.h>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "BaseTracker.h" // included for Track struct.

using namespace cv;
class Detector
{
public:
	Detector();
	void detect(Mat img);
	const std::vector<Track> &getTracks() const;
	std::vector<Track>::iterator firstTrack();
	std::vector<Track>::iterator lastTrack();
	bool tracksFound() const;

private:
	int x;
	std::string face_examples;
	std::vector<Rect> faces;
	std::vector<Track> tracks;
	CascadeClassifier classifier;
};
#endif