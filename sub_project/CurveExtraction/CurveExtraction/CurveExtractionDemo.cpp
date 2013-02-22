/************************************************************************/
/*  This software is developed by Ming-Ming Cheng.				        */
/*       Url: http://cg.cs.tsinghua.edu.cn/people/~cmm/                 */
/*  This software is free fro non-commercial use. In order to use this	*/
/*  software for academic use, you must cite the corresponding paper:	*/
/*      Ming-Ming Cheng, Curve Structure Extraction for Cartoon Images, */
/*      in The 5th Joint Conference on Harmonious Human Machine			*/
/*      Environment (HHME), 2009, pp. 13-20.							*/
/************************************************************************/

#include "stdafx.h"
#include "CmCurveEx.h"
#include "Saliency/FT/HKSaliency_FT.h"
#include "Saliency/HC/HKSaliency_HC.h"
#include "Saliency/LC/HKSaliency_LC.h"
#include "Saliency/SR/HKSaliency_SR.h"
#include <cmath>
#include "math/Vector2.h"
#include "math/Quaternion.h"
using namespace HKCV;

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

float GetLight(const cv::Vec3f& v)
{
	return v[0] * 0.299 + v[1] * 0.587 + v[2] * 0.114;
}
int g_channel = 0;
int g_rect = 1;
float GetLight(const cv::Vec3f* v)
{
	return v[0][0] * 0.299 + v[0][1] * 0.587 + v[0][2] * 0.114;
	//return sqrt(v[0][0] * v[0][0] + v[0][1] * v[0][1] + v[0][2] * v[0][2]);
}

bool LightComparef(const ColorPtrf& lhs, const ColorPtrf& rhs)
{
	return lhs.value() < rhs.value();
}

bool LightCompare(const ColorPtr& lhs, const ColorPtr& rhs)
{
	return GetLight(lhs.c) < GetLight(rhs.c);
}

void GetMatrixf(int w, int h, floatptrs& ary, int x, int y, cv::Mat& img)
{
	const int xend = x + w / 2;
	const int yend = y + h / 2;

	for (int i = x - w / 2, i2 = 0; i <= xend; ++i, ++i2)
	{
		for (int j = y - h / 2, j2 = 0; j <= yend; ++j, ++j2)
		{
			int ix = abs(i);
			int jy = abs(j);

			if (ix >= img.cols - 1)
			{
				ix -= ix - (img.cols - 1) + 1;
			}

			if (jy >= img.rows - 1)
			{
				jy -= jy - (img.rows - 1) + 1;
			}

			float& v = img.at<float>(jy, ix);
			ary[j2 * w + i2].c = &v;
			ary[j2 * w + i2].x = ix;
			ary[j2 * w + i2].y = jy;
		}
	}
}

void GetMatrix(int w, int h, Vec3fptrs& ary, int x, int y, cv::Mat& img)
{
	const int xend = x + w / 2;
	const int yend = y + h / 2;

	for (int i = x - w / 2, i2 = 0; i <= xend; ++i, ++i2)
	{
		for (int j = y - h / 2, j2 = 0; j <= yend; ++j, ++j2)
		{
			int ix = abs(i);
			int jy = abs(j);

			if (ix >= img.cols - 1)
			{
				ix -= ix - (img.cols - 1) + 1;
			}

			if (jy >= img.rows - 1)
			{
				jy -= jy - (img.rows - 1) + 1;
			}

			cv::Vec3f& v = img.at<cv::Vec3f>(jy, ix);
			ary[j2 * w + i2].c = &v;
			ary[j2 * w + i2].x = ix;
			ary[j2 * w + i2].y = jy;
		}
	}
}

void RarePixel(cv::Mat& src, cv::Mat& dst, int rectw, int recth)
{
	Vec3fptrs ary(rectw * recth);
	int median = rectw * recth / 2;

	for (int x = 0; x < src.cols; ++x)
	{
		for (int y = 0; y < src.rows; ++y)
		{
			GetMatrix(rectw, recth, ary, x, y, src);
			std::sort(ary.begin(), ary.end(), LightCompare);
			float v1 = GetLight(ary[median].c) - GetLight(ary.front().c);
			float v2 = GetLight(ary.back().c) - GetLight(ary.front().c);

			if (v1 > 10 / 255.0)
			{
				float& v = dst.at<float>(ary[0].y, ary[0].x);
				v += v1;
			}

			if (v2 > 10 / 255.0)
			{
				float& v = dst.at<float>(ary.front().y, ary.front().x);
				v += v2;
			}
		}
	}

	for (int x = 0; x < src.cols; ++x)
	{
		for (int y = 0; y < src.rows; ++y)
		{
			float& v = dst.at<float>(y, x);
			v = pow(v, 0.4f);
		}
	}

	normalize(dst, dst, 0, 1, cv::NORM_MINMAX);
}

