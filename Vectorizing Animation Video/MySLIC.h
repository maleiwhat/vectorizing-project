#pragma once
#include "SLIC.h"

#include <map>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include "LineDef.h"
#include "ColorConstraint.h"

struct RGBColor
{
	int r, g, b;
	RGBColor(): r(0), g(0), b(0) {}
	RGBColor(int in_r, int in_g, int in_b): r(in_r), g(in_g), b(in_b) {}
	void operator=(const RGBColor& in)
	{
		r = in.r;
		g = in.g;
		b = in.b;
	}
};

struct XYZColor
{
	double x, y, z;
	XYZColor(): x(0.0), y(0.0), z(0.0) {}
	XYZColor(double in_x, double in_y, double in_z): x(in_x), y(in_y), z(in_z) {}
	void operator=(const XYZColor& in)
	{
		x = in.x;
		y = in.y;
		z = in.z;
	}
};

struct LabColor
{
	double l, a, b;
	LabColor(): l(0.0), a(0.0), b(0.0) {}
	LabColor(double in_l, double in_a, double in_b): l(in_l), a(in_a), b(in_b) {}
	void operator=(const LabColor& in)
	{
		l = in.l;
		a = in.a;
		b = in.b;
	}
};
typedef std::vector<LabColor> LabVec;

//============================================================================
// XYZ to sRGB conversion; helper for RGB2LAB()
//============================================================================
XYZColor LAB2XYZ(LabColor& val);
//============================================================================
// sRGB to CIELAB conversion
//============================================================================
RGBColor LAB2RGB(LabColor& val);
//============================================================================
// sRGB to XYZ conversion; helper for RGB2LAB()
//============================================================================
XYZColor RGB2XYZ(RGBColor& val);
//============================================================================
// sRGB to CIELAB conversion (uses RGB2XYZ function)
//============================================================================
LabColor RGB2LAB(RGBColor& val);

class MySLIC : public SLIC
{
private:
	uints   m_Buff;
public:
	MySLIC(const cv::Mat& img);
	~MySLIC(void);
public:
	struct Pixel
	{
		int x, y;
	};
	struct BoxBoundary
	{
		int max_x, max_y;
		int min_x, min_y;
		BoxBoundary(): max_x(0), max_y(0), min_x(9999), min_y(9999) {}
	};
	typedef std::vector<Pixel> Pixels;
	typedef std::map<int, Pixels> PixelsMap;

	struct SLICLabelPatch
	{
		int  pid;
		ints idxs;
	};
	typedef std::vector<SLICLabelPatch> SLICLabelPatchs;

	struct SLICLabel
	{
		int      index;
		Pixel    seed_xy;
		double   seed_Gx;
		double   seed_Gy;
		LabColor seed_lab;

		Pixels  pixels;
		BoxBoundary boundary;
		std::vector<int> neighbors;
		PixelsMap n_boundary;
	};
	typedef std::vector<SLICLabel> SLICLabels;

public:
	ColorConstraints BuildColorConstraints(const SLICLabels& labels, const SLICLabelPatchs& patchs,
										   cv::Mat img);
	void ComputePatchFromColor(const SLICLabels& labels, SLICLabelPatchs& patchs, double threshold);
	//============================================================================
	// Superpixel segmentation for a given step size (superpixel size ~= step*step)
	//============================================================================
	void DoSuperpixelSegmentation_ForGivenSuperpixelSize(
		int*&                       labels_map,
		SLICLabels&                 labels,
		int&                        numlabels,
		const int&                  superpixelsize,
		const double&               compactness);
	//============================================================================
	// Superpixel segmentation for a given number of superpixels
	//============================================================================
	void DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels(
		int*&                       labels_map,
		SLICLabels&                 labels,
		int&                        numlabels,
		const int&                  K,           //required number of superpixels
		const double&               compactness);//10-20 is a good value for CIELAB space
protected:
	double ColorError(const LabColor& c1, const LabColor& c2);
	void AddNeighbors(int idx, SLICLabelPatch& patch, const SLICLabels& labels, ints& marked,
					  double threshold);
	//============================================================================
	// Pick seeds for superpixels when step size of super pixels is given.
	//============================================================================
	void GetSeeds_ForGivenStepSize(
		std::vector<double>&                kseedsl,
		std::vector<double>&                kseedsa,
		std::vector<double>&                kseedsb,
		std::vector<double>&                kseedsgx,
		std::vector<double>&                kseedsgy,
		std::vector<double>&                kseedsx,
		std::vector<double>&                kseedsy,
		const int&                          STEP);
	//============================================================================
	// The main SLIC algorithm for generating superpixels
	//============================================================================
	void PerformSuperpixel(
		std::vector<double>&                kseedsl,
		std::vector<double>&                kseedsa,
		std::vector<double>&                kseedsb,
		std::vector<double>&                kseedsgx,
		std::vector<double>&                kseedsgy,
		std::vector<double>&                kseedsx,
		std::vector<double>&                kseedsy,
		int*&                               klabels,
		const int&                          STEP,
		const double&                       compactness = 10.0);
	//============================================================================
	// Post-processing of SLIC segmentation, to avoid stray labels.
	//============================================================================
	void EnforceLabelConnectivity(
		const int*                  labels,
		int*&
		nlabels,    //input labels that need to be corrected to remove stray labels
		std::vector<int>&           map,        //new label map to old one
		int&
		numlabels,  //the number of labels changes in the end if segments are removed
		const int&                  K);         //the number of superpixels desired by the user
public:
	void FindNeighbors(
		SLICLabels&             labels,
		int*&               labels_map);
	void FindBoxBoundary(
		SLICLabels&             labels);

	void GetRGBtoLABConversion(
		const unsigned int*         ubuff,
		LabColor*&                  xyzbuff);

	double MaxDistanceofColor(
		SLICLabel&          label
	);

	double MaxDistanceofCenterAndSeed(
		SLICLabel&          label
	);

	double AvgLab(
		SLICLabel&          label
	);

	double VarLab(
		SLICLabel&          label
	);

	void ExtractLabel(
		SLICLabel&                  label,
		unsigned int*               label_img
	);

	double GetDisLab(int x1, int y1, int x2, int y2);


	double GetDisGxy(int x1, int y1, int x2, int y2);

};

