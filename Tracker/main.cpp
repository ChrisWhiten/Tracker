#include "Tracker.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{	
	QApplication a(argc, argv);
	Tracker w;
	w.show();
	return a.exec();
}
