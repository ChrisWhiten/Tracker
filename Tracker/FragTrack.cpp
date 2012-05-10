#include "FragTrack.h"
#include <opencv\highgui.h>
#include <cmath>

using namespace std;

cv::Mat FragTrack::getROI(cv::Mat parent, int x, int y, int width, int height)
{
	int true_width, true_height;

	if (x + width >= parent.cols)
	{
		true_width = parent.cols - x - 1; 
	}
	else
	{
		true_width = width;
	}

	if (y + height >= parent.rows)
	{
		true_height = parent.rows - y - 1;
	}
	else
	{
		true_height = height;
	}

	cv::Mat ret = parent(cv::Rect(x, y, true_width, true_height));
	return ret;
}

void FragTrack::addTracks(cv::Mat &I, const std::vector<Track> &t)
{
	img = I;
	// allocate space for the IIV_I and the combined votes maps
	cv::Mat* curr_ii;
	

	int i;
	for (i=0; i < num_bins; i++) 
	{
		curr_ii = new cv::Mat(I.rows,I.cols,CV_32S);
		IIV_I.push_back(curr_ii);
	}


	buildIntegralHistogram(I, IIV_I);

	for (unsigned i = 0; i < t.size(); ++i)
	{
		bool is_new = true;
		for (unsigned j = 0; j < tracks.size(); ++j)
		{
			// manhattan distance
			int dist = 0;
			dist += abs(tracks[j]->getXTopLeft() - t[i].x);
			dist += abs(tracks[j]->getYTopLeft() - t[i].y);

			if (dist < 80)
			{
				is_new = false;
				break;
			}
		}

		// if it's new, add it to the tracks.
		if (is_new)
		{
			cout << "if it's new i want to see. " << endl;
			for (unsigned j = 0; j < tracks.size(); ++j)
			{
				// manhattan distance
				int dist = 0;
				dist += abs(tracks[j]->getXTopLeft() - t[i].x);
				dist += abs(tracks[j]->getYTopLeft() - t[i].y);
				cout << "distance is " << 40 << endl;
			}

			cv::waitKey();


			cv::Mat tmp_mat = getROI(I, t[i].x, t[i].y, t[i].width, t[i].height);

			cv::Mat *curr_template = new cv::Mat(tmp_mat.clone());
			tmp_mat.release();
			FaceTemplate *ft = new FaceTemplate(*curr_template, I, t[i].x, t[i].y);

			// template center
			int t_halfw = (int)floor((double)ft->getWidth() / 2.0 );
			int t_halfh = (int)floor((double)ft->getHeight() / 2.0 );


			// build the histograms of the patches in the template
			buildTemplatePatchHistograms(ft);
			tracks.push_back(ft);
		}
	}
}

FragTrack::FragTrack(std::string name)
{}

FragTrack::~FragTrack(void)
{
	vector < cv::Mat* >::iterator it;
	for ( it = IIV_I.begin() ; it != IIV_I.end() ; it++ ) {
		(*it)->release();
	}
	for ( it = patch_vote_maps.begin() ; it != patch_vote_maps.end() ; it++ ) {
		(*it)->release();
	}

	vector < FaceTemplate *>::iterator track;
	for (track = tracks.begin(); track != tracks.end(); ++track)
	{

		vector < Patch *>::iterator patch;
		for (patch = (*track)->patches.begin(); patch != (*track)->patches.end(); ++patch)
		{
			delete (*patch);
		}
	}

}


// Build the histograms for each of the "fragments" (patches)
void FragTrack::buildTemplatePatchHistograms(FaceTemplate *face)
{
	// clear the current histograms
	for (auto it = face->patch_histograms.begin(); it != face->patch_histograms.end(); it++)
	{
		(*it)->clear();
		delete (*it);
	}
	face->patch_histograms.clear();
	face->patches.clear();

	// define the patches on this track
	definePatches(face);

	// now compute the histograms for every defined patch
	for (auto patch = face->patches.begin() ; patch != face->patches.end() ; ++patch)
	{
		// compute current patch histogram
		vector <double> *current_histogram = new vector<double>;
		int tl_y = face->getYCenter() + (*patch)->dy - (*patch)->half_height;
		int tl_x = face->getXCenter() + (*patch)->dx - (*patch)->half_width;
		int br_y = face->getYCenter() + (*patch)->dy + (*patch)->half_height;
		int br_x = face->getXCenter() + (*patch)->dx + (*patch)->half_width;
		computeHistogram(tl_y, tl_x, br_y, br_x, IIV_I, *current_histogram);
		face->patch_histograms.push_back(current_histogram);
	}

	return;
}

