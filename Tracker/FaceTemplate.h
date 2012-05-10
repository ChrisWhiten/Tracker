#ifndef FACETEMPLATE_H
#define FACETEMPLATE_H
#include <opencv2\core\core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

//
// Patch - a rectangular patch to be tracked
//

struct Patch
{
	int dx;   // of patch center with respect to template center
	int dy;   // of patch center with respect to template center
	int half_width;    //patch width is 2*w+1
	int half_height;    //patch height is 2*h+1
};


// FaceTemplate is a class that represents the tracked area.
// The main parameter you may want to play with is the tail_factor variable.
//
// tail factor is set at the bottom of the .cpp file.
// For a detected face, we take a ratio of the face's width and
// extend the tracking area down by that ratio.  So, if 
// tail_factor is set to 2.0, it takes twice the width of the face,
// and extends the tracked area by that length.  This lets us 
// gather some of the body for tracking as well.

class FaceTemplate
{
public:
	FaceTemplate(cv::Mat t, cv::Mat &parent, int _x, int _y, bool enhance=true, bool assign_new_id = true);

	void updateTemplate(cv::Mat t, cv::Mat &parent, int _x, int _y);
	void updateLocation(int _x, int _y);
	void updateDims(int h, int w, cv::Mat &parent);

	cv::Mat getTemplate();
	void setTemplate(cv::Mat t);

	int getWidth() const;
	int getHeight() const;
	int getYTopLeft() const;
	int getXTopLeft() const;
	int getYCenter() const;
	int getXCenter() const;
	cv::Scalar getColour() const;
	int getId() const;

	void setY(int _y);
	void setX(int _x);
	void setHeight(int h);
	void setWidth(int w);

	std::vector < Patch* > patches;
	std::vector < std::vector<double>* > patch_histograms;
	
private:
	cv::Mat face_template;
	cv::Scalar colour;
	static const double tail_factor;

	int height;
	int width;
	int x; // from topleft
	int y; // from topleft.
	int id;
	static int next_id;
};

#endif