#include "Tracker.h"

// These functions below convert a cv::Mat to a QImage.
// Adapted from http://stackoverflow.com/questions/5026965/how-to-convert-an-opencv-cvmat-to-qimage
// The grayscale ones seem to be inverted.  Look into this later.

QImage Mat2QImage(const cv::Mat3b &src) 
{
	QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
    for (unsigned row = 0; row < src.rows; ++row) 
	{
	    const cv::Vec3b *srcrow = src[row];
        QRgb *destrow = (QRgb*)dest.scanLine(row);
		for (unsigned col = 0; col< src.cols; ++col) 
		{
	        destrow[col] = qRgba(srcrow[col][2], srcrow[col][1], srcrow[col][0], 255);
        }
    }
	return dest;
}


QImage Mat2QImage(const cv::Mat_<double> &src)
{
        double scale = 255.0;
        QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
        for (int y = 0; y < src.rows; ++y) {
                const double *srcrow = src[y];
                QRgb *destrow = (QRgb*)dest.scanLine(y);
                for (int x = 0; x < src.cols; ++x) {
                        unsigned int color = srcrow[x] * scale;
                        destrow[x] = qRgba(color, color, color, 255);
                }
        }
        return dest;
}

QImage Mat2QImage(const cv::Mat_<unsigned char> &src)
{
        double scale = 255.0;
        QImage dest(src.cols, src.rows, QImage::Format_ARGB32);
        for (int y = 0; y < src.rows; ++y) {
                const unsigned char *srcrow = src[y];
                QRgb *destrow = (QRgb*)dest.scanLine(y);
                for (int x = 0; x < src.cols; ++x) {
                        unsigned int color = srcrow[x] * scale;
                        destrow[x] = qRgba(color, color, color, 255);
                }
        }
        return dest;
}


Tracker::Tracker(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	my_label = new QLabel();
	
	// Update the UI
	timer = new QTimer(this);
	timer->setInterval(1000/FPS);
	connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
	timer->start();
}

Tracker::~Tracker()
{

}


// [TODO]
// Fix the label geometry a bit...
// Resize the frame so it fits nicely into the window.
void Tracker::nextFrame()
{
	static int i = 0;
	std::stringstream ss;

	// Load frame from file and convert into Qt format
	ss << "C:/data/face/" << i << ".jpg";
	cv::Mat3b frame = cv::imread(ss.str());
	QImage my_image = Mat2QImage(frame);

	ui.input_sequence->setGeometry(0, 0, frame.cols, frame.rows);
	ui.input_sequence->setPixmap(QPixmap::fromImage(my_image));
	ui.input_label->setGeometry(frame.cols/2, frame.rows + 10, ui.input_label->width(), ui.input_label->height());

	// for now, just repeat the input sequence.  The tracked image should be here.
	ui.output_sequence->setGeometry(frame.cols + 10, 0, frame.cols, frame.rows);
	ui.output_sequence->setPixmap(QPixmap::fromImage(my_image));
	ui.output_label->setGeometry((3 * frame.cols)/2 + 10, frame.rows + 10, ui.output_label->width(), ui.output_label->height());

	ss.str("");
	i++;
	repaint();
}