#ifndef TRACKER_H
#define TRACKER_H

#include <QtGui/QMainWindow>
#include <qlabel.h>
#include <qtimer.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include "ui_Tracker.h"

class Tracker : public QMainWindow
{
	Q_OBJECT

public:
	Tracker(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Tracker();

private slots:
	void nextFrame();

private:
	Ui::TrackerClass ui;
	QLabel *my_label;
	QTimer *timer;

	static const int FPS = 30;
};

#endif // TRACKER_H