void FragTrack::definePatches(FaceTemplate *f)
{
	int height = f->getHeight();
	int width = f->getWidth();

	// delete old patches.
	for (int i = 0 ; i < f->patches.size(); i++)
	{
		delete f->patches[i];
	}

	f->patches.clear();

	// vertical patches.
	// we want to fit 10 vertical patches across the width of the template.
	// each vertical patch is half the height of the template
	// so we get 20 vertical patches.

	Patch *p1, *p2;
	int centre_x = f->getXCenter();
	int centre_y = f->getYCenter();



	// vertical patches
	int patch_width = (int)floor((double)width/10);// - 1;
	int patch_halfwidth = (int)floor((double)patch_width/2);

	int patch_height = (int)floor((double)height/2);// - 1;
	int patch_halfheight = (int)floor((double)patch_height/2);
	
	cv::Mat pd(f->getTemplate());
	for (int i = 0; i < 10; i++)
	{
		p1 = new Patch();
		p1->half_height = patch_halfheight; 
		p1->half_width = patch_halfwidth; 
		
		p1->dx = f->getXTopLeft() + (i * patch_width) + (patch_halfwidth) - centre_x; // might need to subtract more from here, so we get the middleo f the patch instead of the corner of it.
		p1->dy = f->getYTopLeft() + (patch_halfheight) - centre_y;
		
		p2 = new Patch();
		p2->half_height = patch_halfheight;
		p2->half_width = patch_halfwidth; 
		p2->dx = f->getXTopLeft() + (i * patch_width) + patch_halfwidth - centre_x;
		p2->dy = f->getYTopLeft() + 3*patch_halfheight - centre_y;



		f->patches.push_back(p1);
		f->patches.push_back(p2);
	}

	// horizontal patches
	patch_width = (int)floor((double)width/2) - 1;
	patch_halfwidth = (int)floor((double)patch_width/2);

	patch_height = (int)floor((double)height/10) - 1;
	patch_halfheight = (int)floor((double)patch_height/2);

	for (int i = 0; i < 10; i++)
	{
		p1 = new Patch();
		p1->half_height = patch_halfheight;
		p1->half_width = patch_halfwidth;

		p1->dx = f->getXTopLeft() + patch_halfwidth - centre_x;
		p1->dy = f->getYTopLeft() + (i * patch_height) + patch_halfheight - centre_y;

		p2 = new Patch();
		p2->half_height = patch_halfheight;
		p2->half_width = patch_halfwidth;
		p2->dx = f->getXTopLeft() + 3*patch_halfwidth - centre_x;
		p2->dy = f->getYTopLeft() + (i * patch_height) + patch_halfheight - centre_y;

		f->patches.push_back(p1);
		f->patches.push_back(p2);
	}
	return;
}


// getBinForEachPixel - routine which bins the image I and returns the result
// in bin_mat. 
void FragTrack::getBinForEachPixel(cv::Mat &I, cv::Mat *bin_mat)
{
	double bin_width = floor(256. / (double)(num_bins));
	
	for (int row=0; row<bin_mat->rows; row++)
	for (int col=0; col<bin_mat->cols; col++)
	{
		int b = (int)(floor ( I.at<unsigned char>(row,col) / bin_width ));
		if ( b > (num_bins - 1) ) 
		{
			b = num_bins - 1;
		}
		bin_mat->at<float>(row,col) = b;
	}
	return;
}

//
// buildIntegralHistogram compute integral histogram. Also possible to use OpenCV's 
// routine, however there is a difference in the size of matrices returned