void UnsharpMask0(cv::Mat& img, float amount, long  radius, float thresh)
{
	radius += (1 - (radius % 2));
	cv::Mat inF32;
	img.convertTo(inF32 , CV_32F);
	cv::Mat out;
	GaussianBlur(inF32 , out , cv::Size(radius , radius) , 0.0);
	cv::Mat hp = inF32 - out;
	cv::Mat hpabs = cv::abs(hp);
	cv::Mat hpthr;
	cv::threshold(hpabs , hpthr , thresh , 1.0 , cv::THRESH_BINARY);
	cv::Mat ret1 = inF32 + amount * hp.mul(hpthr);
	cv::Mat ret;
	normalize(ret, ret, 0, 1, cv::NORM_MINMAX);
	ret1.convertTo(img , CV_8U);
}

void UnsharpMask1(cv::Mat& img, double amount, double radius, double threshold)
{
	// create blurred img
	cv::Mat img32F, imgBlur32F, imgHighContrast32F, imgDiff32F, unsharpMas32F, colDelta32F, compRes, compRes32F, prod;
	double r = 1.5;
	img.convertTo(img32F, CV_32F);
	cv::GaussianBlur(img32F, imgBlur32F, cv::Size(0, 0), radius);
	cv::subtract(img32F, imgBlur32F, unsharpMas32F);
	// increase contrast( original, amount percent )
	imgHighContrast32F = img32F * amount / 100.0f;
	cv::subtract(imgHighContrast32F, img32F, imgDiff32F);
	unsharpMas32F /= 255.0f;
	cv::multiply(unsharpMas32F, imgDiff32F, colDelta32F);
	cv::compare(cv::abs(colDelta32F), threshold, compRes, cv::CMP_GT);
	compRes.convertTo(compRes32F, CV_32F);
	cv::multiply(compRes32F, colDelta32F, prod);
	cv::add(img32F, prod, img32F);
	//normalize(img32F, img32F, 0, 1, cv::NORM_MINMAX);
	img32F.convertTo(img, CV_8U);
}

void AdjustContrast(cv::Mat& src, cv::Mat& dst, float contrast)
{
	//求原圖均值
	cv::Vec3f mean(0, 0, 0);

	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{
			const cv::Vec3f& ori = src.at<cv::Vec3f>(y, x);
			mean += ori;
		}
	}

	for (int k = 0; k < 3; k++)
	{
		mean[k] /= src.rows * src.cols;
	}

	//調整對比度
	if (contrast <= -255)
	{
		//當增量等於-255時，是圖像對比度的下端極限，此時，圖像RGB各份量都等於閥值，圖像呈全灰色，灰度圖上只有1條線，即閥值灰度；
		for (int y = 0; y < src.rows; y++)
		{
			for (int x = 0; x < src.cols; x++)
			{
				cv::Vec3f& v = dst.at<cv::Vec3f>(y, x);
				v = mean;
			}
		}
	}
	else if (contrast > -255 &&  contrast <= 0)
	{
		//(1)nRGB = RGB + (RGB - Threshold) * Contrast / 255
		// 當增量大於-255且小於0時，直接用上面的公式計算圖像像素各份量
		//公式中，nRGB表示調整後的R、G、B份量，RGB表示原圖R、G、B份量，Threshold為給定的閥值，Contrast為處理過的對比度增量。
		for (int y = 0; y < src.rows; y++)
		{
			for (int x = 0; x < src.cols; x++)
			{
				cv::Vec3f nRGB;
				cv::Vec3f& ori = src.at<cv::Vec3f>(y, x);

				for (int k = 0; k < 3; k++)
				{
					nRGB[k] = ori[k] + (ori[k] - mean[k]) * contrast / 255;
				}

				cv::Vec3f& v = dst.at<cv::Vec3f>(y, x);
				v = nRGB;
			}
		}
	}
	else if (contrast > 0 && contrast < 255)
	{
		//當增量大於0且小於255時，則先按下面公式(2)處理增量，然後再按上面公式(1)計算對比度：
		//(2)、nContrast = 255 * 255 / (255 - Contrast) - 255
		//公式中的nContrast為處理後的對比度增量，Contrast為給定的對比度增量。
		cv::Vec3f nRGB;
		int nContrast = 255 * 255 / (255 - contrast) - 255;

		for (int y = 0; y < src.rows; y++)
		{
			for (int x = 0; x < src.cols; x++)
			{
				cv::Vec3f& ori = src.at<cv::Vec3f>(y, x);

				for (int k = 0; k < 3; k++)
				{
					nRGB[k] = ori[k] + (ori[k] - mean[k]) * nContrast / 255;
				}

				cv::Vec3f& v = dst.at<cv::Vec3f>(y, x);
				v = nRGB;
			}
		}
	}
	else
	{
		//當增量等於 255時，是圖像對比度的上端極限，實際等於設置圖像閥值，圖像由最多八種顏色組成，灰度圖上最多8條線，
		//即紅、黃、綠、青、藍、紫及黑與白；
		for (int y = 0; y < src.rows; y++)
		{
			for (int x = 0; x < src.cols; x++)
			{
				cv::Vec3f rgb;
				cv::Vec3f& ori = src.at<cv::Vec3f>(y, x);

				for (int k = 0; k < 3; k++)
				{
					if (ori[k] > mean[k])
					{
						rgb[k] = 255;
					}
					else
					{
						rgb[k] = 0;
					}
				}

				cv::Vec3f& v = dst.at<cv::Vec3f>(y, x);
				v = rgb;
			}
		}
	}
}

