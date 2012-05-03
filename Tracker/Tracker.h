#ifndef TRACKER_H
#define TRACKER_H

#include <QtGui/QMainWindow>
#include <qtimer.h>

// for file/directory manipulations
#include <qdir.h>
#include <qfiledialog.h>

// for image processing and representation
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// basic C++ operations.
#include <iostream>
#include <string>
#include <sstream>

#include "ui_Tracker.h"
#include "Constants.h"
#include "DummyTracker.h"
#include "OpenCVToQtInterfacing.h"

class Tracker : public QMainWindow
{
	Q_OBJECT

public:
	Tracker(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Tracker();

private slots:
	void nextFrame();
	void loadFiles();
	void beginTracking();

private:
	Ui::TrackerClass ui;
	QDir directory;
	QTimer *timer;
	QStringList files;
	std::vector<BaseTracker *> trackers;
	cv::VideoCapture *capture;

	// false if reading from an actual video file.
	bool reading_sequence_of_images;
	
	void trackFrame(cv::Mat &input, cv::Mat &output);
	cv::Mat3b getFrame();
	void endTracking();
	

};

#endif // TRACKER_H
