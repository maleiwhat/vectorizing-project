#ifndef _HKCV_H_
#define _HKCV_H_

#include "HKCV_Utils.h"

namespace HKCV
{
	/// Adjust the brightness and/or contrast of image
	/// 1. Input image must be 8 bits image
	/// 2. Output image has the same type as input image
	/// 3. 1st parameter has range [-1,1]
	/// 4. 2nd parameter has range [-1,1]
	void Brightness_Contrast(const cv::Mat&, cv::Mat&, double, double);

	/// UnSharp Mask Filter
	/// 1. Input image could be any type of image
	/// 2. Output image has the same type as input image
	/// 3. The value of threshold must has the same range of input image
	/// 3. The value of threshold must has the same range of input image
	/// 4. Range of 1st: [0.1,120], range of 2nd: [-10,10], range of 3rd: [0,255]
	void Unsharp_Mask(const cv::Mat&, cv::Mat&, double, double, double, cv::Mat& = cv::Mat());

	/// Color quantization
	/// 1. Input is a CV_32FC3 image with element range [0,1]
	/// 2. 2nd output is a bin index map of each pixel 
	/// 3. 3rd output is a vector of color of bins
	/// 4. 4th output is a vector of frequency of bins
	/// 5. 5th parameter is number of bins
	/// 6. 6th parameter is the percentage of most occurring color to preserve
	int Quantize(const cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&, int, double=0.95);

	/// Find contours and minimum bounding rectangle of a binary image
	void Find_Contours(const cv::Mat&, VecPnt2is&, cv::Rect&);

	/// Conversion between RGB color space and lab(l alpha beta) color space
	void RGB2lab(const cv::Mat&, cv::Mat&);
	void lab2RGB(const cv::Mat&, cv::Mat&);
}

#endif