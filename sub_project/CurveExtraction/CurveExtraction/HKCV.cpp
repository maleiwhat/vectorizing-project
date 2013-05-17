
#define _USE_MATH_DEFINES
#include <cmath>
#include "HKCV.h"

using namespace HKCV;
using namespace cv;

/// Adjust the brightness and/or contrast of image
/// 1. Input image must be 8 bits image
/// 2. Output image has the same type as input image
/// 3. _bright has range [-1,1]
/// 4. _contrast has range [-1,1]
void HKCV::Brightness_Contrast(const Mat& _src, Mat& _dst, double _bright,
                               double _contrast)
{
	/// The input should be channel 1 OR channel 3 with type uchar (8U)
	if ((_src.channels() != 1 && _src.channels() != 3) || _src.depth() != CV_8U)
	{
		CV_Error(CV_StsBadArg,
		         "_src has incorrect channel (not 1 or 3) and type (not CV_8U)");
	}

	/// The lookup table
	Mat     lut_mat(1, 256, CV_8U);
	uchar*  lut_ptr = lut_mat.data;
	double  slant;

	for (int i = 0; i < 256; i++)
	{
		double v = i / 255.0;

		/* apply brightness */
		if (_bright < 0.0)
		{
			v = v * (1.0 + _bright);
		}
		else
		{
			v = v + ((1.0 - v) * _bright);
		}

		slant = tan((_contrast + 1.0) * M_PI_4);
		v = (v - 0.5) * slant + 0.5;
		lut_ptr[i] = saturate_cast<uchar>(cvRound(v * 255.0 + 0.5));
	}

	if (_src.channels() == 3)
	{
		VecMat cvt_chnls;
		split(_src, cvt_chnls);
		// PERFORM IT ON EVERY CHANNEL
		LUT(cvt_chnls[0], lut_mat, cvt_chnls[0]);
		LUT(cvt_chnls[1], lut_mat, cvt_chnls[1]);
		LUT(cvt_chnls[2], lut_mat, cvt_chnls[2]);
		merge(cvt_chnls, _dst);
	}
	else
	{
		//PERFORM IT ON THE CHANNEL
		LUT(_src, lut_mat, _dst);
	}
}

/// UnSharp Mask Filter
/// 1. Input image could be any type of image
/// 2. Output image has the same type as input image
/// 3. The value of threshold must has the same range of input image
/// 4. Range of _r: [0.1,120], range of _a: [-10,10], range of _t: [0,255]
void HKCV::Unsharp_Mask(const cv::Mat& _src, cv::Mat& _dst, double _r,
                        double _a, double _t, cv::Mat& _mask)
{
	double  sigma = _r + 1.0;
	Mat     blur, lct, srpn;
	GaussianBlur(_src, blur, cv::Size(), sigma, sigma);
	lct  = abs(_src - blur) < _t;
	srpn = _src * (1.0 + _a) + blur * (-_a);
	_src.copyTo(srpn, lct);
	srpn.copyTo(_dst, _mask);
}