bool FragTrack::buildIntegralHistogram(cv::Mat &I ,vector <cv::Mat*> &integral_histogram)
{
	//reset integral_histogram matrices
	vector < cv::Mat* >::iterator it;
	for ( it = integral_histogram.begin() ; it != integral_histogram.end() ; it++ ) 
	{
		(*(*it)) = cv::Mat::zeros((*it)->rows, (*it)->cols, (*it)->type()); // set each integral histogram piece to the zero matrix.
	}


	cv::Mat *curr_bin_mat = new cv::Mat(I.rows, I.cols, CV_32F);
	getBinForEachPixel(I, curr_bin_mat);

	// fill matrices
	int i , j , currBin, count;
	double vup , vleft , vdiag, z;

	for ( i = 0 ; i < I.rows ; i++ ) 
	{
		for ( j = 0 ; j < I.cols ; j++ )
		{
			currBin = curr_bin_mat->at<float>(i,j);

			for ( it = integral_histogram.begin() , count = 0 ; it != integral_histogram.end() ; it++ , count++ ) 
			{
				if ( i == 0 ) // no up
				{
					vup = 0;
					vdiag = 0;
				} 
				else 
				{
					vup = (*it)->at<float>(i - 1, j);
				}
				
				if ( j == 0 ) // no left
				{
					vleft = 0;
					vdiag = 0;
				} 
				else 
				{
					vleft = (*it)->at<float>(i, j - 1);
				}

				if ( i > 0 && j > 0 ) // diag exists
				{
					vdiag = (*it)->at<float>(i - 1, j - 1);
				}

				//set cell value
				z = vleft + vup - vdiag;
				if ( currBin == count ) 
					z++;
				(*it)->at<float>(i, j) = z;

			}// next it
		}// next j
	}// next i

	curr_bin_mat->release();
	return true;
}


// computeHistogram - uses the integral histogram data structure to quickly compute
// a histogram in a rectangular region
bool FragTrack::computeHistogram(int tl_y , int tl_x , int br_y , int br_x , vector < cv::Mat* >& iiv , vector < double >& hist)
{
	hist.clear();
	double left , up , diag;
	double z, sum = 0;

	for (auto it = iiv.begin(); it != iiv.end(); it++)
	{
		if (tl_x == 0) 
		{
			left = 0;
			diag = 0;
		} 
		else 
		{
			left = (*it)->at<float>(br_y, tl_x - 1);
		}

		if (tl_y == 0)
		{
			up = 0;
			diag = 0;
		} 
		else 
		{
			up = (*it)->at<float>(tl_y - 1, br_x);
		}

		if ( tl_x > 0 && tl_y > 0 ) 
		{
			diag = (*it)->at<float>(tl_y - 1, tl_x - 1);
		}

		z = (*it)->at<float>(br_y, br_x) - left - up + diag;
		//cout << "left = " << left << " and up = " << up << " and diag = " << diag << endl;
		//cout << "z = " << z << endl;
		hist.push_back(z);
		sum += z;
	}

	// normalize histogram
	for (auto it2 = hist.begin(); it2 != hist.end(); it2++) 
	{
		(*it2) /= sum;
	}
	return true;
}


//
// compareHistograms - 
// Uses a variation of the  Kolmogorov-Smirnov statistic to compare two
// histograms. For one-dimensional data this is exactly equivalent to EMD
// but much faster of course
// 
// Amit January 21'st 2008
//

double FragTrack::compareHistograms(vector <double> &h1 , vector <double> &h2)
{
	double sum = 0;
	double cdf1 = 0;
	double cdf2 = 0;
	double z;
	double ctr = 0;
	vector < double >::iterator it1 , it2;

	for ( it1 = h1.begin() , it2 = h2.begin() ; it1 != h1.end() , it2 != h2.end() ; it1++ , it2++ ) {
		cdf1 += (*it1);
		cdf2 += (*it2);

		z = cdf1 - cdf2;
		sum += abs(z);
		ctr++;
	}
	return (sum/ctr);
}

