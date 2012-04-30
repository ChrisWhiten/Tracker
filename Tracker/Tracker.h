#ifndef TRACKER_H
#define TRACKER_H

#include <QtGui/QMainWindow>
#include "ui_Tracker.h"

class Tracker : public QMainWindow
{
	Q_OBJECT

public:
	Tracker(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Tracker();

private:
	Ui::TrackerClass ui;
};

#endif // TRACKER_H