/// Color quantization
/// 1. img3f is a CV_32FC3 image with element range [0,1]
/// 2. idx1i a bin index map of each pixel
/// 3. _color3fis a vector of color of bins
/// 4. _colorNum is a vector of frequency of bins
/// 5. _binNum is number of bins
/// 6. ratio is the percentage of most occurring color to preserve
int HKCV::Quantize(const Mat& img3f, Mat& idx1i, Mat& _color3f, Mat& _colorNum,
                   int _binNum, double ratio)
{
	/*static const int clrNums[3] = {_binNum, _binNum, _binNum};
	static const float clrTmp[3] = {clrNums[0] - 0.0001f, clrNums[1] - 0.0001f, clrNums[2] - 0.0001f};
	static const int w[3] = {clrNums[1] * clrNums[2], clrNums[2], 1};*/
	int clrNums[3], w[3];
	float clrTmp[3];
	clrNums[0] = _binNum; clrNums[1] = _binNum; clrNums[2] = _binNum;
	clrTmp[0] = clrNums[0] - 0.0001f; clrTmp[1] = clrNums[1] - 0.0001f;
	clrTmp[2] = clrNums[2] - 0.0001f;
	w[0] = clrNums[1] * clrNums[2]; w[1] = clrNums[2]; w[2] = 1;
	CV_Assert(img3f.data != NULL);
	idx1i = Mat::zeros(img3f.size(), CV_32S);
	int rows = img3f.rows, cols = img3f.cols;

	if (img3f.isContinuous() && idx1i.isContinuous())
	{
		cols *= rows;
		rows = 1;
	}

	/// Build color pallet
	std::map<int, int> pallet;

	for (int y = 0; y < rows; y++)
	{
		const float* imgData = img3f.ptr<float>(y);
		int* idx = idx1i.ptr<int>(y);

		for (int x = 0; x < cols; x++, imgData += 3)
		{
			/// map a color to unique integer
			/// color is quantized here too
			idx[x] = (int)(imgData[0] * clrTmp[0]) * w[0] + (int)(imgData[1] * clrTmp[1]) *
			         w[1] + (int)(imgData[2] * clrTmp[2]);
			/// increase color counter map(color, num)
			pallet[idx[x]] ++;
		}
	}

	// Fine significant colors
	int maxNum = 0;
	{
		int count = 0;
		/// create histogram bins with frequency from high to low
		std::vector<std::pair<int, int>> num; // (num, color) pairs in num
		num.reserve(pallet.size());

		for (std::map<int, int>::iterator it = pallet.begin(); it != pallet.end(); it++)
		{
			num.push_back(std::pair<int, int>(it->second,
			                                  it->first));    // (color, num) pairs in pallet
		}

		sort(num.begin(), num.end(), std::greater<std::pair<int, int>>());
		/// choosing more frequently occurring colors and ensure
		/// these colors cover the colors of more than ratio%
		maxNum = (int)num.size();
		int maxDropNum = cvRound(rows * cols * (1 - ratio));

		for (int crnt = num[maxNum - 1].first; crnt < maxDropNum
		        && maxNum > 1; maxNum--)
		{
			crnt += num[maxNum - 2].first;
		}

		/// To avoid very rarely case
		/// Not clear what happens here? (avoid large or small maxNum?)
		maxNum = min(maxNum, 256);

		if (maxNum < 10)
		{
			maxNum = min((int)pallet.size(), 100);
		}

		/// rebuild the pallet (color, index in num)
		pallet.clear();

		for (int i = 0; i < maxNum; i++)
		{
			pallet[num[i].second] = i;
		}

		/// convert signed integer back to three component colors
		vector<Vec3i> color3i(num.size());

		for (unsigned int i = 0; i < num.size(); i++)
		{
			color3i[i][0] = num[i].second / w[0];
			color3i[i][1] = num[i].second % w[0] / w[1];
			color3i[i][2] = num[i].second % w[1];
		}

		/// replace the remaining (1-ratio)% pixels by the
		/// closest colors in the histogram
		for (unsigned int i = maxNum; i < num.size(); i++)
		{
			int simIdx = 0, simVal = INT_MAX;

			for (int j = 0; j < maxNum; j++)
			{
				int d_ij = CvVec3SqrDist(color3i[i], color3i[j]);

				if (d_ij < simVal)
				{
					simVal = d_ij, simIdx = j;
				}
			}

			pallet[num[i].second] = pallet[num[simIdx].second];
		}
	}
	/// compute the average color and frequency of each bin
	_color3f = Mat::zeros(1, maxNum, CV_32FC3);
	_colorNum = Mat::zeros(_color3f.size(), CV_32S);
	Vec3f* color = (Vec3f*)(_color3f.data);
	int* colorNum = (int*)(_colorNum.data);

	for (int y = 0; y < rows; y++)
	{
		const Vec3f* imgData = img3f.ptr<Vec3f>(y);
		int* idx = idx1i.ptr<int>(y);

		for (int x = 0; x < cols; x++)
		{
			idx[x] = pallet[idx[x]];
			color[idx[x]] += imgData[x];
			colorNum[idx[x]] ++;
		}
	}

	for (int i = 0; i < _color3f.cols; i++)
	{
		color[i] /= colorNum[i];
	}

	return _color3f.cols;
}

/// Find contours and minimum bounding rectangle of a binary image
void HKCV::Find_Contours(const cv::Mat& _img, VecPnt2is& _cntrs,
                         cv::Rect& _rect)
{
	/// The input should be channel 1 with type uchar (8U)
	if (_img.channels() != 1 || _img.depth() != CV_8U)
	{
		CV_Error(CV_StsBadArg,
		         "_img has incorrect channel (not 1) and type (not CV_8U)");
	}

	/// Find contours
	Mat tmpImg = _img.clone();
	_cntrs.clear();
	VecVec4i hircy;
	findContours(tmpImg, _cntrs, hircy, CV_RETR_LIST, CHAIN_APPROX_SIMPLE);
	/// Find minimum bounding rectangle
	_rect = boundingRect(_cntrs[0]);

	for (size_t i = 1; i < _cntrs.size(); ++i)
	{
		_rect = _rect | boundingRect(_cntrs[i]);
	}
}

static const float RGB2LMS[] =
{
	0.3811f, 0.5783f, 0.0402f,
	0.1967f, 0.7244f, 0.0782f,
	0.0241f, 0.1288f, 0.8444f
};

static const float LMS2RGB[] =
{
	4.4679f, -3.5873f,  0.1193f,
	-1.2186f,  2.3809f, -0.1624f,
	0.0497f, -0.2439f,  1.2045f
};