// computes the votes map associated with a single patch
void FragTrack::computeSinglePatchVotes (Patch *p , FaceTemplate *f, vector <double> &reference_hist,
										   int min_row, int min_col,
										   int max_row, int max_col,
										   cv::Mat *votes, int &min_r, int &min_c,
										   int &max_r, int &max_c, FaceTemplate *ref)
{
	int M = (*IIV_I.begin())->rows;
	int N = (*IIV_I.begin())->cols;
	int minx , maxx , miny, maxy;

	//compute left margin
	if ( p->half_width > p->dx ) 
	{
		minx = p->half_width;
	} 
	else
		minx = p->dx;

	//compute right margin
	if ( p->dx < -p->half_width ) 
	{
		maxx = N - 1 + p->dx;
	} 
	else
		maxx = N - 1 - p->half_width;

	//compute up margin
	if ( p->half_height > p->dy ) 
	{
		miny = p->half_height;
	} 
	else
		miny = p->dy;

	//compute bottom margin
	if ( p->dy < -p->half_height ) 
	{
		maxy = M - 1 + p->dy;
	} 
	else
		maxy = M - 1 - p->half_height;

	//
	// patch center (y,x) votes for the target center at (y - dy,x - dx)
	// we want votes only in the range min/max-row/col
	// we now enforce it:
	//      

	if (miny < min_row + p->dy) {miny = min_row + p->dy;}
	if (maxy > max_row + p->dy) {maxy = max_row + p->dy;}
	if (minx < min_col + p->dx) {minx = min_col + p->dx;}
	if (maxx > max_col + p->dx) {maxx = max_col + p->dx;}

	for (int row = 0; row < votes->rows; row++)
	{
		for (int col = 0; col < votes->cols; col++)
		{
			votes->at<float>(row,col) = 1000.0;
		}
	}

	//////// ignoring all of this above!
	int x , y;
	double z = 0;
	double sum_z = 0;
	double t = 0;
	vector < double > curr_hist;

	int min_x = -search_radius;
	int max_x = search_radius;
	int min_y = -search_radius;
	int max_y = search_radius;

	
	unsigned col = 0;
	for (x = min_x; x <= max_x; x++) 
	{
		unsigned row = 0;
		for (y = min_y; y <= max_y; y++) 
		{
			int tl_x = f->getXCenter() + p->dx - p->half_width + x;
			int tl_y = f->getYCenter() + p->dy - p->half_height + y;
			int br_x = f->getXCenter() + p->dx + p->half_width + x;
			int br_y = f->getYCenter() + p->dy + p->half_height + y;

			if (!((tl_x < 0) // over the left boundary 
				|| (br_x >= (*IIV_I.begin())->cols) // over the right boundary
				|| (tl_y < 0) // over top boundary
				|| (br_y >= (*IIV_I.begin())->rows) // over bottom boundary
				))
			{
				computeHistogram(tl_y, tl_x, br_y, br_x, IIV_I, curr_hist);

				//
				// compare the two histograms
				//
				z = compareHistograms(reference_hist, curr_hist);
				if (x == 0 && y == 0)
				{
					/*cout << "refrence hist from previous frame! " << endl << "------------------" << endl << endl;
					for (int index = 0; index < reference_hist.size(); index++)
					{
						cout << reference_hist[index] << endl;
					}

					cout << endl << "current hist from current frame! " << endl << "------------------" << endl << endl;
					for (int index = 0; index < curr_hist.size(); index++)
					{
						cout << curr_hist[index] << endl;
					}

					///// recompute histogram on reference image.
					buildIntegralHistogram(&(ref->getTemplate()), IIV_I);
					computeHistogram(0, 0, ref->patches[0]->half_height*2, ref->patches[0]->half_width*2, IIV_I, curr_hist);
					cout << endl << "new hist from old frame! " << endl << "------------------" << endl << endl;
					for (int index = 0; index < curr_hist.size(); index++)
					{
						cout << curr_hist[index] << endl;
					}*/

				}

				// now the votemap is not the whole image but only the portion between
				// min-max row-col
				// so y-dy = min_row --> vote for index = 0
				// 

				votes->at<float>(row, col) = z;
			}
			row++;
		}
		col++;
	}

	//
	// return the region where votes were added
	//

	min_c = minx - p->dx;
	max_c = maxx - p->dx;
	min_r = miny - p->dy;
	max_r = maxy - p->dy;
	return;
}


