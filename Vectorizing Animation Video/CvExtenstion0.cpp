#define _USE_MATH_DEFINES
#include "stdcv.h"
#include <cmath>
#include <algorithm>
#include <deque>
#include <iterator>
#include "CvExtenstion.h"
#include "CvExtenstion0.h"
#include "math\Quaternion.h"
#include "CmCurveEx.h"
#include "curve_fit.h"
#include "algSpline.h"
#include "SplineShape.h"
#include "math\Vector2.h"

cv::Mat CannyEdge(cv::Mat& image, double threshold1/*=0*/,
				  double threshold2/*=30*/, int apertureSize/*=3*/, bool L2gradient/*=false*/)
{
	cv::Mat edges;
	cvtColor(image, edges, CV_BGR2GRAY);   //convert from RGB color space to GRAY
	Canny(edges, edges,
		  threshold1,
		  threshold2,
		  apertureSize, L2gradient);
	cvtColor(edges, edges, CV_GRAY2BGR);
	bwmorph_clean(image);
	//Skel();
	return edges;
}

void bwmorph_clean(cv::Mat& image)
{
	for (int j = 1; j < image.cols - 1; ++j)
	{
		for (int i = 1; i < image.rows - 1; ++i)
		{
			cv::Vec3b& intensity = image.at<cv::Vec3b>(i, j);
			if (intensity[0] != 0 && intensity[1] != 0 && intensity[2] != 0)
			{
				bool zero = true;
				zero &= IsZero(image, i - 1, j - 1);
				zero &= IsZero(image, i - 1, j);
				zero &= IsZero(image, i - 1, j + 1);
				zero &= IsZero(image, i  , j - 1);
				zero &= IsZero(image, i  , j + 1);
				zero &= IsZero(image, i + 1, j - 1);
				zero &= IsZero(image, i + 1, j);
				zero &= IsZero(image, i + 1, j + 1);
				if (zero)
				{
					intensity[0] = 0;
					intensity[1] = 0;
					intensity[2] = 0;
				}
			}
		}
	}
}

bool IsZero(cv::Mat& image, int i, int j)
{
	cv::Vec3b& intensity = image.at<cv::Vec3b>(i, j);
	if (intensity[0] == 0 && intensity[1] == 0 && intensity[2] == 0)
	{
		return true;
	}
	return false;
}


Lines ComputeEdgeLine(const cv::Mat& image)
{
	cv::Mat tImage = image;
	Lines   res;
	for (int i = 0; i < tImage.rows; ++i)
	{
		for (int j = 0; j < tImage.cols; ++j)
		{
			cv::Vec3b& intensity = tImage.at<cv::Vec3b>(i, j);
			if (intensity[0] != 0 || intensity[1] != 0 || intensity[2] != 0)
			{
				Line line;
				line.push_back(Vector2(j, i));
				intensity[0] = 0;
				intensity[1] = 0;
				intensity[2] = 0;
				EdgeLink(tImage, line);
				res.push_back(line);
			}
		}
	}
	return res;
}