void UnsharpMask2(cv::Mat& src, float amount, float radius, float threshold)
{
	threshold /= 255.0;
	cv::Mat dst = src.clone();
	cv::Mat blurimage(src.rows, src.cols, src.type(), cv::Scalar(0));
	cv::Mat DiffImage(src.rows, src.cols, src.type(), cv::Scalar(0));
	//原圖的高對比度圖像
	cv::Mat highcontrast(src.rows, src.cols, src.type(), cv::Scalar(0));
	AdjustContrast(src, highcontrast, amount);
	//原圖的模糊圖像
	cv::GaussianBlur(src, blurimage, cv::Size(0, 0), radius);

	//原圖與模糊圖作差
	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{
			cv::Vec3f& ori = src.at<cv::Vec3f>(y, x);
			cv::Vec3f& blur = blurimage.at<cv::Vec3f>(y, x);
			cv::Vec3f val;
			val[0] = abs(ori[0] - blur[0]);
			val[1] = abs(ori[1] - blur[1]);
			val[2] = abs(ori[2] - blur[2]);
			DiffImage.at<cv::Vec3f>(y, x) = val;
		}
	}

	//銳化
	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{
			cv::Vec3f& hc = highcontrast.at<cv::Vec3f>(y, x);
			cv::Vec3f& diff = DiffImage.at<cv::Vec3f>(y, x);
			cv::Vec3f& ori = src.at<cv::Vec3f>(y, x);
			cv::Vec3f val;

			for (int k = 0; k < 3; k++)
			{
				if (diff[k] > threshold)
				{
					//最終圖像 = 原始*(1-r) + 高對比*r
					val[k] = ori[k] * (100.0 - amount) + hc[k] * amount;
					val[k] *= 0.01;
				}
				else
				{
					val[k] = ori[k];
				}
			}

			dst.at<cv::Vec3f>(y, x) = val;
		}
	}

	normalize(dst, dst, 0, 1, cv::NORM_MINMAX);
	src = dst;
}

#include "math/Vector2.h"

typedef Vector2s Line;
typedef std::vector<Line> Lines;
struct WeightData
{
	WeightData(Vector2& p, int w): pos(p), weight(w)	{}
	Vector2 pos;
	int  weight;
	bool operator<(const WeightData& wd)
	{
		return weight < wd.weight;
	}
};
typedef std::vector<WeightData> Weights;