/// Conversion between RGB color space and lab(l alpha beta) color space
void HKCV::RGB2lab(const cv::Mat& _bgr, cv::Mat& _lab)
{
	/// The input should be channel 3 with float type
	if (_bgr.channels() != 3 || _bgr.depth() != CV_32F)
	{
		CV_Error(CV_StsBadArg,
		         "_bgr has incorrect channel (not 3) and type (not CV_32F)");
	}

	float C0 = RGB2LMS[0], C1 = RGB2LMS[1], C2 = RGB2LMS[2],
	      C3 = RGB2LMS[3], C4 = RGB2LMS[4], C5 = RGB2LMS[5],
	      C6 = RGB2LMS[6], C7 = RGB2LMS[7], C8 = RGB2LMS[8];
	float sqrt3 = 1.0f / std::sqrt(3.0f);
	float sqrt2 = 1.0f / std::sqrt(2.0f);
	float sqrt6 = 1.0f / std::sqrt(6.0f);
	float D0 = sqrt3, D1 =  sqrt3, D2 = sqrt3,
	      D3 = sqrt6, D4 =  sqrt6, D5 = sqrt6 * -2.0f,
	      D6 = sqrt2, D7 = -sqrt2, D8 = 0.0f;
	_lab.create(_bgr.size(), _bgr.type());
	int rows = _bgr.rows, cols = _bgr.cols;

	for (int y = 0; y < rows; ++y)
	{
		const float* bgr_ptr = _bgr.ptr<float>(y);
		float* lab_ptr = _lab.ptr<float>(y);

		for (int x = 0; x < cols; ++x, bgr_ptr += 3, lab_ptr += 3)
		{
			float pxl_L = bgr_ptr[2] * C0 + bgr_ptr[1] * C1 + bgr_ptr[0] * C2;
			float pxl_M = bgr_ptr[2] * C3 + bgr_ptr[1] * C4 + bgr_ptr[0] * C5;
			float pxl_S = bgr_ptr[2] * C6 + bgr_ptr[1] * C7 + bgr_ptr[0] * C8;

			if (pxl_L <= 1.0e-6f) { pxl_L = 1.0e-6f; }

			if (pxl_M <= 1.0e-6f) { pxl_M = 1.0e-6f; }

			if (pxl_S <= 1.0e-6f) { pxl_S = 1.0e-6f; }

			pxl_L = std::log10(pxl_L);
			pxl_M = std::log10(pxl_M);
			pxl_S = std::log10(pxl_S);
			lab_ptr[0] = pxl_L * D0 + pxl_M * D1 + pxl_S * D2; // l
			lab_ptr[1] = pxl_L * D3 + pxl_M * D4 + pxl_S * D5; // alpha
			lab_ptr[2] = pxl_L * D6 + pxl_M * D7 + pxl_S * D8; // beta
		}
	}
}

void HKCV::lab2RGB(const cv::Mat& _lab, cv::Mat& _bgr)
{
	/// The input should be channel 3 with float type
	if (_lab.channels() != 3 || _lab.depth() != CV_32F)
	{
		CV_Error(CV_StsBadArg,
		         "_lab has incorrect channel (not 3) and type (not CV_32F)");
	}

	float C0 = LMS2RGB[0], C1 = LMS2RGB[1], C2 = LMS2RGB[2],
	      C3 = LMS2RGB[3], C4 = LMS2RGB[4], C5 = LMS2RGB[5],
	      C6 = LMS2RGB[6], C7 = LMS2RGB[7], C8 = LMS2RGB[8];
	float sqrt3 = std::sqrt(3.0f) / 3.0f;
	float sqrt2 = std::sqrt(2.0f) / 2.0f;
	float sqrt6 = std::sqrt(6.0f) / 6.0f;
	float D0 = sqrt3, D1 = sqrt6,       D2 =  sqrt2,
	      D3 = sqrt3, D4 = sqrt6,       D5 = -sqrt2,
	      D6 = sqrt3, D7 = sqrt6 * -2.0f, D8 = 0.0f;
	_bgr.create(_lab.size(), _lab.type());
	int rows = _lab.rows, cols = _lab.cols;

	for (int y = 0; y < rows; ++y)
	{
		float* bgr_ptr = _bgr.ptr<float>(y);
		const float* lab_ptr = _lab.ptr<float>(y);

		for (int x = 0; x < cols; ++x, bgr_ptr += 3, lab_ptr += 3)
		{
			float pxl_L = lab_ptr[0] * D0 + lab_ptr[1] * D1 + lab_ptr[2] * D2;
			float pxl_M = lab_ptr[0] * D3 + lab_ptr[1] * D4 + lab_ptr[2] * D5;
			float pxl_S = lab_ptr[0] * D6 + lab_ptr[1] * D7 + lab_ptr[2] * D8;
			pxl_L = std::pow(10.0f, pxl_L);
			pxl_M = std::pow(10.0f, pxl_M);
			pxl_S = std::pow(10.0f, pxl_S);
			bgr_ptr[2] = pxl_L * C0 + pxl_M * C1 + pxl_S * C2; // B
			bgr_ptr[1] = pxl_L * C3 + pxl_M * C4 + pxl_S * C5; // G
			bgr_ptr[0] = pxl_L * C6 + pxl_M * C7 + pxl_S * C8; // R
		}
	}
}