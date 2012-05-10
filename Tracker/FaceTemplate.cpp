#include "FaceTemplate.h"

FaceTemplate::FaceTemplate(cv::Mat t, cv::Mat &parent, int _x, int _y, bool enhance, bool assign_new_id)
{
	if (assign_new_id)
	{
		id = next_id;
		next_id++;
	}

	if (enhance)
	{
		double rect_height = t.rows + (t.cols * tail_factor);
		if ((rect_height + _y) >= parent.rows) { rect_height = parent.rows - _y - 1; } 
		cv::Rect rect(_x, _y, t.cols, rect_height);
		cv::Mat tmp = parent(rect);
		face_template = tmp.clone();
		tmp.release();
	}
	else
	{
		face_template = t;
	}

	x = _x;
	y = _y;
	height = face_template.rows;
	width = face_template.cols;
	colour = cv::Scalar(255, 20*id, 20*id);
}

void FaceTemplate::updateTemplate(cv::Mat t, cv::Mat &parent, int _x, int _y)
{
	face_template = t;
	updateLocation(_x, _y);
	updateDims(t.rows, t.cols, parent);
}

void FaceTemplate::updateLocation(int _x, int _y)
{
	x = _x;
	y = _y;
}

void FaceTemplate::updateDims(int h, int w, cv::Mat &parent)
{
	if (y + h > parent.rows)
	{
		height = parent.rows - y; // - 1;
		std::cout << "shrinking in updateDims.  y and height = " << y << ", " << h << " and parent.rows = " << parent.rows << std::endl;
	}
	else
	{
		height = h;
	}

	if (x + w >= parent.cols)
	{
		width = parent.cols - x; // - 1;
	}
	else
	{
		width = w;
	}
}

cv::Mat FaceTemplate::getTemplate()
{
	return face_template;
}

void FaceTemplate::setTemplate(cv::Mat t)
{
	face_template = t;
}

cv::Scalar FaceTemplate::getColour() const
{
	return colour;
}

int FaceTemplate::getId() const
{
	return id;
}

int FaceTemplate::getWidth() const
{
	return width;
}

int FaceTemplate::getHeight() const
{
	return height;
}

int FaceTemplate::getXTopLeft() const
{
	return x;
}

int FaceTemplate::getYTopLeft() const
{
	return y;
}

int FaceTemplate::getXCenter() const
{
	int half_w = (int) floor((double)width / 2.0);
	return x + half_w;
}

int FaceTemplate::getYCenter() const
{
	int half_h = (int) floor((double)height / 2.0);
	return y + half_h;
}

void FaceTemplate::setY(int _y)
{
	y = _y;
}

void FaceTemplate::setX(int _x)
{
	x = _x;
}

void FaceTemplate::setHeight(int h)
{
	height = h;
}

void FaceTemplate::setWidth(int w)
{
	width = w;
}

const double FaceTemplate::tail_factor = 0.0;
int FaceTemplate::next_id = 0;