Weights wm_init;
Weights wm_init_cross;
Weights wm_init2;
struct _tmp_function
{
	_tmp_function()
	{
		wm_init.push_back(WeightData(Vector2(-1, -1), 1));
		wm_init.push_back(WeightData(Vector2(0, -1), 1));
		wm_init.push_back(WeightData(Vector2(1, -1), 1));
		wm_init.push_back(WeightData(Vector2(-1, 0), 1));
		wm_init.push_back(WeightData(Vector2(1, 0), 1));
		wm_init.push_back(WeightData(Vector2(-1, 1), 1));
		wm_init.push_back(WeightData(Vector2(0, 1), 1));
		wm_init.push_back(WeightData(Vector2(1, 1), 1));
		wm_init_cross.push_back(WeightData(Vector2(0, -1), 1));
		wm_init_cross.push_back(WeightData(Vector2(0, +1), 1));
		wm_init_cross.push_back(WeightData(Vector2(-1, 0), 1));
		wm_init_cross.push_back(WeightData(Vector2(+1, 0), 1));
		wm_init2.push_back(WeightData(Vector2(-2, -2), 1));
		wm_init2.push_back(WeightData(Vector2(-1, -2), 1));
		wm_init2.push_back(WeightData(Vector2(0, -2), 1));
		wm_init2.push_back(WeightData(Vector2(1, -2), 1));
		wm_init2.push_back(WeightData(Vector2(2, -2), 1));
		wm_init2.push_back(WeightData(Vector2(-2, -1), 1));
		wm_init2.push_back(WeightData(Vector2(2, -1), 1));
		wm_init2.push_back(WeightData(Vector2(-2, 0), 1));
		wm_init2.push_back(WeightData(Vector2(2, 0), 1));
		wm_init2.push_back(WeightData(Vector2(-2, 1), 1));
		wm_init2.push_back(WeightData(Vector2(2, 1), 1));
		wm_init2.push_back(WeightData(Vector2(-2, 2), 1));
		wm_init2.push_back(WeightData(Vector2(-1, 2), 1));
		wm_init2.push_back(WeightData(Vector2(0, 2), 1));
		wm_init2.push_back(WeightData(Vector2(1, 2), 1));
		wm_init2.push_back(WeightData(Vector2(2, 2), 1));
	}
} __tmp_function;

void EdgeLink2(cv::Mat& image, Line& now_line)
{
	bool	edgefail = false;

	for (; !edgefail;)
	{
		edgefail = true;
		Weights wm = wm_init;

		if (now_line.size() > 1)
		{
			int x, y;
			Vector2 move = now_line.back() - *(now_line.end() - 2);

			for (int i = 0; i < wm.size(); i++)
			{
				if (move.y != 0 && move.y == wm[i].pos.y)
				{
					wm[i].weight++;
				}

				if (move.x != 0 && move.x == wm[i].pos.x)
				{
					wm[i].weight++;
				}

				if (wm[i].pos == move)
				{
					wm[i].weight++;
				}
			}
		}

		std::sort(wm.begin(), wm.end());
		const Vector2& v = now_line.back();

		for (int i = 0; i < wm.size(); i++)
		{
			int x = v.x + wm[i].pos.x;
			int y = v.y + wm[i].pos.y;

			if (y < 0) { y = 0; }

			if (y >= image.rows) { y = image.rows - 1; }

			if (x < 0) { x = 0; }

			if (x >= image.cols) { x = image.cols - 1; }

			char& intensity = image.at<char>(y, x);

			if (intensity != 0)
			{
				now_line.push_back(Vector2(x, y));
				intensity = 0;
				edgefail = false;
				break;
			}
		}

		if (edgefail)
		{
			for (int i = 0; i < wm_init2.size(); i++)
			{
				int x = v.x + wm_init2[i].pos.x;
				int y = v.y + wm_init2[i].pos.y;

				if (y < 0) { y = 0; }

				if (y >= image.rows) { y = image.rows - 1; }

				if (x < 0) { x = 0; }

				if (x >= image.cols) { x = image.cols - 1; }

				char& intensity = image.at<char>(y, x);

				if (intensity != 0)
				{
					now_line.push_back(Vector2(x, y));
					intensity = 0;
					edgefail = false;
					break;
				}
			}
		}
	}

	std::reverse(now_line.begin(), now_line.end());
	edgefail = false;

	for (; !edgefail;)
	{
		edgefail = true;
		Weights wm = wm_init;

		if (now_line.size() > 1)
		{
			int x, y;
			Vector2 move = now_line.back() - *(now_line.end() - 2);

			for (int i = 0; i < wm.size(); i++)
			{
				if (move.y != 0 && move.y == wm[i].pos.y)
				{
					wm[i].weight++;
				}

				if (move.x != 0 && move.x == wm[i].pos.x)
				{
					wm[i].weight++;
				}

				if (wm[i].pos == move)
				{
					wm[i].weight++;
				}
			}
		}

		std::sort(wm.begin(), wm.end());
		const Vector2& v = now_line.back();

		for (int i = 0; i < wm.size(); i++)
		{
			int x = v.x + wm[i].pos.x;
			int y = v.y + wm[i].pos.y;

			if (x < 0) { x = 0; }

			if (x >= image.cols) { x = image.cols - 1; }

			if (y < 0) { y = 0; }

			if (y >= image.rows) { y = image.rows - 1; }

			char& intensity = image.at<char>(y, x);

			if (intensity != 0)
			{
				now_line.push_back(Vector2(x, y));
				intensity = 0;
				edgefail = false;
				break;
			}
		}

		if (edgefail)
		{
			for (int i = 0; i < wm_init2.size(); i++)
			{
				int x = v.x + wm_init2[i].pos.x;
				int y = v.y + wm_init2[i].pos.y;

				if (x < 0) { x = 0; }

				if (x >= image.cols) { x = image.cols - 1; }

				if (y < 0) { y = 0; }

				if (y >= image.rows) { y = image.rows - 1; }

				char& intensity = image.at<char>(y, x);

				if (intensity != 0)
				{
					now_line.push_back(Vector2(x, y));
					intensity = 0;
					edgefail = false;
					break;
				}
			}
		}
	}
}