void EdgeLink(cv::Mat& image, Line& now_line)
{
	bool    edgefail = false;
	for (; !edgefail;)
	{
		edgefail = true;
		Weights wm = wm_init;
		if (now_line.size() > 1)
		{
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
			if (y < 0)
			{
				y = 0;
			}
			if (y >= image.rows)
			{
				y = image.rows - 1;
			}
			if (x < 0)
			{
				x = 0;
			}
			if (x >= image.cols)
			{
				x = image.cols - 1;
			}
			cv::Vec3b& intensity = image.at<cv::Vec3b>(y, x);
			if (intensity[0] != 0 || intensity[1] != 0 || intensity[2] != 0)
			{
				now_line.push_back(Vector2(x, y));
				intensity[0] = 0;
				intensity[1] = 0;
				intensity[2] = 0;
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
				if (y < 0)
				{
					y = 0;
				}
				if (y >= image.rows)
				{
					y = image.rows - 1;
				}
				if (x < 0)
				{
					x = 0;
				}
				if (x >= image.cols)
				{
					x = image.cols - 1;
				}
				cv::Vec3b& intensity = image.at<cv::Vec3b>(y, x);
				if (intensity[0] != 0 || intensity[1] != 0 || intensity[2] != 0)
				{
					now_line.push_back(Vector2(x, y));
					intensity[0] = 0;
					intensity[1] = 0;
					intensity[2] = 0;
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
			if (x < 0)
			{
				x = 0;
			}
			if (x >= image.cols)
			{
				x = image.cols - 1;
			}
			if (y < 0)
			{
				y = 0;
			}
			if (y >= image.rows)
			{
				y = image.rows - 1;
			}
			cv::Vec3b& intensity = image.at<cv::Vec3b>(y, x);
			if (intensity[0] != 0 || intensity[1] != 0 || intensity[2] != 0)
			{
				now_line.push_back(Vector2(x, y));
				intensity[0] = 0;
				intensity[1] = 0;
				intensity[2] = 0;
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
				if (x < 0)
				{
					x = 0;
				}
				if (x >= image.cols)
				{
					x = image.cols - 1;
				}
				if (y < 0)
				{
					y = 0;
				}
				if (y >= image.rows)
				{
					y = image.rows - 1;
				}
				cv::Vec3b& intensity = image.at<cv::Vec3b>(y, x);
				if (intensity[0] != 0 || intensity[1] != 0 || intensity[2] != 0)
				{
					now_line.push_back(Vector2(x, y));
					intensity[0] = 0;
					intensity[1] = 0;
					intensity[2] = 0;
					edgefail = false;
					break;
				}
			}
		}
	}
}


void EdgeLink2(cv::Mat& image, Line& now_line)
{
	bool    edgefail = false;
	for (; !edgefail;)
	{
		edgefail = true;
		Weights wm = wm_init;
		if (now_line.size() > 1)
		{
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
			if (y < 0)
			{
				y = 0;
			}
			if (y >= image.rows)
			{
				y = image.rows - 1;
			}
			if (x < 0)
			{
				x = 0;
			}
			if (x >= image.cols)
			{
				x = image.cols - 1;
			}
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
				if (y < 0)
				{
					y = 0;
				}
				if (y >= image.rows)
				{
					y = image.rows - 1;
				}
				if (x < 0)
				{
					x = 0;
				}
				if (x >= image.cols)
				{
					x = image.cols - 1;
				}
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
			if (x < 0)
			{
				x = 0;
			}
			if (x >= image.cols)
			{
				x = image.cols - 1;
			}
			if (y < 0)
			{
				y = 0;
			}
			if (y >= image.rows)
			{
				y = image.rows - 1;
			}
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
				if (x < 0)
				{
					x = 0;
				}
				if (x >= image.cols)
				{
					x = image.cols - 1;
				}
				if (y < 0)
				{
					y = 0;
				}
				if (y >= image.rows)
				{
					y = image.rows - 1;
				}
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
	Lines   res;
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
				// debug use
				//              cv::imshow("tImage", tImage);
				//
				//              cv::Mat drawing = cv::Mat::zeros(tImage.size(), CV_8UC3),
				//                  show3u3 = cv::Mat::zeros(tImage.size(), CV_8UC3);;
				//              cv::RNG rng(12345);
				//              for (int i = 0; i < res.size(); ++i)
				//              {
				//                  cv::Vec3b color(rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
				//                  Line& now_line = res[i];
				//
				//                  for (int j = 0; j < now_line.size(); ++j)
				//                  {
				//                      if (j != 0)
				//                      {
				//                          cv::Point p1(now_line[j - 1].x, now_line[j - 1].y);
				//                          cv::Point p2(now_line[j].x, now_line[j].y);
				//                          cv::line(show3u3, p1, p2, cv::Scalar(color), 1, 8, 0);
				//                      }
				//                  }
				//              }
				//
				//              imshow("drawing", show3u3);
				//              cv::waitKey();
			}
		}
	}
	return res;
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

void GetSkeletonLine(cv::Mat bmap, Lines& lines, double_vector2d& linewidths)
{
	int rectw = 5, recth = 5;
	floatptrs aryr(rectw * recth);
	cv::Mat skeleton;
	bmap.convertTo(skeleton, CV_8U, 1);
	cvThin(skeleton, skeleton, 10);
	normalize(skeleton, skeleton, 0, 255, cv::NORM_MINMAX);
	imshow("skeleton", skeleton);
	lines = ComputeEdgeLine2(skeleton);
	double_vector2d().swap(linewidths);
	linewidths.resize(lines.size());
	for (int i = 0; i < lines.size(); ++i)
	{
		Line& now_line = lines[i];
		double_vector& now_linewidth = linewidths[i];
		now_linewidth.resize(now_line.size());
		for (int j = 0; j < now_line.size(); ++j)
		{
			GetMatrixf(rectw, recth, aryr, now_line[j].x, now_line[j].y, bmap);
			float minwidth = 2;
			for (int k = 0; k < recth * rectw; ++k)
			{
				if (aryr[k].value() == 0)
				{
					float x = now_line[j].x - aryr[k].x;
					float y = now_line[j].y - aryr[k].y;
					float sum = sqrtf(x * x + y * y);
					if (sum < minwidth)
					{
						minwidth = sum;
					}
				}
			}
			if (minwidth < 2)
			{
				minwidth = 2;
			}
			now_linewidth[j] = minwidth;
		}
	}
}

void Skeleton(cv::Mat& image)
{
	cv::Mat skel(image.size(), image.type(), cv::Scalar(0));
	cv::Mat temp;
	cv::Mat eroded;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
	bool done;
	do
	{
		cv::erode(image, eroded, element);
		cv::dilate(eroded, temp, element);   // temp = open(img)
		cv::subtract(image, temp, temp);
		cv::bitwise_or(skel, temp, skel);
		eroded.copyTo(image);
		done = (cv::norm(image) == 0);
	}
	while (!done);
	image = skel;
}

void cvThin(cv::Mat& src, cv::Mat& dst, int iterations /*= 1*/)
{
	cv::Size size(src.cols, src.rows);
	dst = src.clone();
	int n = 0, i = 0, j = 0;
	for (n = 0; n < iterations; n++)
	{
		cv::Mat t_image = dst.clone();
		for (i = 0; i < size.height;  i++)
		{
			for (j = 0; j < size.width; j++)
			{
				if (t_image.at<uchar>(i, j) == 1)
				{
					int ap = 0;
					int p2 = (i == 0) ? 0 : t_image.at<uchar>(i - 1, j);
					int p3 = (i == 0 || j == size.width - 1) ? 0 : t_image.at<uchar>(i - 1, j + 1);
					if (p2 == 0 && p3 == 1)
					{
						ap++;
					}
					int p4 = (j == size.width - 1) ? 0 : t_image.at<uchar>(i, j + 1);
					if (p3 == 0 && p4 == 1)
					{
						ap++;
					}
					int p5 = (i == size.height - 1
							  || j == size.width - 1) ? 0 : t_image.at<uchar>(i + 1, j + 1);
					if (p4 == 0 && p5 == 1)
					{
						ap++;
					}
					int p6 = (i == size.height - 1) ? 0 : t_image.at<uchar>(i + 1, j);
					if (p5 == 0 && p6 == 1)
					{
						ap++;
					}
					int p7 = (i == size.height - 1 || j == 0) ? 0 : t_image.at<uchar>(i + 1, j - 1);
					if (p6 == 0 && p7 == 1)
					{
						ap++;
					}
					int p8 = (j == 0) ? 0 : t_image.at<uchar>(i, j - 1);
					if (p7 == 0 && p8 == 1)
					{
						ap++;
					}
					int p9 = (i == 0 || j == 0) ? 0 : t_image.at<uchar>(i - 1, j - 1);
					if (p8 == 0 && p9 == 1)
					{
						ap++;
					}
					if (p9 == 0 && p2 == 1)
					{
						ap++;
					}
					if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1
							&& (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7)
					{
						if (ap == 1)
						{
							if (!(p2 && p4 && p6))
							{
								if (!(p4 && p6 && p8))
								{
									dst.at<uchar>(i, j) = 0;
								}
							}
						}
					}
				}
			}
		}
		t_image = dst.clone();
		for (i = 0; i < size.height;  i++)
		{
			for (int j = 0; j < size.width; j++)
			{
				if (t_image.at<uchar>(i, j) == 1)
				{
					int ap = 0;
					int p2 = (i == 0) ? 0 : t_image.at<uchar>(i - 1, j);
					int p3 = (i == 0 || j == size.width - 1) ? 0 : t_image.at<uchar>(i - 1, j + 1);
					if (p2 == 0 && p3 == 1)
					{
						ap++;
					}
					int p4 = (j == size.width - 1) ? 0 : t_image.at<uchar>(i, j + 1);
					if (p3 == 0 && p4 == 1)
					{
						ap++;
					}
					int p5 = (i == size.height - 1
							  || j == size.width - 1) ? 0 : t_image.at<uchar>(i + 1, j + 1);
					if (p4 == 0 && p5 == 1)
					{
						ap++;
					}
					int p6 = (i == size.height - 1) ? 0 : t_image.at<uchar>(i + 1, j);
					if (p5 == 0 && p6 == 1)
					{
						ap++;
					}
					int p7 = (i == size.height - 1 || j == 0) ? 0 : t_image.at<uchar>(i + 1, j - 1);
					if (p6 == 0 && p7 == 1)
					{
						ap++;
					}
					int p8 = (j == 0) ? 0 : t_image.at<uchar>(i, j - 1);
					if (p7 == 0 && p8 == 1)
					{
						ap++;
					}
					int p9 = (i == 0 || j == 0) ? 0 : t_image.at<uchar>(i - 1, j - 1);
					if (p8 == 0 && p9 == 1)
					{
						ap++;
					}
					if (p9 == 0 && p2 == 1)
					{
						ap++;
					}
					if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1
							&& (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7)
					{
						if (ap == 1)
						{
							if (p2 * p4 * p8 == 0)
							{
								if (p2 * p6 * p8 == 0)
								{
									dst.at<uchar>(i, j) = 0;
								}
							}
						}
					}
				}
			}
		}
	}
}

cv::Mat Laplace(const cv::Mat& image, int aperture_size)
{
	cv::Mat edges;
	cvtColor(image, edges, CV_BGR2GRAY);   //convert from RGB color space to GRAY
	Laplacian(edges, edges, CV_8U, aperture_size, 1);
	cvtColor(edges, edges, CV_GRAY2BGR);
	return edges;
}

void S1FloodFill(cv::Mat& image, cv::Mat& mask, int x, int y,
				 CvLines& out_array)
{
	cv::Vec3b& c = image.at<cv::Vec3b>(y, x);
	if ((c[2] != 0 && c[1] != 0 && c[0] != 0) || c[0] == 255)
	{
		return;
	}
	cv::Point seed(x, y);
	cv::Rect ccomp;
	cv::Scalar newVal(128, 1, 1);
	int area;
	threshold(mask, mask, 1, 128, CV_THRESH_BINARY);
	area = floodFill(image, mask, seed, newVal, &ccomp);
	// get Contour line
	cv::Mat mask2 = mask.clone();
	for (int i = 0; i < mask2.rows ; i++)
	{
		char& c = mask2.at<char>(i, 0);
		c = 0;
		char& c2 = mask2.at<char>(i, mask2.cols - 1);
		c2 = 0;
	}
	for (int j = 0; j < mask2.cols ; j++)
	{
		char& c = mask2.at<char>(0, j);
		c = 0;
		char& c2 = mask2.at<char>(mask2.rows - 1, j);
		c2 = 0;
	}
	//  Dilation( mask2, 0, 1 );
	//  Erosion( mask2, 0, 1 );
	for (int i = 1; i < mask2.rows - 1; i++)
	{
		for (int j = 1; j < mask2.cols - 1; j++)
		{
			char& c = mask2.at<char>(i, j);
			if (c)
			{
				c = 255;
			}
		}
	}
	CvLines points;
	cv::findContours(mask2, points, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	double tarea = cv::contourArea(points.front());
	if (tarea < 50)
	{
		return ;
	}
	CV_Assert(points.size() == 1);
	out_array.push_back(points.front());
}

void S1FloodFill(cv::Mat& image, cv::Mat& mask, int x, int y,
				 CvPatchs& out_array, int dilation, int erosion)
{
	cv::Vec3b& c = image.at<cv::Vec3b>(y, x);
	if ((c[2] != 0 && c[1] != 0 && c[0] != 0) || c[0] == 255)
	{
		return;
	}
	cv::Point seed(x, y);
	cv::Rect ccomp;
	cv::Scalar newVal(128, 1, 1);
	int area;
	threshold(mask, mask, 1, 128, CV_THRESH_BINARY);
	area = floodFill(image, mask, seed, newVal, &ccomp);
	// get Contour line
	cv::Mat mask2 = mask.clone();
	ClearEdge(mask2);
	for (int i = 1; i < mask2.rows - 1; i++)
	{
		for (int j = 1; j < mask2.cols - 1; j++)
		{
			uchar& intensity = mask2.at<uchar>(i, j);
			if (intensity > 0)
			{
				intensity = 255;
			}
		}
	}
	cv::imshow("image", image);
	//cv::waitKey();
	CvLines points;
	cv::findContours(mask2, points, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	double tarea = cv::contourArea(points.front());
	if (points.empty() || tarea < 6)
	{
		return;
	}
	printf("num: %d\t area: %f\n", points.size(), tarea);
	if (dilation > 0)
	{
		Dilation(mask2, 1, dilation);
	}
	cv::Mat mask22 = mask2.clone();
	if (erosion > 0)
	{
		Erosion(mask2, 2, erosion);
	}
	CvLines points2;
	cv::findContours(mask2, points2, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	for (int i = erosion - 1; i >= 0 && points2.empty(); --i)
	{
		mask2 = mask22.clone();
		if (i > 0)
		{
			Erosion(mask2, 2, i);
		}
		cv::findContours(mask2, points2, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	}
	//cv::waitKey();
	CvPatch cvp;
	cvp.Outer() = points.front();
	if (points.size() > 1)
	{
		std::copy(points.begin() + 1, points.end(), std::back_inserter(cvp.Inter()));
	}
	cvp.Outer2() = points2.front();
	if (points2.size() > 1)
	{
		std::copy(points2.begin() + 1, points2.end(), std::back_inserter(cvp.Inter2()));
	}
	out_array.push_back(cvp);
}

CvLines GetSidelines(const cv::Mat& image0)
{
	cv::Mat mask, image;
	image0.copyTo(image);
	mask.create(image.rows + 2, image.cols + 2, CV_8UC1);
	mask = cv::Scalar::all(0);
	CvLines cvp;
	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{
			S1FloodFill(image, mask, j, i, cvp);
		}
	}
	return cvp;
}

CvPatchs S1GetPatchs(const cv::Mat& image0)
{
	cv::Mat mask, image;
	image0.copyTo(image);
	mask.create(image.rows + 2, image.cols + 2, CV_8UC1);
	mask = cv::Scalar::all(0);
	CvPatchs cvps;
	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{
			S1FloodFill(image, mask, j, i, cvps);
		}
	}
	return cvps;
}

CvPatchs S1GetPatchs(const cv::Mat& image0, int dilation, int erosion)
{
	cv::Mat mask, image;
	image0.copyTo(image);
	mask.create(image.rows + 2, image.cols + 2, CV_8UC1);
	mask = cv::Scalar::all(0);
	CvPatchs cvps;
	for (int i = 1; i < image.rows - 1; i++)
	{
		for (int j = 1; j < image.cols - 1; j++)
		{
			S1FloodFill(image, mask, j, i, cvps, dilation, erosion);
		}
	}
	return cvps;
}

Lines ComputeTrappedBallEdge(cv::Mat& image, const Lines& old_line,
							 int ball_radius)
{
	cv::Mat timage(image.size(), CV_8U, cv::Scalar(0));
	for (Lines::const_iterator it = old_line.begin(); it != old_line.end(); ++it)
	{
		for (Line::const_iterator it2 = it->begin(); it2 != it->end(); ++it2)
		{
			uchar& intensity = timage.at<uchar>(it2->y, it2->x);
			intensity = 255;
		}
	}
	Lines res = old_line;
	for (Lines::iterator it = res.begin(); it != res.end(); ++it)
	{
		Line& li = *it;
		if (li.size() < 4)
		{
			continue;
		}
		bool findhole = false;
		Vector2 ahead = (li[li.size() - 1] - li[li.size() - 2]) +
						(li[li.size() - 1] - li[li.size() - 3]);
		ahead.normalise();
		if (LinkTrapBallBack(li, ahead, timage, ball_radius))
		{
			continue;
		}
		for (int i = 1; i < 6; ++i)
		{
			Vector2 up = Quaternion::GetRotation(ahead, i * 10);
			if (LinkTrapBallBack(li, up, timage, ball_radius))
			{
				break;
			}
			Vector2 down = Quaternion::GetRotation(ahead, -i * 10);
			if (LinkTrapBallBack(li, down, timage, ball_radius))
			{
				break;
			}
		}
	}
	for (Lines::iterator it = res.begin(); it != res.end(); ++it)
	{
		std::reverse(it->begin(), it->end());
	}
	for (Lines::iterator it = res.begin(); it != res.end(); ++it)
	{
		Line& li = *it;
		if (li.size() < 4)
		{
			continue;
		}
		bool findhole = false;
		Vector2 ahead = (li[li.size() - 1] - li[li.size() - 2]) +
						(li[li.size() - 1] - li[li.size() - 3]);
		ahead.normalise();
		if (LinkTrapBallBack(li, ahead, timage, ball_radius))
		{
			continue;
		}
		for (int i = 1; i < 6; ++i)
		{
			Vector2 up = Quaternion::GetRotation(ahead, i * 10);
			if (LinkTrapBallBack(li, up, timage, ball_radius))
			{
				break;
			}
			Vector2 down = Quaternion::GetRotation(ahead, -i * 10);
			if (LinkTrapBallBack(li, down, timage, ball_radius))
			{
				break;
			}
		}
	}
	return res;
}


bool LinkTrapBallBack(Line& li, const Vector2& ahead, cv::Mat& image,
					  int ball_radius)
{
	for (int r = 1; r <= ball_radius; ++r)
	{
		Vector2 tmp = li.back() + ahead * r;
		if (li.back() == Vector2((int)tmp.x, (int)tmp.y))
		{
			continue;
		}
		if (CorrectPosition(image, (int)tmp.x, (int)tmp.y))
		{
			uchar& intensity = image.at<uchar>((int)tmp.y, (int)tmp.x);
			if (intensity == 255)
			{
				Vector2 t = li.back();
				for (int go_r = 1; go_r <= r; ++go_r)
				{
					Vector2 tmp2 = t + ahead * go_r;
					if (CorrectPosition(image, (int)tmp2.x, (int)tmp2.y))
					{
						uchar& color = image.at<uchar>((int)tmp2.y, (int)tmp2.x);
						color = 255;
						li.push_back(Vector2((int)tmp2.x, (int)tmp2.y));
					}
					else
					{
						break;
					}
				}
				std::unique(li.begin(), li.end());
				return true;
			}
		}
		else
		{
			Vector2 t = li.back();
			for (int go_r = 1; go_r <= r; ++go_r)
			{
				Vector2 tmp2 = t + ahead * go_r;
				if (CorrectPosition(image, (int)tmp2.x, (int)tmp2.y))
				{
					uchar& color = image.at<uchar>((int)tmp2.y, (int)tmp2.x);
					color = 255;
					li.push_back(Vector2((int)tmp2.x, (int)tmp2.y));
				}
				else
				{
					break;
				}
			}
			std::unique(li.begin(), li.end());
			return true;
		}
	}
	return false;
}

bool IsSafePos2(cv::Mat img, int x, int y)
{
	if (x >= 1 && y >= 1 && x < img.cols - 1 && y < img.rows - 1)
	{
		return true;
	}
	return false;
}

bool IsSafePos(cv::Mat img, int x, int y)
{
	if (x >= 0 && y >= 0 && x < img.cols && y < img.rows)
	{
		return true;
	}
	return false;
}

cv::Mat TrapBallMask1(cv::Mat LineImg, int size, int moprh)
{
	int size2 = size * 2 + 1;
	cv::Mat circle = getStructuringElement(moprh,
										   cv::Size(size2, size2));
	cv::Mat res = LineImg.clone();
	//res = cv::Scalar(0);
	cv::Mat hasFill;
	hasFill.create(LineImg.rows - size2, LineImg.cols - size2, CV_8UC1);
	hasFill = cv::Scalar(0);
	struct V2
	{
		V2() {}
		V2(int x, int y): x(x), y(y) {}
		int x, y;
	};
	typedef std::vector<V2> V2s;
	for (int i = 0; i < LineImg.rows - size2; ++i)
	{
		for (int j = 0; j < LineImg.cols - size2; ++j)
		{
			if (res.at<cv::Vec3b>(i + size, j + size)[0] == 0 && CheckMaskImg1(LineImg, circle, j, i))
			{
				V2s checks;
				checks.push_back(V2(j, i));
				while (!checks.empty())
				{
					V2 v = checks.back();
					checks.pop_back();
					hasFill.at<uchar>(v.y, v.x) = 1;
					V2 test[8];
					for (int i = 0; i < 8; ++i)
					{
						test[i] = v;
					}
					test[0].y -= 1;
					test[1].y += 1;
					test[2].x -= 1;
					test[3].x += 1;
					test[4].x += 1;
					test[4].y += 1;
					test[5].x -= 1;
					test[5].y += 1;
					test[6].x += 1;
					test[6].y -= 1;
					test[7].x -= 1;
					test[7].y -= 1;
					for (int i = 0; i < 8; ++i)
					{
						if (IsSafePos(hasFill, test[i].x, test[i].y)
								&& (hasFill.at<uchar>(test[i].y, test[i].x) == 0)
								&& CheckMaskImg1(LineImg, circle, test[i].x, test[i].y))
						{
							checks.push_back(test[i]);
						}
					}
					cv::Vec3b cc = MaskImgGet(res, circle, v.x, v.y);
					if (cc[0] == 0)
					{
						int c;
						c = rand();
						MaskImgDraw(res, circle, v.x, v.y, cv::Vec3b(c % 200 + 50, (c / 255) % 200 + 50,
									(c * 10) % 200 + 50));
					}
					else
					{
						MaskImgDraw(res, circle, v.x, v.y, cc);
					}
				}
			}
		}
	}
//  cv::imshow("res", res);
//  cv::waitKey();
	return res;
}


cv::Mat TrapBallMask2(cv::Mat LineImg, int size, int moprh)
{
	int size2 = size;
	cv::Mat circle = getStructuringElement(moprh,
										   cv::Size(size2, size2));
	cv::Mat res = LineImg.clone();
	//res = cv::Scalar(0);
	cv::Mat hasFill;
	hasFill.create(LineImg.rows - size2, LineImg.cols - size2, CV_8UC1);
	hasFill = cv::Scalar(0);
	struct V2
	{
		V2() {}
		V2(int x, int y): x(x), y(y) {}
		int x, y;
	};
	typedef std::vector<V2> V2s;
	for (int i = 0; i < LineImg.rows - size2; ++i)
	{
		for (int j = 0; j < LineImg.cols - size2; ++j)
		{
			if (CheckMaskImg2(LineImg, circle, j, i))
			{
				V2s checks;
				checks.push_back(V2(j, i));
				while (!checks.empty())
				{
					V2 v = checks.back();
					checks.pop_back();
					V2 test[8];
					for (int i = 0; i < 8; ++i)
					{
						test[i] = v;
					}
					test[0].y -= 1;
					test[1].y += 1;
					test[2].x -= 1;
					test[3].x += 1;
					test[4].x += 1;
					test[4].y += 1;
					test[5].x -= 1;
					test[5].y += 1;
					test[6].x += 1;
					test[6].y -= 1;
					test[7].x -= 1;
					test[7].y -= 1;
					for (int i = 0; i < 8; ++i)
					{
						if (IsSafePos2(hasFill, test[i].x, test[i].y)
								&& (hasFill.at<uchar>(test[i].y, test[i].x) == 0)
								&& CheckMaskImg2(res, circle, test[i].x, test[i].y))
						{
							checks.push_back(test[i]);
						}
					}
					if (hasFill.at<uchar>(v.y, v.x) == 0)
					{
						hasFill.at<uchar>(v.y, v.x) = 1;
						cv::Vec3b cc = MaskImgGet(res, circle, v.x, v.y);
						MaskImgDraw(res, circle, v.x, v.y, cc);
					}
				}
			}
		}
	}
//  cv::imshow("res", res);
//  cv::waitKey();
	return res;
}

cv::Mat TrapBallMask3(cv::Mat LineImg, int size, int moprh)
{
	int size2 = size;
	cv::Mat circle = getStructuringElement(moprh,
										   cv::Size(size2, size2));
	cv::Mat circlebig = getStructuringElement(moprh,
						cv::Size(size2 + 2, size2 + 2));
	cv::Mat res = LineImg.clone();
	cv::Mat hasFill;
	hasFill.create(LineImg.rows - size2, LineImg.cols - size2, CV_8UC1);
	hasFill = cv::Scalar(0);
	struct V2
	{
		V2() {}
		V2(int x, int y): x(x), y(y) {}
		int x, y;
	};
	typedef std::vector<V2> V2s;
	for (int i = 1; i < LineImg.rows - size2; ++i)
	{
		for (int j = 1; j < LineImg.cols - size2; ++j)
		{
			if (CheckMaskImg3(LineImg, circlebig, circle, j, i))
			{
				V2s checks;
				checks.push_back(V2(j, i));
				while (!checks.empty())
				{
					V2 v = checks.back();
					checks.pop_back();
					V2 test[8];
					for (int i = 0; i < 8; ++i)
					{
						test[i] = v;
					}
					test[0].y -= 1;
					test[1].y += 1;
					test[2].x -= 1;
					test[3].x += 1;
					test[4].x += 1;
					test[4].y += 1;
					test[5].x -= 1;
					test[5].y += 1;
					test[6].x += 1;
					test[6].y -= 1;
					test[7].x -= 1;
					test[7].y -= 1;
					for (int i = 0; i < 8; ++i)
					{
						if (IsSafePos2(hasFill, test[i].x, test[i].y)
								&& (hasFill.at<uchar>(test[i].y, test[i].x) == 0)
								&& CheckMaskImg3(res, circlebig, circle, test[i].x, test[i].y))
						{
							checks.push_back(test[i]);
						}
					}
					if (hasFill.at<uchar>(v.y, v.x) == 0)
					{
						hasFill.at<uchar>(v.y, v.x) = 1;
						cv::Vec3b cc = MaskImgGet(res, circle, v.x, v.y);
						MaskImgDraw(res, circle, v.x, v.y, cc);
					}
				}
			}
		}
	}
	//  cv::imshow("res", res);
	//  cv::waitKey();
	return res;
}


cv::Mat TrapBallMask4(cv::Mat LineImg, int moprh)
{
	cv::Mat res = LineImg.clone();
	struct V2
	{
		V2() {}
		V2(int x, int y): x(x), y(y) {}
		int x, y;
	};
	cv::Mat hasFill;
	hasFill.create(LineImg.rows, LineImg.cols, CV_8UC1);
	hasFill = cv::Scalar(0);
	typedef std::deque<V2> V2s;
	for (int i = 1; i < res.rows - 1; ++i)
	{
		for (int j = 1; j < res.cols - 1; ++j)
		{
			hasFill = cv::Scalar(0);
			V2 test[4];
			V2s checks;
			checks.push_back(V2(j, i));
			while (!checks.empty())
			{
				V2 v = checks.front();
				checks.pop_front();
				hasFill.at<uchar>(v.y, v.x) = 0;
				if (res.at<cv::Vec3b>(v.y, v.x)[0] != 0)
				{
					continue;
				}
				for (int k = 0; k < 4; ++k)
				{
					test[k] = v;
				}
				test[0].y -= 1;
				test[1].y += 1;
				test[2].x -= 1;
				test[3].x += 1;
				bool hascolor = false;
				for (int k = 0; k < 4; ++k)
				{
					if (IsSafePos(res, test[k].x, test[k].y))
					{
						cv::Vec3b cc = res.at<cv::Vec3b>(test[k].y, test[k].x);
						if (cc[0] != 0 && cc[0] != 255)
						{
							res.at<cv::Vec3b>(v.y, v.x) = cc;
							hascolor = true;
							break;
						}
					}
				}
				if (hascolor)
				{
					for (int k = 0; k < 4; ++k)
					{
						if (IsSafePos(res, test[k].x, test[k].y)
								&& (res.at<cv::Vec3b>(test[k].y, test[k].x)[0] == 0))
						{
							checks.push_back(test[k]);
						}
					}
				}
			}
		}
	}
	//  cv::imshow("res", res);
	//  cv::waitKey();
	return res;
}

bool CheckMaskImg1(cv::Mat LineImg, cv::Mat mask, int x, int y)
{
	bool has0 = false;
	for (int i = 0; i < mask.rows; i++)
	{
		for (int j = 0; j < mask.cols; j++)
		{
			if (mask.at<uchar>(i, j) > 0)
			{
				if (LineImg.at<cv::Vec3b>(y + i, x + j)[0] == 255)
				{
					return false;
				}
				else if (LineImg.at<cv::Vec3b>(y + i, x + j)[0] == 0)
				{
					has0 = true;
				}
			}
		}
	}
	return has0;
}

bool CheckMaskImg2(cv::Mat LineImg, cv::Mat mask, int x, int y)
{
	bool has0 = false;
	bool hasNo0 = false;
	for (int i = 0; i < mask.rows; i++)
	{
		for (int j = 0; j < mask.cols; j++)
		{
			if (mask.at<uchar>(i, j) > 0)
			{
				if (LineImg.at<cv::Vec3b>(y + i, x + j)[0] == 255)
				{
					return false;
				}
				else if (LineImg.at<cv::Vec3b>(y + i, x + j)[0] == 0)
				{
					has0 = true;
				}
				else
				{
					hasNo0 = true;
				}
			}
		}
	}
	return has0 && hasNo0;
}


bool CheckMaskImg3(cv::Mat LineImg, cv::Mat maskbig, cv::Mat mask, int x, int y)
{
	bool has0 = false;
	bool hasNo0 = false;
	for (int i = 0; i < mask.rows; i++)
	{
		for (int j = 0; j < mask.cols; j++)
		{
			if (mask.at<uchar>(i, j) > 0)
			{
				if (LineImg.at<cv::Vec3b>(y + i , x + j)[0] == 255)
				{
					return false;
				}
				else if (LineImg.at<cv::Vec3b>(y + i , x + j)[0] == 0)
				{
					has0 = true;
				}
			}
		}
	}
	cv::Vec3b test(0, 0, 0);
	for (int i = 0; i < maskbig.rows; i++)
	{
		for (int j = 0; j < maskbig.cols; j++)
		{
			if (maskbig.at<uchar>(i, j) > 0)
			{
				char kk = LineImg.at<cv::Vec3b>(y + i - 1, x + j - 1)[0];
				cv::Vec3b cc = LineImg.at<cv::Vec3b>(y + i - 1, x + j - 1);
				if (kk != 0 && kk != 255)
				{
					hasNo0 = true;
					if (test == cv::Vec3b(0, 0, 0))
					{
						test = cc;
					}
					else if (test != cc)
					{
						return false;
					}
				}
			}
		}
	}
	return has0 && hasNo0;
}

void MaskImgDraw(cv::Mat LineImg, cv::Mat mask, int x, int y, cv::Vec3b c)
{
	for (int i = 0; i < mask.rows; i++)
	{
		for (int j = 0; j < mask.cols; j++)
		{
			if (mask.at<uchar>(i, j) > 0)
			{
				LineImg.at<cv::Vec3b>(y + i, x + j) = c;
			}
		}
	}
}

cv::Vec3b MaskImgGet(cv::Mat LineImg, cv::Mat mask, int x, int y)
{
	for (int i = 0; i < mask.rows; i++)
	{
		for (int j = 0; j < mask.cols; j++)
		{
			if (mask.at<uchar>(i, j) > 0)
			{
				if (LineImg.at<cv::Vec3b>(y + i, x + j)[0] != 255 && LineImg.at<cv::Vec3b>(y + i, x + j)[0] != 0)
				{
					return LineImg.at<cv::Vec3b>(y + i, x + j);
				}
			}
		}
	}
	return cv::Vec3b();
}

cv::Mat ConvertToMedian(cv::Mat TestImg, cv::Mat src)
{
	cv::Mat res = TestImg.clone();
	struct V2
	{
		V2() {}
		V2(int x, int y): x(x), y(y) {}
		int x, y;
	};
	cv::Mat hasFill;
	hasFill.create(TestImg.rows, TestImg.cols, CV_8UC1);
	hasFill = cv::Scalar(0);
	typedef std::deque<V2> V2s;
	for (int i = 1; i < res.rows - 1; ++i)
	{
		for (int j = 1; j < res.cols - 1; ++j)
		{
			if (res.at<cv::Vec3b>(i, j)[0] == 255 ||
					res.at<cv::Vec3b>(i, j)[0] == 0 ||
					hasFill.at<uchar>(i, j) != 0)
			{
				continue;
			}
			cv::Vec3b ori = res.at<cv::Vec3b>(i, j);
			V2 test[4];
			V2s checks;
			V2s saves;
			ColorConstraintMedian ccm;
			checks.push_back(V2(j, i));
			while (!checks.empty())
			{
				V2 v = checks.front();
				saves.push_back(v);
				checks.pop_front();
				ccm.AddPoint(0, 0, src.at<cv::Vec3b>(v.y, v.x));
				for (int k = 0; k < 4; ++k)
				{
					test[k] = v;
				}
				test[0].y -= 1;
				test[1].y += 1;
				test[2].x -= 1;
				test[3].x += 1;
				for (int k = 0; k < 4; ++k)
				{
					if (IsSafePos(res, test[k].x, test[k].y) &&
							hasFill.at<uchar>(test[k].y, test[k].x) == 0)
					{
						cv::Vec3b cc = res.at<cv::Vec3b>(test[k].y, test[k].x);
						if (ori == cc)
						{
							hasFill.at<uchar>(test[k].y, test[k].x) = 1;
							checks.push_back(test[k]);
						}
					}
				}
			}
			cv::Vec3b medcolor = ccm.GetColorCvPoint();
			const uchar ERROR_MAX = 5;
			for (int k = 0; k < saves.size(); ++k)
			{
				cv::Vec3b& ori = src.at<cv::Vec3b>(saves[k].y, saves[k].x);
				cv::Vec3b& dst = res.at<cv::Vec3b>(saves[k].y, saves[k].x);
				dst[0] = (ori[0] + medcolor[0]) * 0.5;
				dst[0] = std::min<int>(dst[0], medcolor[0] + ERROR_MAX);
				dst[0] = std::max<int>(dst[0], medcolor[0] - ERROR_MAX);
				dst[1] = (ori[1] + medcolor[1]) * 0.5;
				dst[1] = std::min<int>(dst[1], medcolor[1] + ERROR_MAX);
				dst[1] = std::max<int>(dst[1], medcolor[1] - ERROR_MAX);
				dst[2] = (ori[2] + medcolor[2]) * 0.5;
				dst[2] = std::min<int>(dst[2], medcolor[2] + ERROR_MAX);
				dst[2] = std::max<int>(dst[2], medcolor[2] - ERROR_MAX);
			}
		}
	}
	return res;
}

cv::Mat ConvertToIndex(cv::Mat src, cv::Mat oriimg, Vector3s& output)
{
	output.clear();
	int markid = 1;
	cv::Mat res = src.clone();
	struct V2
	{
		V2() {}
		V2(int x, int y): x(x), y(y) {}
		int x, y;
	};
	typedef std::deque<V2> V2s;
	cv::Mat hasFill;
	hasFill.create(src.rows, src.cols, CV_8UC1);
	hasFill = cv::Scalar(0);
	for (int i = 0; i < res.rows ; ++i)
	{
		for (int j = 0; j < res.cols ; ++j)
		{
			int v = res.at<cv::Vec3b>(i, j)[0] + res.at<cv::Vec3b>(i, j)[1] * 255 +
					res.at<cv::Vec3b>(i, j)[2] * 255 * 255;
			if (v != 0 && (res.at<cv::Vec3b>(i, j)[0] == 255 || v < markid))
			{
				continue;
			}
			cv::Vec3b ori = res.at<cv::Vec3b>(i, j);
			V2 test[4];
			V2s checks;
			V2s saves;
			checks.push_back(V2(j, i));
			while (!checks.empty())
			{
				V2 v = checks.front();
				saves.push_back(v);
				checks.pop_front();
				for (int k = 0; k < 4; ++k)
				{
					test[k] = v;
				}
				test[0].y -= 1;
				test[1].y += 1;
				test[2].x -= 1;
				test[3].x += 1;
				for (int k = 0; k < 4; ++k)
				{
					if (IsSafePos(res, test[k].x, test[k].y) &&
							hasFill.at<uchar>(test[k].y, test[k].x) == 0)
					{
						cv::Vec3b cc = src.at<cv::Vec3b>(test[k].y, test[k].x);
						if (ori == cc)
						{
							hasFill.at<uchar>(test[k].y, test[k].x) = 1;
							checks.push_back(test[k]);
						}
					}
				}
			}
			cv::Vec3b medcolor = cv::Vec3b(markid, markid / 255, markid / 255 / 255);
			ColorConstraintMedian ccm;
			for (int k = 0; k < saves.size(); ++k)
			{
				cv::Vec3b& dst = res.at<cv::Vec3b>(saves[k].y, saves[k].x);
				ccm.AddPoint(0, 0, oriimg.at<cv::Vec3b>(saves[k].y, saves[k].x));
				dst = medcolor;
			}
			markid++;
			output.push_back(ccm.GetColorVector3());
		}
	}
	return res;
}