//
// computeAllPatchVotes - runs on all patches and computes each one's vote map
// Then combines all the votes maps (robustly) to a single vote map
void FragTrack::computeAllPatchVotes(FaceTemplate *current_face,
										 FaceTemplate *reference,
										 int img_height, int img_width,
   										 int min_row, int min_col,
										 int max_row, int max_col , cv::Mat* combined_vote,
										 vector<int>& x_coords,
										 vector<int>& y_coords,
										 vector<double>& patch_scores) 
{
	cv::Mat* current_votemap;
	vector <double> *reference_patch_histogram;

	patch_vote_maps.clear();
	x_coords.clear();
	y_coords.clear();
	patch_scores.clear();

	vector<int> vote_regions_min_row; 
	vector<int> vote_regions_min_col;
	vector<int> vote_regions_max_row;
	vector<int> vote_regions_max_col;

	int minx, miny, maxx, maxy;

	int vm_width = max_col - min_col + 1;
	int vm_height = max_row - min_row + 1;

	int i = 0;
	for (auto patch = current_face->patches.begin() ; patch != current_face->patches.end() ; ++patch) 
	{
		// current_votemap holds votes across the whole search radius.  +- 7 in width+height if search radius is 7.
		current_votemap = new cv::Mat(2 * search_radius + 1, 2 * search_radius + 1, CV_32F);

		// compute current patch histogram
		reference_patch_histogram = reference->patch_histograms[i]; 

		int track_center_x = (int)floor((double)current_face->getWidth() / 2.0 );
		int track_center_y = (int)floor((double)current_face->getHeight() / 2.0 );

		int p_cx = track_center_x + (*patch)->dx;
		int p_cy = track_center_y + (*patch)->dy;

		// for a single patch, slides along the search radius window and computes histogram dissimilarities for each hypothesis
		// basically, find the most similar patch (by histogram distance) to current_patch_histogram, the i'th histogram from the reference.
		// returns resultant votes in current_votemap.
		//cout << "reference is located at " << reference->patches[i]->dx << ", " << reference->patches[i]->dy << endl;
		//cout << "current patch is located at " << (*patch)->dx << ", " << (*patch)->dy << endl;
		computeSinglePatchVotes ( (*patch) , current_face, *reference_patch_histogram , min_row, min_col,
							          max_row, max_col, current_votemap, miny, minx, maxy, maxx, reference);


		patch_vote_maps.push_back(current_votemap); // curr_vm is a matrix of the results of each "sliding window" test to compare histogram against neighbours.
		vote_regions_min_row.push_back(miny);
		vote_regions_min_col.push_back(minx);
		vote_regions_max_row.push_back(maxy);
		vote_regions_max_col.push_back(maxx);
				
		//
		// find the position based on this patch:
		//

		//cv::Mat debug(current_face->getTemplate());

		//int x_tl = (current_face->getXCenter() - current_face->getXTopLeft()) + (*patch)->dx - (*patch)->half_width;// - track->getXTopLeft();
		//int y_tl = current_face->getYCenter() + (*patch)->dy - (*patch)->half_height - current_face->getYTopLeft();

		//cv::Rect patch_rect(x_tl, y_tl, (*patch)->half_width*2, (*patch)->half_height*2); 

		//cv::rectangle(debug, patch_rect, cv::Scalar(255, 255, 255), 1);
		////cv::namedWindow("currentframe");
		//cv::imshow("currentframe", debug);

		//cv::Mat prev(reference->getTemplate());
		//x_tl = (reference->getXCenter() - reference->getXTopLeft()) + reference->patches[i]->dx - reference->patches[i]->half_width;
		//y_tl = reference->getYCenter() + reference->patches[i]->dy - reference->patches[i]->half_height - reference->getYTopLeft();
		//cv::Rect prev_rect(x_tl, y_tl, reference->patches[i]->half_width*2, reference->patches[i]->half_height*2);

		//cv::rectangle(prev, prev_rect, cv::Scalar(255, 255, 255), 1);
		//cv::namedWindow("prevframe");
		//cv::imshow("prevframe", prev);
		//cout << "patching " << endl;
		//cv::waitKey();

		cv::Point min_loc;
		cv::Point max_loc;
		double minval;
		double maxval;

		// min_loc will hold the location of the minimum in the current_votemap matrix.
		// the center value of the votemap = stay in the same spot
		// the values otherwise are all relative to that...
		cv::minMaxLoc(*current_votemap, &minval, &maxval, &min_loc, &max_loc);
		
		int midpoint_x = floor((double)current_votemap->cols/2);
		int midpoint_y = floor((double)current_votemap->rows/2);
		int new_dx = reference->patches[i]->dx + min_loc.x - midpoint_x;
		int new_dy = reference->patches[i]->dy + min_loc.y - midpoint_y;

		int plot_x = (current_face->getXCenter() - current_face->getXTopLeft()) + new_dx;// - (*patch)->half_width;
		int plot_y = (current_face->getYCenter() - current_face->getYTopLeft()) + new_dy;// - (*patch)->half_height;
		//cout << "circle at x, y location " << plot_x << ", " << plot_y << endl;
		//cv::Point voted_point(plot_x, plot_y);
		//cv::circle(debug, voted_point, 3, cv::Scalar(255,255,255));
		//cv::imshow("currentframe", debug);
		//cout << "midpoint x = " << midpoint_x << endl;
		//cout << min_loc << endl;
		//cout << "circling " << endl;
		//cout << minval << endl;
		//cv::waitKey();

		x_coords.push_back(min_col+min_loc.x); // these are the x,y coords of the hypothesis.
		y_coords.push_back(min_row+min_loc.y);
		patch_scores.push_back(minval); // minval is the score of that hypothesis
	}  // next patch

	//
	// combine patch votes - using a quantile based score makes this combination
	// robust to occlusions
	//
	combineVoteMaps(patch_vote_maps, combined_vote, min_col, min_row, max_col, max_row, current_face);

	// combined_vote is a matrix of the size of the sliding window radius, 
	// where each value contains the probability that that location is the current location.

	//
	// release stuff 
	//

	vector< cv::Mat* >::iterator vm_it;
	for (vm_it = patch_vote_maps.begin(); vm_it != patch_vote_maps.end(); vm_it++)
	{
		(*vm_it)->release();
	}
	patch_vote_maps.clear();
	i++;
	return;
}