Lines ComputeEdgeLine2(const cv::Mat& image)
{
	cv::Mat tImage = image;
	Lines	res;

	for (int i = 0; i < tImage.rows; ++i)
	{
		for (int j = 0; j < tImage.cols; ++j)
		{
			char& intensity = tImage.at<char>(i, j);

			if (intensity != 0)
			{
				Line line;
				line.push_back(Vector2(j, i));
				intensity = 0;
				EdgeLink2(tImage, line);
				res.push_back(line);
			}
		}
	}

	return res;
}


void SmoothThreshold(cv::Mat src, cv::Mat& dst)
{
	//cv::resize(src, dst, cv::Size(src.cols * 2, src.rows * 2), 0, 0, CV_INTER_CUBIC);
	dst = src.clone();

	for (int r = 0; r < dst.rows; r++)
	{
		for (int c = 0; c < dst.cols; c++)
		{
			float s = dst.at<float>(r, c);
			float& v = dst.at<float>(r, c);

			if (s < 0.4)
			{
				v = 0;
			}
			else
			{
				v = 1;
			}
		}
	}

	
	cv::Mat skeleton;
	dst.convertTo(skeleton, CV_8U, 1);
	dst.convertTo(dst, CV_8U, 255);
	cvThin(skeleton, skeleton, 5);
 	//normalize(skeleton, skeleton, 0, 255, cv::NORM_MINMAX);
// 	Lines lines = ComputeEdgeLine2(skeleton);
// 	cv::RNG rng(12345);
// 	cv::Mat drawing = cv::Mat::zeros( skeleton.size(), CV_8UC3 );
// 	for (auto it = lines.begin();it!= lines.end();++it)
// 	{
// 		cv::Vec3b color( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
// 		for (auto it2 = it->begin();it2!=it->end();++it2)
// 		{
// 			drawing.at<cv::Vec3b>(it2->y, it2->x) = color;
// 		}
// 	}
// 	imshow("drawing", drawing);
// 	cv::waitKey();
//	imshow("skeleton", skeleton);
//	imshow("dst", dst);
	std::vector<cv::Vec4i> hierarchy;
	std::vector<std::vector<cv::Point>> contours;
	findContours( dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );

	cv::Mat drawing = cv::Mat::zeros( dst.size(), CV_8UC3 );
	cv::RNG rng(12345);
	for( int i = 0; i< contours.size(); i++ )
	{
		cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		//drawContours( drawing, contours, i, color, 1, 8, hierarchy, 0, cv::Point() );
		//if ( hierarchy[i][3] != -1 ) {
			drawContours( drawing, contours, i, color, -1 );
		//}
	}
	imshow( "Contours", drawing );
	cv::waitKey();
}

