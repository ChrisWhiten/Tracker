#include "Tracker.h"


Tracker::Tracker(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), reading_sequence_of_images(false)
{
	ui.setupUi(this);

	connect(ui.actionLoad, SIGNAL(triggered()), this, SLOT(loadFiles()));
	connect(ui.actionBeginTracking, SIGNAL(triggered()), this, SLOT(beginTracking()));


	// initialize trackers
	trackers.push_back(new DummyTracker());
	
	// Update the UI
	timer = new QTimer(this);
	timer->setInterval(1000/Constants::FPS);
	connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
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
void Tracker::nextFrame()
{
	// load frame.
	cv::Mat3b frame = getFrame();
	if (!frame.data)
	{
		return;
	}

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

	repaint();
}

// Slot associated with the user wanting to select files to load.
void Tracker::loadFiles()
{
    files = QFileDialog::getOpenFileNames(this, tr("Directory"), directory.path());
	if (files.length() > 1)
	{
		reading_sequence_of_images = true;
	}
	else
	{
		// initialize video to be read.
		capture = new cv::VideoCapture(files[0].toStdString());
		reading_sequence_of_images = false;
	}
}

void Tracker::beginTracking()
{
	timer->start();
}

void Tracker::endTracking()
{
	timer->stop();
	if (capture)
	{
		delete capture;
	}

	// this is where we might also write some logs and results.
}

cv::Mat3b Tracker::getFrame()
{
	cv::Mat3b frame;
	if (reading_sequence_of_images)
	{
		static int frame_number = 0;
		if (frame_number >= files.size())
		{
			return frame;
		}

		QString filename = files[frame_number];
		frame = cv::imread(filename.toStdString());
		frame_number++;
	}
	else
	{
		*capture >> frame;	
	}
	return frame;
}