//
// combineVoteMaps - at each hypothesis sorts the score given by each patch
// and takes the Q'th quantile as the score. This ignores outlier scores contributed
// by patches affected by occlusions for example
//


// ok, new combinevotemaps.  the vote_maps are 2*search_radius + 1 by 2*search_radius + 1.  middle value in the matrix means object did not move.
// each of these values is not actually a pixel, but a distance relative to the previous frame.
void FragTrack::combineVoteMaps(vector< cv::Mat* > &vote_maps, cv::Mat *V, int min_col, int min_row, int max_col, int max_row, FaceTemplate *f)
{
	int M = (vote_maps[0])->rows;
	int N = (vote_maps[0])->cols; // this doesn't need to be cols because it's just 2*search_radius + 1.  they're the same in x and y.

	int Z = vote_maps.size(); 
	
	vector< double > Fv;

	int Q_index = (int) floor(((double) Z) / 4.0);
	//Q_index = 4;



	for (int row = 0; row < M; row++)
	{
		for (int col = 0; col < N; col++)
		{

			Fv.clear();

			for (int p = 0; p < Z; p++)
			{
				if (row >= vote_maps[p]->rows || col >= vote_maps[p]->cols)
				{
					Fv.push_back(1000.0);
				}
				else
				{
					double z = vote_maps[p]->at<float>(row,col);
					Fv.push_back(z);
				}
			}

			std::sort(Fv.begin(),Fv.end());
		
			double Q = Fv[Q_index];

			// a quarter of the vote maps vote for this patch to be at hypothesis i,j with score Q.
			V->at<float>(row, col) = Q;
		}
	}

	return;
}