void Flood_LowLying(cv::Mat src, cv::Mat& dst, int rectw, int recth)
{
	cv::Mat MaxCapacity, MaxCapacity2, WaterMap, RealMap, ThresholdMap, tmp;
	Vec3fptrs ary(rectw * recth);
	dst = src.clone();
	MaxCapacity.create(src.rows, src.cols, CV_32F);
	WaterMap.create(src.rows, src.cols, CV_32F);
	RealMap.create(src.rows, src.cols, CV_32F);
	ThresholdMap.create(src.rows, src.cols, CV_32F);
	//WaterMap = cv::Scalar(1);

	for (int y = 0; y < src.rows; ++y)
	{
		for (int x = 0; x < src.cols; ++x)
		{
			GetMatrix(rectw, recth, ary, x, y, src);
			float& wmax = MaxCapacity.at<float>(y, x);
			std::sort(ary.begin(), ary.end(), LightCompare);
			wmax = GetLight(ary.back().c) - GetLight(src.at<cv::Vec3f>(y, x));
		}
	}

	for (int y = 0; y < src.rows; ++y)
	{
		for (int x = 0; x < src.cols; ++x)
		{
			float& r = RealMap.at<float>(y, x);
			cv::Vec3f& v = src.at<cv::Vec3f>(y, x);
			float& w = WaterMap.at<float>(y, x);
			float& wmax = MaxCapacity.at<float>(y, x);
			w = wmax*0.5;
			r = w + GetLight(v);
		}
	}

	normalize(MaxCapacity, MaxCapacity, 0, 1, cv::NORM_MINMAX);
	floatptrs aryf(rectw * recth);
	floatptrs aryr(rectw * recth);

	for (int count = 0; count < 10000; ++count)
	{
// 		if (count % 3 == 0)
// 		{
// 			for (int y = 0; y < src.rows; ++y)
// 			{
// 				for (int x = 0; x < src.cols; ++x)
// 				{
// 					float& w = WaterMap.at<float>(y, x);
// 					float& t = ThresholdMap.at<float>(y, x);
//
// 					if (w > 0.1)
// 					{
// 						t = w;
// 					}
// 					else
// 					{
// 						t = 0;
// 					}
// 				}
// 			}
// 		}
		tmp = WaterMap.clone();
		for (int y = 0; y < tmp.rows; ++y)
		{
			for (int x = 0; x < tmp.cols; ++x)
			{
				float& w = tmp.at<float>(y, x);
				w = 0.5-w;
			}
		}
		imshow("MaxCapacity", MaxCapacity);
		imshow("WaterMap", tmp);
		imshow("RealMap", RealMap);
		cv::waitKey(1);
		int start1 = 0, step1 = 1, end1 = src.rows;
		int start2 = 0, step2 = 1, end2 = src.cols;

// 		if (count % 2 == 1)
// 		{
// 			start1 = src.rows - 1;
// 			step1 = -1;
// 			end1 = -1;
// 			start2 = src.cols - 1;
// 			step2 = -1;
// 			end2 = -1;
// 		}

		for (int y = start1; y != end1; y += step1)
		{
			for (int x = start2; x != end2; x += step2)
			{
				float& wmax = MaxCapacity.at<float>(y, x);
				float& w = WaterMap.at<float>(y, x);
				float& r = RealMap.at<float>(y, x);

				if (w == 0) { continue; }

				GetMatrixf(rectw, recth, aryr, x, y, RealMap);
				std::sort(aryr.begin(), aryr.end(), LightComparef);

				for (int i = 0; i < 9; ++i)
				{
					if (r > aryr[i].value())
					{
						float wmax2 = MaxCapacity.at<float>(aryr[i].y, aryr[i].x);
						float maxadd = wmax2 - aryr[i].value();
						if (maxadd > 0)
						{
							if (maxadd >= w)
							{
								float& w2 = WaterMap.at<float>(aryr[i].y, aryr[i].x);
								w2 += w;
								float& r2 = RealMap.at<float>(aryr[i].y, aryr[i].x);
								r2 += w;
								r  -= w;
								w   = 0;
								break;
							}
							else
							{
								float add = w - maxadd;
								float& w2 = WaterMap.at<float>(aryr[i].y, aryr[i].x);
								w2 += add;
								float& r2 = RealMap.at<float>(aryr[i].y, aryr[i].x);
								r2 += add;
								r  -= add;
								w  -= add;
							}
						}
						else if (maxadd < 0)
						{
							float newr = (r + aryr[i].value()) * 0.5;
							float mover = r - newr;

							if (mover > 0)
							{
								float& w2 = WaterMap.at<float>(aryr[i].y, aryr[i].x);
								w2 += mover;
								float& r2 = RealMap.at<float>(aryr[i].y, aryr[i].x);
								r2 += mover;
								r  -= mover;
								w  -= mover;
							}
						}
					}
					else
					{
						break;
					}
				}
			}
		}
	}

	// END
	normalize(MaxCapacity, MaxCapacity, 0, 1, cv::NORM_MINMAX);
	for (int r = 0; r < MaxCapacity.rows; r++)
	{
		for (int c = 0; c < MaxCapacity.cols; c++)
		{
			float& s = MaxCapacity.at<float>(r, c);

			if (s > 0.4)
			{
				s = 0.4;
			}
			s = pow(s, 0.9f);
		}
	}

	normalize(MaxCapacity, MaxCapacity, 0, 1, cv::NORM_MINMAX);
	SmoothThreshold(MaxCapacity, MaxCapacity2);
// 	float garma = 2;
//
// 	for (int r = 0; r < MaxCapacity.rows; r++)
// 	{
// 		for (int c = 0; c < MaxCapacity.cols; c++)
// 		{
// 			float& s = MaxCapacity.at<float>(r, c);
// 			s = pow(s, garma);
// 			//s *= 30;
// 			cv::Vec3f& v = dst.at<cv::Vec3f>(r, c);
// 			v[0] -= s;
// 			v[1] -= s;
// 			v[2] -= s;
//
// 			if (v[0] < 0) { v[0] = 0; }
//
// 			if (v[1] < 0) { v[1] = 0; }
//
// 			if (v[2] < 0) { v[2] = 0; }
//
// 			if (s > 0.5)
// 			{
// 				//s = 1;
// 			}
// 			else
// 			{
// 				//s = 0;
// 			}
// 		}
// 	}
	cv::namedWindow("MaxCapacity", 0);
	cv::namedWindow("WaterMap", 0);
	cv::namedWindow("RealMap", 0);
	normalize(MaxCapacity, MaxCapacity, 0, 1, cv::NORM_MINMAX);
	//normalize(WaterMap, WaterMap, 0, 1, cv::NORM_MINMAX);
	imshow("MaxCapacity", MaxCapacity);
	imshow("WaterMap", WaterMap);
	imshow("RealMap", RealMap);
	dst = MaxCapacity2;
	cv::Mat outimg;
	dst.convertTo(outimg, CV_8U, 255);
	imwrite("girl.png", outimg);
// 	imshow("src", src);
	imshow("dst", dst);
	cv::waitKey(0);
}

