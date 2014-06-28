#pragma once
#include "SLIC.h"

#include <map>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include "LineDef.h"

class MySLIC : public SLIC
{
private:
	uints	m_Buff;
public:
	MySLIC(const cv::Mat& img);
	~MySLIC(void);
public:
	struct Pixel{int x,y;};
	struct BoxBoundary{
		int max_x,max_y;
		int min_x,min_y;
		BoxBoundary():max_x(0),max_y(0),min_x(9999),min_y(9999){}
	};
	typedef std::vector<Pixel> Pixels;
	typedef std::map<int,Pixels> PixelsMap;

	struct LABColor{
		double l,a,b;
	};

	struct SLICLabel{
		int		 index;
		Pixel	 seed_xy;
		double	 seed_Gx;
		double	 seed_Gy;
		LABColor seed_lab;

		Pixels	pixels;
		BoxBoundary boundary;
		std::vector<int> neighbors;
		PixelsMap n_boundary;
	};
	typedef std::vector<SLICLabel> SLICLabels;
	
public:
	//============================================================================
	// Superpixel segmentation for a given step size (superpixel size ~= step*step)
	//============================================================================
	void DoSuperpixelSegmentation_ForGivenSuperpixelSize(
		int*&						labels_map,
		SLICLabels&					labels,
		int&						numlabels,
		const int&					superpixelsize,
		const double&               compactness);
	//============================================================================
	// Superpixel segmentation for a given number of superpixels
	//============================================================================
	void DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels(
		int*&						labels_map,
		SLICLabels&					labels,
		int&						numlabels,
		const int&					K,			 //required number of superpixels
		const double&               compactness);//10-20 is a good value for CIELAB space
protected:
	//============================================================================
	// Pick seeds for superpixels when step size of superpixels is given.
	//============================================================================
	void GetSeeds_ForGivenStepSize(
		std::vector<double>&				kseedsl,
		std::vector<double>&				kseedsa,
		std::vector<double>&				kseedsb,
		std::vector<double>&				kseedsgx,
		std::vector<double>&				kseedsgy,
		std::vector<double>&				kseedsx,
		std::vector<double>&				kseedsy,
		const int&							STEP);
	//============================================================================
	// The main SLIC algorithm for generating superpixels
	//============================================================================
	void PerformSuperpixel(
		std::vector<double>&				kseedsl,
		std::vector<double>&				kseedsa,
		std::vector<double>&				kseedsb,
		std::vector<double>&				kseedsgx,
		std::vector<double>&				kseedsgy,
		std::vector<double>&				kseedsx,
		std::vector<double>&				kseedsy,
		int*&								klabels,
		const int&							STEP,
		const double&						compactness = 10.0);
	//============================================================================
	// Post-processing of SLIC segmentation, to avoid stray labels.
	//============================================================================
	void EnforceLabelConnectivity(
		const int*					labels,
		int*&						nlabels,	//input labels that need to be corrected to remove stray labels
		std::vector<int>&           map,		//new label map to old one
		int&						numlabels,	//the number of labels changes in the end if segments are removed
		const int&					K);			//the number of superpixels desired by the user
public:
	void FindNeighbors(
		SLICLabels&				labels,
		int*&				labels_map);
	void FindBoxBoundary(
		SLICLabels&				labels);

	void GetRGBtoLABConversion(
		const unsigned int*			ubuff,
		LABColor*&					xyzbuff);

	double MaxDistanceofColor(
		SLICLabel&			label
		);

	double MaxDistanceofCenterAndSeed(
		SLICLabel&			label
		);

	double AvgLab(
		SLICLabel&			label
		);

	double VarLab(
		SLICLabel&			label
		);

	void ExtractLabel(
		SLICLabel&					label,
		unsigned int*				label_img
	);

	double GetDisLab(int x1,int y1, int x2, int y2);


	double GetDisGxy(int x1,int y1, int x2, int y2);
};

