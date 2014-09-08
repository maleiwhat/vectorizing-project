#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Edge.h"
#include "Patch.h"
#include "LineFragment.h"
#include "PatchSpline.h"
#include "ImageSpline.h"
#include "ColorConstraintMathModel.h"

struct WeightData
{
	WeightData(Vector2& p, int w): pos(p), weight(w)    {}
	Vector2 pos;
	int  weight;
	bool operator<(const WeightData& wd)
	{
		return weight < wd.weight;
	}
};
typedef std::vector<WeightData> Weights;

extern Weights wm_init;
extern Weights wm_init_cross;
extern Weights wm_init2;

// ÂX´²
void    Dilation(cv::Mat& image, int dilation_elem = 2, int dilation_size = 2);
// ²Ó¤Æ
void    Erosion(cv::Mat& image, int erosion_elem, int erosion_size);
void	FloodFillReColor(cv::Mat& image);
void    S2FloodFill(int& cc, cv::Mat& image, cv::Mat& mask01, cv::Mat mask02,
					int range, int x, int y, CvPatchs& out_array, int dilation = 0);
void    S2FloodFill(cv::Mat& image, cv::Mat& mask01, cv::Mat mask02, int range,
					int x, int y, int dilation = 0, int erosion = 0);
void    S3FloodFill(int& cc, cv::Mat& image, cv::Mat& mask01, int x, int y, CvPatchs& out_array);
void    S3FloodFill(int& cc, cv::Mat& image, cv::Mat& mask01, int x, int y);
void    S3_1FloodFill(int& cc, cv::Mat& image, cv::Mat& mask01, int x, int y, 
					  CvPatchs& out_array, cv::Mat& ori_image, ColorConstraints& ccms,
					  cv::Mat& out);
void    S3_2FloodFill(int& cc, cv::Mat& image, cv::Mat& mask01, int x, int y, 
					  CvPatchs& out_array, cv::Mat& ori_image, ColorConstraints& ccms,
					  cv::Mat& out);
void    S4FloodFill(cv::Mat& image, cv::Mat& mask01, int range,
					int x, int y);
void    S5FloodFill(int& cc, cv::Mat& image, cv::Mat& mask01, cv::Mat mask02,
					int range, int x, int y, CvPatchs& out_array, int dilation,
					cv::Mat image_orig, cv::Mat out);
void    LineFloodFill(cv::Mat& image, cv::Mat& mask01, int& cc, int x, int y);

void    S6FloodFill(cv::Mat& image, cv::Mat& mask01, int& cc, int x, int y);
void    S7FloodFill(cv::Mat& image, cv::Mat& mask01, int& cc, int x, int y, int minArea);
void    S8FloodFill(cv::Mat& image, cv::Mat& mask01, int& cc, int x, int y);
void    S9FloodFill(cv::Mat& image, cv::Mat& mask01, int& cc, int x, int y, int chipsize,
					cv::Mat& out_chips_mask);

CvPatchs S2GetPatchs(const cv::Mat& image, int dilation = 0, int erosion = 0);
CvPatchs S2_1GetPatchs(const cv::Mat& image);
CvPatchs S2_2GetPatchs(const cv::Mat& image, cv::Mat& ori_image, ColorConstraints& ccms,
					   cv::Mat& out);
CvPatchs S2_3GetPatchs(const cv::Mat& image, cv::Mat& ori_image, ColorConstraints& ccms,
					   cv::Mat& out);
ImageSpline S3GetPatchs(cv::Mat& image0, double BlackRegionThreshold, cv::Mat& image1);
ImageSpline S4GetPatchs(const cv::Mat& image0, int dilation, int erosion);
ImageSpline Sx1GetPatchs(cv::Mat& image0);

ImageSpline GetImageSpline(CvPatchs& patchs, const Lines& lines,
						   cv::Mat lineImage);
ImageSpline GetImageSpline(CvPatchs& patchs);
Line CvPointToLine(const CvLine& cvps);

PatchLines GetPatchSplines(CvPatchs& patchs, cv::Mat& patchImage);
Lines GetAllLineFromLineImage(cv::Mat& lineImage);
void    FixHole(cv::Mat& patchImage);
void    FillSmallHole(cv::Mat& patchImage);
cv::Mat MakeColorLineImage(const cv::Mat& image0, const Lines& lines);
void DrawColorLineImage(cv::Mat& image0, const Lines& lines);
void DrawWhiteLineImage(cv::Mat& image0, const Lines& lines);
cv::Mat MakeLineImage(const cv::Mat& image, const Lines& lines);
cv::Mat MakeLineImage(const cv::Mat& image, Patch& patch);

inline bool CorrectPosition(cv::Mat& image, int x, int y)
{
	if (x >= 0 && y >= 0 && x < image.cols && y < image.rows)
	{
		return true;
	}
	return false;
}

void ClearEdge(cv::Mat& image);
void AddEdge(cv::Mat& image);
void EdgeLink_LineFragment(cv::Mat& image, Line& now_line, int now_value);
void AddCathetus(CvPatchs& cvps);
void AddCathetus(CvLine& cps);
void AddCathetus(Lines& cvps);
void AddCathetus(Line& cps);

struct ColorPtr
{
	ColorPtr() {}
	ColorPtr(cv::Vec3f* _c, int _x, int _y)
		: c(_c), x(_x), y(_y)
	{}
	cv::Vec3f* c;
	int x, y;
};
typedef std::vector<ColorPtr> Vec3fptrs;
typedef std::vector<Vec3fptrs> Vec3fptrs2d;

struct ColorPtrb
{
	ColorPtrb() {}
	ColorPtrb(cv::Vec3b* _c, int _x, int _y)
		: c(_c), x(_x), y(_y)
	{}
	cv::Vec3b* c;
	int x, y;
};
typedef std::vector<ColorPtrb> Vec3bptrs;
typedef std::vector<Vec3bptrs> Vec3bptrs2d;

struct ColorPtrf
{
	ColorPtrf() {}
	ColorPtrf(float* _c, int _x, int _y)
		: c(_c), x(_x), y(_y)
	{}
	float* c;
	int x, y;
	float value() const
	{
		return *c;
	}
	operator float()
	{
		return *c;
	}
};
typedef std::vector<ColorPtrf> floatptrs;
typedef std::vector<floatptrs> floatptrs2d;
void GetMatrixb(int w, int h, Vec3bptrs& ary, int x, int y, cv::Mat& img);
void Collect_Water(cv::Mat src, cv::Mat& dst, int rectw, int recth,
				   double BlackRegionThreshold);

ImageSpline ComputeLines(cv::Mat img, double BlackRegionThreshold);
void DrawCvPatchs(CvPatchs& tmp_cvps, cv::Mat tmp_image2);
ImageSpline S4GetPatchs(const cv::Mat& image0, int dilation, int erosion);
ImageSpline S5GetPatchs(const cv::Mat& image0, const cv::Mat& orig);
Lines S6GetPatchs(const cv::Mat& image0, int dilation, int erosion, cv::Mat& outimg = cv::Mat());
Lines S7GetPatchs(const cv::Mat& image0, const cv::Mat& noline, int dilation, int erosion);
cv::Mat S6GetEngrgy(const cv::Mat& image0, int dilation, int erosion);
cv::Mat MarkDiffence(cv::Mat src, int rectw, int recth);

cv::Mat FixSpaceLine(cv::Mat image, cv::Mat oriImg);
