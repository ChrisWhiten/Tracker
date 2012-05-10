#ifndef FRAGMENTS_TRACKER_H
#define FRAGMENTS_TRACKER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#include "FaceTemplate.h"
#include "BaseTracker.h"
using namespace std;

// FragTrack
//
// To access this class, the main methods are the constructor
// and the handleFrame method.
//
// Pass the input image, the image to output onto, and a vector of rectangles 
// that contain the desired tracking regions to the constructor.
//
// Pass the input image, output image, and the name of the window to output onto,
// for the handleFrame method.  Just loop on handleFrame for every frame.
//
// Two parameters you may want to play with are the top two private parameters here.
// num_bins is the number of bins for the integral histogram, while
// search_radius is the number of pixels we search in each direction for the next location
// of the tracked area.  Increasing these should increase accuracy, but slow down the tracker.

class FragTrack : public BaseTracker
{
public:
	cv::Mat img; // current frame

	FragTrack(std::string name = "FragTrack");
	std::vector<Track> trackFrame(cv::Mat &frame);
	void addTracks(cv::Mat &I, const std::vector<Track> &t);
	void init();
	~FragTrack(void);

private:
	static const int num_bins = 8;
	static const int search_radius = 17;

	vector <cv::Mat*> IIV_I; // integral histogram of the image.
	vector <cv::Mat*> IIV_I2; // integral histogram of the image.
	vector <vector<double>*> template_patches_histograms;
	vector <cv::Mat*> patch_vote_maps;
	vector<FaceTemplate *> tracks; // tracked objects in the scene

	void definePatches(FaceTemplate *f);
	void buildTemplatePatchHistograms(FaceTemplate *face,
		                                 vector<vector<double>*>& patch_histograms);

	void getBinForEachPixel(cv::Mat &I, cv::Mat* bin_mat);
	bool computeIH(cv::Mat &I, vector< cv::Mat* >& vec_II);
	bool computeHistogram(int tl_y, int tl_x, int br_y, int br_x,
		                   vector<cv::Mat*> &iiv, vector<double> &hist);

	double compareHistograms(vector <double> &h1 , vector <double> &h2);
	void computeSinglePatchVotes (Patch* p , FaceTemplate *f, vector <double>& hist,
										   int minrow, int mincol,
										   int maxrow, int maxcol,
										   cv::Mat* votes, int& min_r, int& min_c,
										   int& max_r, int& max_c, FaceTemplate *ref);

	void computeAllPatchVotes(FaceTemplate *track,
									FaceTemplate *reference,
								    int img_height, int img_width,
   								    int minrow, int mincol,
								    int maxrow, int maxcol , cv::Mat* combined_vote,
								    vector<int>& x_coords,
								    vector<int>& y_coords,
								    vector<double>& patch_scores) ;

	void combineVoteMaps(vector< cv::Mat* >& vote_maps, cv::Mat* V, int mincol, 
							int minrow, int maxcol, int maxrow, FaceTemplate *f);

	void findTemplate(FaceTemplate * track, 
					   int img_height, int img_width,
					   int& result_y, int& result_x, int &result_height, int &result_width, double& score,
					   vector<int>& x_coords, vector<int>& y_coords);

	void updateTemplate(FaceTemplate *face, int new_height,int new_width,
						   int new_cy, int new_cx, double scale_factor,
						   cv::Mat &I);

	void drawRectangle(FaceTemplate *face, cv::Mat* I);
	void drawRectangle(cv::Mat* I, cv::Rect r);
	cv::Mat getROI(cv::Mat parent, int x, int y, int width, int height);
};

#endif