//
// computeTrackDisplacement - the main routine. 
// Searches for the template in the region defined
// by min_row,min_col and max_row,max_col.
// Returns the result in result_y,result_x and with it its associated score
void FragTrack::computeTrackDisplacement(FaceTemplate *track,
							 int img_height, int img_width,
							 int &result_y, int &result_x, 
							 int &result_height, int &result_width,
							 double &score,
							 vector<int> &x_coords,
							 vector<int> &y_coords)
{
	// need to check these scales...
	// standard.
	// +- 6% width (6 is arbitrary)
	// +- 6% height
	// +- 6% w and h
	
	// right now scale changes are commented out here.
	// to enable fixed scale changes...
	// set scale_sizes to 3.
	// uncomment the suffix of the arrays below it up until the first 3 values for each.

	const int scale_sizes = 1;
	int size_to_grow_upwards[scale_sizes] = {0};//, (int)track->getHeight()*.05, (-1)*(int)track->getHeight()*.05};//, 0, 0};
	int size_to_grow_downwards[scale_sizes] = {0};//, (int)track->getHeight()*.05, (-1)*(int)track->getHeight()*.05};//, 0, 0};
	int size_to_grow_left[scale_sizes] = {0};//, (int)track->getWidth()*.05, (-1)*(int)track->getWidth()*.05};//, (int)track->getWidth()*.05, (-1)*(int)track->getWidth()*.05};
	int size_to_grow_right[scale_sizes] = {0};//, (int)track->getWidth()*.05, (-1)*(int)track->getWidth()*.05};//, (int)track->getWidth()*.05, (-1)*(int)track->getWidth()*.05};


	double scores[scale_sizes];
	int result_ys[scale_sizes];
	int result_xs[scale_sizes];

	for (unsigned i = 0; i < scale_sizes; i++)
	{
		// allocate hypothesis size of the track.
		int x = track->getXTopLeft() - size_to_grow_left[i];
		int y = track->getYTopLeft() - size_to_grow_upwards[i];
		int height = track->getHeight() + 2*size_to_grow_downwards[i];
		int width = track->getWidth() + 2*size_to_grow_right[i];

		// create a new FaceTemplate based on this hypothesis.  
		cv::Mat tmp = getROI(img, x, y, width, height);
		FaceTemplate *hypothesis_track = new FaceTemplate(tmp, img, x, y, false, false); 

		
		// make sure we're in bounds.
		int min_row = hypothesis_track->getYCenter() - search_radius;
		int min_col = hypothesis_track->getXCenter() - search_radius;
		int max_row = hypothesis_track->getYCenter() + search_radius;
		int max_col = hypothesis_track->getXCenter() + search_radius;

		if (min_row - hypothesis_track->getHeight()/2 < 0) {min_row = hypothesis_track->getHeight()/2;}
		if (min_col - hypothesis_track->getWidth()/2 < 0) {min_col = hypothesis_track->getWidth()/2;}
		if (max_row >= img_height) {max_row = img_height - 1;}
		if (max_col >= img_width) {max_col = img_width - 1;}

		// out of bounds. remove.
		if (max_row - min_row + 1 <= 0 || max_col - min_col + 1 <= 0)
		{
			score = DELETE_TRACK;
			return;
		}

		vector<double> patch_scores;
		cv::Mat* combined_vote = new cv::Mat(2 * search_radius + 1, 2 * search_radius + 1, CV_32F);
		
		// build patches.
		buildTemplatePatchHistograms(hypothesis_track);

		// combined_vote is a matrix the size of the sliding window radius.  
		// each entry is the probability that that location is the next tracking location.
		computeAllPatchVotes(hypothesis_track, track, img_height, img_width,
		                      min_row, min_col, max_row, max_col,
							  combined_vote,
							  x_coords,
							  y_coords,
							  patch_scores);

		
		// find the "winning" track location for this scale.
		// The "winning" location is characterized by the center of the track.
		cv::Point min_loc, max_loc;
		double minval, maxval;
		cv::minMaxLoc(*combined_vote, &minval, &maxval, &min_loc, &max_loc);

		int cx = min_loc.x;
		int cy = min_loc.y;
		result_y = track->getYCenter() + (min_loc.y - search_radius);
		result_x = track->getXCenter() + (min_loc.x - search_radius);
		score = minval;

		scores[i] = score;
		result_ys[i] = result_y;
		result_xs[i] = result_x;

		combined_vote->release();
		delete hypothesis_track;
	}

	// pick the optimal scale, based on score.
	double winning_score = scores[0];
	int winner = 0;
	for (int i = 1; i < scale_sizes; i++)
	{
		if (scores[i] < winning_score)
		{
			winner = i;
			winning_score = scores[i];
		}
	}

	result_y = result_ys[winner];
	result_x = result_xs[winner];
	result_height = track->getHeight() + 2 * size_to_grow_downwards[winner]; 
	result_width = track->getWidth() + 2 * size_to_grow_right[winner];
	score = scores[winner];
	return;
}

