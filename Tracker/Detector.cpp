#include "Detector.h"

Detector::Detector()
{
	face_examples = "C:/data/tracking/haarcascade_frontalface_alt.xml";

	// train the classifier
	if (!classifier.load(face_examples))
	{
		std::cout << "Could not load training examples for classifier.  Exiting." << std::endl;
		cv::waitKey();
		exit(1);
	}
}

void Detector::detect(Mat &img)
{
	faces.clear();
	Mat temp(img);
	//cv::cvtColor(img, temp, CV_BGR2GRAY);
	equalizeHist(temp, temp);
	classifier.detectMultiScale(temp, faces, 1.05, 10);//, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));

	// convert detected rectangles into tracks.
	for (unsigned i = 0; i < faces.size(); ++i)
	{
		Track t;
		t.x = faces[i].x;
		t.y = faces[i].y;
		t.width = faces[i].width;
		t.height = faces[i].height;

		tracks.push_back(t);
	}
}

const std::vector<Track> &Detector::getTracks() const
{
	return tracks;
}

std::vector<Track>::iterator Detector::firstTrack()
{
	return tracks.begin();
}

std::vector<Track>::iterator Detector::lastTrack()
{
	return tracks.end();
}

bool Detector::tracksFound() const
{
	return !tracks.empty();
}