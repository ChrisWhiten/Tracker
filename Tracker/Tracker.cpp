#include "Tracker.h"


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
{}

// Track the frame through each target tracker,
// and display the results with a coloured rectangle.
void Tracker::trackFrame(cv::Mat &input, cv::Mat &output)
{
	for (unsigned i = 0; i < trackers.size(); ++i)
	{
		std::vector<Track> tracks = trackers[i]->trackFrame(input);
		for (unsigned j = 0; j < tracks.size(); ++j)
		{
			cv::Point tl(tracks[i].x, tracks[i].y);
			cv::Point br(tracks[i].x + tracks[i].width, tracks[i].y + tracks[i].height);
			cv::rectangle(output, tl, br, trackers[i]->colour, 2);
		}
	}
}

// 'slot' callback emitted from the timer timing out.
// Takes next frame, feeds it into the tracker, and displays 
// the results on our Qt window.
//
// Kind of messy, needs to be cleaned up in the future.
// [TODO]: Gracefully handle end of frames.
// Allow user to select input sequence.
// Handle both .avis and sequence of .jpgs.
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

	QImage qimage_frame = OpenCVToQtInterfacing::Mat2QImage(frame);
	ui.input_sequence->setPixmap(QPixmap::fromImage(qimage_frame));

	int input_label_x = (frame.cols/2) - (ui.input_label->width()/2);
	ui.input_label->setGeometry(input_label_x, frame.rows + 10, ui.input_label->width(), ui.input_label->height());

	// arrange tracked image.
	ui.output_sequence->setGeometry(output_frame.cols + 10, 0, output_frame.cols, output_frame.rows);

	QImage qimage_output_frame = OpenCVToQtInterfacing::Mat2QImage(output_frame);
	ui.output_sequence->setPixmap(QPixmap::fromImage(qimage_output_frame));

	int output_label_x = ((3 * output_frame.cols)/2 + 10) - (ui.output_label->width()/2);
	ui.output_label->setGeometry(output_label_x, output_frame.rows + 10, ui.output_label->width(), ui.output_label->height());

	ss.str("");
	i++;
	repaint();
}