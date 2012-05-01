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

	// initialize trackers
	trackers.push_back(new DummyTracker());
	
	// Update the UI
	timer = new QTimer(this);
	timer->setInterval(1000/Constants::FPS);
	connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
	timer->start();
}

Tracker::~Tracker()
{

}


void Tracker::trackFrame(cv::Mat &input, cv::Mat &output)
{
	for (unsigned i = 0; i < trackers.size(); ++i)
	{
		std::vector<Track> tracks = trackers[i]->trackFrame(input);

		// when colour is available, use that to draw the correctly coloured rectangle.
		for (unsigned j = 0; j < tracks.size(); ++j)
		{
			cv::Point tl(tracks[i].x, tracks[i].y);
			cv::Point br(tracks[i].x + tracks[i].width, tracks[i].y + tracks[i].height);
			cv::rectangle(output, tl, br, CV_RGB(255, 0, 0));
		}
	}
}

void Tracker::nextFrame()
{
	static int i = 0;
	std::stringstream ss;

	// Load frame from file and convert into Qt format
	ss << "C:/data/face/" << i << ".jpg";

	cv::Mat3b frame = cv::imread(ss.str());
	cv::Mat3b output_frame(frame.rows, frame.cols);
	frame.copyTo(output_frame);

	trackFrame(frame, output_frame);
	
	// arrange raw and tracked frames to be viewable in our window.
	if (Constants::RESIZE_OUTPUT)
	{
		int frame_width = ui.centralWidget->width()/3;
		int frame_height = ui.centralWidget->height()/2;
		cv::resize(frame, frame, cv::Size(frame_width, frame_height), 0, 0);
		cv::resize(output_frame, output_frame, cv::Size(frame_width, frame_height), 0, 0);
	}

	// arrange raw image.
	ui.input_sequence->setGeometry(0, 0, frame.cols, frame.rows);

	QImage qimage_frame = Mat2QImage(frame);
	ui.input_sequence->setPixmap(QPixmap::fromImage(qimage_frame));

	int input_label_x = (frame.cols/2) - (ui.input_label->width()/2);
	ui.input_label->setGeometry(input_label_x, frame.rows + 10, ui.input_label->width(), ui.input_label->height());

	// arrange tracked image.
	ui.output_sequence->setGeometry(output_frame.cols + 10, 0, output_frame.cols, output_frame.rows);

	QImage qimage_output_frame = Mat2QImage(output_frame);
	ui.output_sequence->setPixmap(QPixmap::fromImage(qimage_output_frame));

	int output_label_x = ((3 * output_frame.cols)/2 + 10) - (ui.output_label->width()/2);
	ui.output_label->setGeometry(output_label_x, output_frame.rows + 10, ui.output_label->width(), ui.output_label->height());

	ss.str("");
	i++;
	repaint();
}