using namespace cv;

typedef vector<float> floats;
typedef vector<vector<float> > widths;

typedef std::vector<WeightData> Weights;

// Weights wm_init;
// struct _tmp_function2
// {
// 	_tmp_function2()
// 	{
// 		for (int i = -5; i <= 5; ++i)
// 		{
// 			for (int j = -5; j <= 5; ++j)
// 			{
// 				wm_init.push_back(WeightData(Vector2(i, j), sqrtf(i * i + j * j)));
// 			}
// 		}
// 	}
// } __tmp_function2;

void Demo(const Mat& img1u)
{
	Mat srcImg1f, show3u = Mat::zeros(img1u.size(), CV_8UC3);
	//img1u.convertTo(srcImg1f, CV_32FC1, 1.0 / 255);
	srcImg1f = img1u.clone();
	CmCurveEx dEdge(srcImg1f);
	dEdge.CalSecDer(3);
	dEdge.Link();
	const vector<CmEdge>& edges = dEdge.GetEdges();

	for (size_t i = 0; i < edges.size(); i++)
	{
		Vec3b color(rand() % 155 + 100, rand() % 155 + 100, rand() % 155 + 100);
		const vector<Point>& pnts = edges[i].pnts;

		for (size_t j = 0; j < pnts.size(); j++)
		{
			show3u.at<Vec3b>(pnts[j]) = color;
		}
	}

	namedWindow("Image", 0);
	imshow("Derivativs", dEdge.GetDer());
	imshow("Image", img1u);
	namedWindow("Curv1", 0);
	imshow("Curv1", show3u);
	waitKey(0);
// 	Mat srcImg1f2, show3u2 = Mat::zeros(img1u.size(), CV_8UC3)
// 		, show3u3 = Mat::zeros(img1u.size()*2, CV_8UC3);
// 	//img1u.convertTo(srcImg1f2, CV_32FC1, 1.0 / 255);
// 	srcImg1f2 = img1u.clone();
// 	CmCurveEx dEdge2(srcImg1f2);
//  	dEdge2.CalSecDer(5);
// 	dEdge2.Link();
// 	const vector<CmEdge>& edges2 = dEdge2.GetEdges();
// 	widths theWidths;
// 	Weights wm = wm_init;
//
// 	for (size_t i = 0; i < edges2.size(); i++)
// 	{
// 		theWidths.push_back(floats());
// 		floats& now = theWidths.back();
// 		Vec3b color(rand() % 155 + 100, rand() % 155 + 100, rand() % 155 + 100);
// 		const vector<Point>& pnts = edges2[i].pnts;
//
// 		for (size_t j = 0; j < pnts.size() - 1; j++)
// 		{
// 			show3u2.at<Vec3b>(pnts[j]) = color;
// 			//////////////////////////////////////////////////////////////////////////
// 			Vector2 v(pnts[j].x, pnts[j].y);
// 			Vector2 v2(pnts[j + 1].x, pnts[j + 1].y);
// 			Vector2 dir = (v2 - v);
// 			dir.normalise();
// 			Vector2 right = Quaternion::GetRotation(dir, 90);
// 			Vector2 left = -right;
// 			float maxlen = 2;
//
// 			for (int i = 6; i <= 10; i++)
// 			{
// 				int x;
// 				int y;
//
// 				if (i % 2 == 0)
// 				{
// 					x = v.x + left.x;
// 					y = v.y + left.y;
// 				}
// 				else
// 				{
// 					x = v.x + right.x;
// 					y = v.y + right.y;
// 					right.normalise();
// 					left.normalise();
// 					right *= i / 2;
// 					left *= i / 2;
// 				}
//
// 				if (y < 0) { y = 0; }
//
// 				if (y >= show3u2.rows) { y = show3u2.rows - 1; }
//
// 				if (x < 0) { x = 0; }
//
// 				if (x >= show3u2.cols) { x = show3u2.cols - 1; }
//
// 				float& intensity = srcImg1f2.at<float>(y, x);
//
// 				if (intensity < 30/255.0)
// 				{
// 					maxlen = i / 2;
// 				}
// 			}
//
// 			now.push_back(maxlen / 2);
//
// 			//////////////////////////////////////////////////////////////////////////
// 			if (j < pnts.size() - 1)
// 			{
// 				line(show3u3, pnts[j]*2, pnts[j + 1]*2, cv::Scalar(color), maxlen, 8, 0);
// 			}
// 		}
// 	}
//
// 	imshow("Derivativs", dEdge2.GetDer());
// 	imshow("Image", img1u);
// 	namedWindow("Curv", 0);
// 	imshow("Curv", show3u2);
// 	namedWindow("Curv2", 0);
// 	imshow("Curv2", show3u3);
// 	waitKey(0);
}