//
// updateTemplate - updates the template's position and makes sure it stays
// inside the image
//
void FragTrack::updateTrack(FaceTemplate *face, int new_height,int new_width,
								 int new_cy, int new_cx, double scale_factor,
								 cv::Mat &I)
{
	static int step = 0;
	int t_halfw = (int)floor((double)new_width / 2.0 );
	int t_halfh = (int)floor((double)new_height / 2.0 );
	
	if (new_cx - t_halfw < 0)
	{
		new_cx = t_halfw;
	}

	if (new_cy - t_halfh < 0)
	{
		new_cy = t_halfh;
	}
	cv::Mat tmp = getROI(I, new_cx - t_halfw, new_cy - t_halfh, new_width, new_height); 
	cv::Mat tmp2 = tmp.clone(); // not sure if tmp will disappear once I goes out of scope, so copy it just in case...
	face->updateTemplate(tmp2, I, new_cx - t_halfw, new_cy - t_halfh);
	if (1)//step % 4 == 0)
	{
		
		// build track's patches.
		buildTemplatePatchHistograms(face);
	}

	// make sure we stay inside the image
	if (face->getXTopLeft() < 0) {face->setX(0);}
	if (face->getYTopLeft() < 0) {face->setY(0);}

	if (face->getYTopLeft() > I.rows - face->getHeight())
	{
		face->setY(I.rows - face->getHeight());
	}

	if (face->getXTopLeft() > I.cols - face->getWidth())
	{
		face->setX(I.cols - face->getWidth());
	}

	step++;
	return;
}


// trackFrame - the outside interface after tracker is initialized. Call it with the
// current frame and get the resultant tracks.
std::vector<Track> FragTrack::trackFrame(cv::Mat &frame)
{	
	std::vector<Track> retval;
	img = frame;

	// build this image's integral histogram.  
	// Use this instead of the actual image, 
	// for efficiency.
	buildIntegralHistogram(frame, IIV_I);
	
	vector<int> x_coords;
	vector<int> y_coords;
	int new_yM, new_xM;
	int new_height, new_width;
	double score_M;

	for (unsigned i = 0; i < tracks.size(); ++i)
	{
		// find out where the track has moved to.
		computeTrackDisplacement(tracks[i],
					  frame.rows, frame.cols,
					  new_yM, new_xM, 
					  new_height, new_width,
					  score_M,
					  x_coords, y_coords);

		if (score_M == DELETE_TRACK)
		{
			for (vector<FaceTemplate *>::iterator it = tracks.begin(); it != tracks.end(); ++it)
			{
				if ((*it)->getId() == tracks[i]->getId())
				{
					tracks.erase(it);
					break;
				}
			}
			continue;
		}
		
		updateTrack(tracks[i], new_height, new_width, new_yM, new_xM, 1, frame);

		// set as a Track so GUI layer understands.
		Track t;
		t.height = tracks[i]->getHeight();
		t.width = tracks[i]->getWidth();
		t.id = tracks[i]->getId();
		t.x = tracks[i]->getXTopLeft();
		t.y = tracks[i]->getYTopLeft();

		retval.push_back(t);
	}

	return retval;
}

void FragTrack::init()
{
}