#include "Tracker.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	srand(0);
	QApplication a(argc, argv);
	Tracker w;
	w.show();
	return a.exec();
}