int main(int argc, char* argv[])
{
	cv::Mat cImg = cv::imread("data\\girl7.png"), cImg2;
	//CmCurveEx::Demo( cImg, 1 );
	//cv::namedWindow("cImg", 0);
	cImg.convertTo(cImg, CV_32FC3, 1.0 / 255);
	Flood_LowLying(cImg, cImg2, 5, 5);
	cvtColor(cImg, cImg, CV_BGR2GRAY);
//	cv::waitKey();
	Demo(cImg);
// 	for (int k = 0; k < 9; k++)
// 	{
// 		printf("%d\n", k);
// 		cv::Mat simg = cImg.clone();
// 		cv::Mat simg2;
// 		UnsharpMask2(simg, 500, 5, 3);
// 		imshow("cImg", simg);
// 		cv::waitKey(0);
// 	}
// 	float garma = 3;
//
// 	for (int r = 0; r < MaxCapacity.rows; r++)
// 	{
// 		for (int c = 0; c < MaxCapacity.cols; c++)
// 		{
// 			cv::Vec3f& s = MaxCapacity.at<cv::Vec3f>(r, c);
// 			s[0] = pow(s[0], garma);
// 			s[1] = pow(s[1], garma);
// 			s[2] = pow(s[2], garma);
// 		}
// 	}
// 	cv::Mat sft, slc;
// 	cv::Mat dft, dlc, img, dst;
// 	slc = cImg.clone();
// 	//cvtColor(cImg, slc, CV_BGR2HLS);
// 	dst.create(cImg.rows, cImg.cols, CV_32FC1);
// 	slc.convertTo(img, CV_32FC3, 1.0);
// 	cImg.convertTo(dlc, CV_32FC3, 1.0);
// 	imshow("cImg", dlc);
// 	cv::namedWindow("img", 0);
// 	imshow("img", img);
//
// 	for (int k = 0; k < 9; k++)
// 	{
// 		{ g_rect += 2; }
// 		dst = cv::Scalar(0);
// 		RarePixel(img, dst, g_rect, g_rect);
// 		normalize(dst, dst, 0, 1, cv::NORM_MINMAX);
// 		imshow("Img", dst);
// 		g_channel = (g_channel + 1) % 3;
// 		cv::waitKey(0);
// 	}
	return 0;
}


// 	cv::Mat kern = (cv::Mat_<float>(6,3) <<
// 		1,  0, 0,
// 		1,  0, 0,
// 		0,  1, 0,
// 		0,  1, 0,
// 		0, 0,  1,
// 		0, 0,  1);
// 	filter2D(img, dst, img.depth(), kern );

//
// 	normalize(img, img, 0, 1, cv::NORM_MINMAX);


