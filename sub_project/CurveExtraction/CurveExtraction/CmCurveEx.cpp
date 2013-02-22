#include "StdAfx.h"
#include "CmCurveEx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/core/core.hpp"

float const static PI_QUARTER = PI_FLOAT * 0.25f;
float const static PI_EIGHTH = PI_FLOAT * 0.125f;

CmCurveEx::CmCurveEx(const cv::Mat& srcImg1f, float maxOrntDif)
	: m_maxAngDif(maxOrntDif)
	, m_img1f(srcImg1f)
{
	CV_Assert(srcImg1f.type() == CV_32FC1);
	m_h = m_img1f.rows;
	m_w = m_img1f.cols;
	m_pDer1f.create(m_h, m_w, CV_32FC1);
	m_pDer2.create(m_h, m_w, CV_8UC1);
	m_pOrnt1f.create(m_h, m_w, CV_32FC1);
	m_pLabel1i.create(m_h, m_w, CV_32SC1);
	m_pNext1i.create(m_h, m_w, CV_32SC1);
}

/* Compute the eigenvalues and eigenvectors of the Hessian matrix given by
dfdrr, dfdrc, and dfdcc, and sort them in descending order according to
their absolute values. */
void CmCurveEx::compute_eigenvals(double dfdrr, double dfdrc, double dfdcc, double eigval[2], double eigvec[2][2])
{
	double theta, t, c, s, e1, e2, n1, n2; /* , phi; */

	/* Compute the eigenvalues and eigenvectors of the Hessian matrix. */
	if (dfdrc != 0.0)
	{
		theta = 0.5 * (dfdcc - dfdrr) / dfdrc;
		t = 1.0 / (fabs(theta) + sqrt(theta * theta + 1.0));

		if (theta < 0.0) { t = -t; }

		c = 1.0 / sqrt(t * t + 1.0);
		s = t * c;
		e1 = dfdrr - t * dfdrc;
		e2 = dfdcc + t * dfdrc;
	}
	else
	{
		c = 1.0;
		s = 0.0;
		e1 = dfdrr;
		e2 = dfdcc;
	}

	n1 = c;
	n2 = -s;

	/* If the absolute value of an eigenvalue is larger than the other, put that
	eigenvalue into first position.  If both are of equal absolute value, put
	the negative one first. */
	if (fabs(e1) > fabs(e2))
	{
		eigval[0] = e1;
		eigval[1] = e2;
		eigvec[0][0] = n1;
		eigvec[0][1] = n2;
		eigvec[1][0] = -n2;
		eigvec[1][1] = n1;
	}
	else if (fabs(e1) < fabs(e2))
	{
		eigval[0] = e2;
		eigval[1] = e1;
		eigvec[0][0] = -n2;
		eigvec[0][1] = n1;
		eigvec[1][0] = n1;
		eigvec[1][1] = n2;
	}
	else
	{
		if (e1 < e2)
		{
			eigval[0] = e1;
			eigval[1] = e2;
			eigvec[0][0] = n1;
			eigvec[0][1] = n2;
			eigvec[1][0] = -n2;
			eigvec[1][1] = n1;
		}
		else
		{
			eigval[0] = e2;
			eigval[1] = e1;
			eigvec[0][0] = -n2;
			eigvec[0][1] = n1;
			eigvec[1][0] = n1;
			eigvec[1][1] = n2;
		}
	}
}

void cvThin(cv::Mat& src, cv::Mat& dst, int iterations)
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

					int p5 = (i == size.height - 1 || j == size.width - 1) ? 0 : t_image.at<uchar>(i + 1, j + 1);

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

					if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7)
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

					int p5 = (i == size.height - 1 || j == size.width - 1) ? 0 : t_image.at<uchar>(i + 1, j + 1);

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

					if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 1 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 7)
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


void Dilation(cv::Mat& image, int dilation_elem, int dilation_size)
{
	int dilation_type;
	int dilation_size2 = dilation_size * 2 + 1;

	if (dilation_elem == 0) { dilation_type = cv::MORPH_RECT; }
	else if (dilation_elem == 1) { dilation_type = cv::MORPH_CROSS; }
	else if (dilation_elem == 2) { dilation_type = cv::MORPH_ELLIPSE; }

	cv::Mat element = getStructuringElement(dilation_type,
	                                        cv::Size(dilation_size2, dilation_size2),
	                                        cv::Point(dilation_size, dilation_size));
	cv::dilate(image, image, element);
}

void Erosion(cv::Mat& image, int erosion_elem, int erosion_size)
{
	int erosion_type;
	int erosion_size2 = erosion_size * 2 + 1;

	if (erosion_elem == 0) { erosion_type = cv::MORPH_RECT; }
	else if (erosion_elem == 1) { erosion_type = cv::MORPH_CROSS; }
	else if (erosion_elem == 2) { erosion_type = cv::MORPH_ELLIPSE; }

	cv::Mat element = getStructuringElement(erosion_type,
	                                        cv::Size(erosion_size2, erosion_size2),
	                                        cv::Point(erosion_size, erosion_size));
	cv::erode(image, image, element);
}

const cv::Mat& CmCurveEx::CalSecDer(int kSize, float linkEndBound, float linkStartBound)
{
	cv::Mat dxx, dxy, dyy;
	Sobel(m_img1f, dxx, CV_32F, 2, 0, kSize);
	Sobel(m_img1f, dxy, CV_32F, 1, 1, kSize);
	Sobel(m_img1f, dyy, CV_32F, 0, 2, kSize);

	double eigval[2], eigvec[2][2];
	for (int y = 0; y < m_h; y++)
	{
		float *xx = dxx.ptr<float>(y);
		float *xy = dxy.ptr<float>(y);
		float *yy = dyy.ptr<float>(y);
		float *pOrnt = m_pOrnt1f.ptr<float>(y);
		float *pDer = m_pDer1f.ptr<float>(y);
		for (int x = 0; x < m_w; x++)
		{
			compute_eigenvals(yy[x], xy[x], xx[x], eigval, eigvec);
			pOrnt[x] = (float)atan2(-eigvec[0][1], eigvec[0][0]); //計算法線方向
			if (pOrnt[x] < 0.0f)
				pOrnt[x] += PI2;
			pDer[x] = float(eigval[0] > 0.0f ? eigval[0] : 0.0f);//計算二階導數
		}
	}

	GaussianBlur(m_pDer1f, m_pDer1f, cv::Size(3, 3), 0);
	normalize(m_pDer1f, m_pDer1f, 0, 1, cv::NORM_MINMAX);
	NoneMaximalSuppress(linkEndBound, linkStartBound);
	return m_pDer1f;
}

const cv::Mat& CmCurveEx::CalFirDer(int kSize, float linkEndBound, float linkStartBound)
{
	cv::Mat dxMat, dyMat;
	Sobel(m_img1f, dxMat, CV_32F, 1, 0, kSize);
	Sobel(m_img1f, dyMat, CV_32F, 0, 1, kSize);

	for (int y = 0; y < m_h; y++)
	{
		float* dx = dxMat.ptr<float>(y);
		float* dy = dyMat.ptr<float>(y);
		float* pOrnt = m_pOrnt1f.ptr<float>(y);
		float* pDer = m_pDer1f.ptr<float>(y);

		for (int x = 0; x < m_w; x++)
		{
			pOrnt[x] = (float)atan2f(dx[x], -dy[x]);

			if (pOrnt[x] < 0.0f)
			{
				pOrnt[x] += PI2;
			}

			pDer[x] = sqrt(dx[x] * dx[x] + dy[x] * dy[x]);
		}
	}

	GaussianBlur(m_pDer1f, m_pDer1f, cv::Size(3, 3), 0);
	normalize(m_pDer1f, m_pDer1f, 0, 1, cv::NORM_MINMAX);
	NoneMaximalSuppress(linkEndBound, linkStartBound);
	return m_pDer1f;
}

void CmCurveEx::NoneMaximalSuppress(float linkEndBound, float linkStartBound)
{
	CV_Assert(m_pDer1f.data != NULL && m_pLabel1i.data != NULL);
	m_StartPnt.clear();
	m_StartPnt.reserve(int(0.08 * m_h * m_w));
	PntImp linePoint;
	m_pLabel1i = IND_BG;

	for (int r = 1; r < m_h - 1; r++)
	{
		float* pDer = m_pDer1f.ptr<float>(r);
		float* pOrnt = m_pOrnt1f.ptr<float>(r);
		int* pLineInd = m_pLabel1i.ptr<int>(r);

		for (int c = 1; c < m_w - 1; c++)
		{
			if (pDer[c] < linkEndBound)
			{
				continue;
			}

			float cosN = sin(pOrnt[c]);
			float sinN = -cos(pOrnt[c]);
			int xSgn = CmSgn<float>(cosN);
			int ySgn = CmSgn<float>(sinN);
			cosN *= cosN;
			sinN *= sinN;

			if (pDer[c] >= (pDer[c + xSgn] * cosN + m_pDer1f.at<float>(r + ySgn, c) * sinN)
			                && pDer[c] >= (pDer[c - xSgn] * cosN + m_pDer1f.at<float>(r - ySgn, c) * sinN))
			{
				pLineInd[c] = IND_NMS;

				if (pDer[c] < linkStartBound)
				{
					continue;
				}

				//add to m_vStartPoint
				linePoint.second = cv::Point(c, r);
				linePoint.first = pDer[c];
				m_StartPnt.push_back(linePoint);
			}
		}
	}
}

const CmEdges& CmCurveEx::Link(int shortRemoveBound /* = 3 */)
{
	CV_Assert(m_pDer1f.data != NULL && m_pLabel1i.data != NULL);
	sort(m_StartPnt.begin(), m_StartPnt.end(), linePointGreater);
	m_pNext1i = -1;
	m_vEdge.clear();
	m_vEdge.reserve(int(0.01 * m_w * m_h));
	CEdge crtEdge(0);//當前邊

	for (vector<PntImp>::iterator it = m_StartPnt.begin(); it != m_StartPnt.end(); it++)
	{
		cv::Point pnt = it->second;

		if (m_pLabel1i.at<int>(pnt) != IND_NMS)
		{
			continue;
		}

		findEdge(pnt, crtEdge, 0);
		findEdge(pnt, crtEdge, 1);

		if (crtEdge.pointNum <= shortRemoveBound)
		{
			cv::Point point = crtEdge.start;
			int i, nextInd;

			for (i = 1; i < crtEdge.pointNum; i++)
			{
				m_pLabel1i.at<int>(point) = IND_SR;
				nextInd = m_pNext1i.at<int>(point);
				point += DIRECTION8[nextInd];
			}

			m_pLabel1i.at<int>(point) = IND_SR;
		}
		else
		{
			m_vEdge.push_back(crtEdge);
			crtEdge.index++;
		}
	}

	// Get edge information
	int edgNum = (int)m_vEdge.size();

	for (int i = 0; i < edgNum; i++)
	{
		CEdge& edge = m_vEdge[i];
		vector<cv::Point>& pnts = edge.pnts;
		pnts.resize(edge.pointNum);
		pnts[0] = edge.start;

		for (int j = 1; j < edge.pointNum; j++)
		{
			pnts[j] = pnts[j - 1] + DIRECTION8[m_pNext1i.at<int>(pnts[j - 1])];
		}
	}

	return m_vEdge;
}

/************************************************************************/
/* 如果isForward為TRUE則沿著m_pOrnt方向尋找crtEdge, 並將沿途點的m_pNext */
/* 相應值置為沿途的方向值, 同時把m_pLineInd的值置為當前線的編號,找不到  */
/* 下一點的時候把最後一個點的坐標置為crtEdge的End坐標.                  */
/* 如果isForward為FALSE則沿著m_pOrnt反方向尋找crtEdge, 並將沿途點的     */
/* m_pNext相應值置為沿途的方向反向值, 同時把m_pLineInd的值置為當前線的  */
/* 編號.找不到下一點的時候如果pointNum太小則active置為false並推出.否則  */
/* 把最後一個點的坐標置為crtEdge的End坐標.                              */
/************************************************************************/
void CmCurveEx::findEdge(cv::Point seed, CEdge& crtEdge, bool isBackWard)
{
	cv::Point pnt = seed;
	float ornt = m_pOrnt1f.at<float>(pnt);

	if (isBackWard)
	{
		ornt += PI_FLOAT;

		if (ornt >= PI2)
		{
			ornt -= PI2;
		}
	}
	else
	{
		crtEdge.pointNum = 1;
		m_pLabel1i.at<int>(pnt) = crtEdge.index;
	}

	int orntInd, nextInd1, nextInd2;

	while (true)
	{
		/*************按照優先級尋找下一個點，方向差異較大不加入**************/
		//下一個點在DIRECTION16最佳方向上找
		orntInd = int(ornt / PI_EIGHTH + 0.5f) % 16;

		if (jumpNext(pnt, ornt, crtEdge, orntInd, isBackWard))
		{
			continue;
		}

		//下一個點在DIRECTION8最佳方向上找
		orntInd = int(ornt / PI_QUARTER + 0.5f) % 8;

		if (goNext(pnt, ornt, crtEdge, orntInd, isBackWard))
		{
			continue;
		}

		//下一個點在DIRECTION16次優方向上找
		orntInd = int(ornt / PI_EIGHTH + 0.5f) % 16;
		nextInd1 = (orntInd + 1) % 16;
		nextInd2 = (orntInd + 15) % 16;

		if (angle(DRT_ANGLE[nextInd1], ornt) < angle(DRT_ANGLE[nextInd2], ornt))
		{
			if (jumpNext(pnt, ornt, crtEdge, nextInd1, isBackWard))
			{
				continue;
			}

			if (jumpNext(pnt, ornt, crtEdge, nextInd2, isBackWard))
			{
				continue;
			}
		}
		else //下一個點在DIRECTION16另一個方向上找
		{
			if (jumpNext(pnt, ornt, crtEdge, nextInd2, isBackWard))
			{
				continue;
			}

			if (jumpNext(pnt, ornt, crtEdge, nextInd1, isBackWard))
			{
				continue;
			}
		}

		//下一個點在DIRECTION8次優方向上找
		orntInd = int(ornt / PI_QUARTER + 0.5f) % 8;
		nextInd1 = (orntInd + 1) % 8;
		nextInd2 = (orntInd + 7) % 8;

		if (angle(DRT_ANGLE[nextInd1], ornt) < angle(DRT_ANGLE[nextInd2], ornt))
		{
			if (goNext(pnt, ornt, crtEdge, nextInd1, isBackWard))
			{
				continue;
			}

			if (goNext(pnt, ornt, crtEdge, nextInd2, isBackWard))
			{
				continue;
			}
		}
		else //下一個點在DIRECTION8另一個方向上找
		{
			if (goNext(pnt, ornt, crtEdge, nextInd2, isBackWard))
			{
				continue;
			}

			if (goNext(pnt, ornt, crtEdge, nextInd1, isBackWard))
			{
				continue;
			}
		}

		/*************按照優先級尋找下一個點，方向差異較大也加入**************/
		//下一個點在DIRECTION16最佳方向上找
		orntInd = int(ornt / PI_EIGHTH + 0.5f) % 16;

		if (jumpNext(pnt, ornt, crtEdge, orntInd, isBackWard))
		{
			continue;
		}

		//下一個點在DIRECTION8最佳方向上找
		orntInd = int(ornt / PI_QUARTER + 0.5f) % 8;

		if (goNext(pnt, ornt, crtEdge, orntInd, isBackWard))
		{
			continue;
		}

		//下一個點在DIRECTION16次優方向上找
		orntInd = int(ornt / PI_EIGHTH + 0.5f) % 16;
		nextInd1 = (orntInd + 1) % 16;
		nextInd2 = (orntInd + 15) % 16;

		if (angle(DRT_ANGLE[nextInd1], ornt) < angle(DRT_ANGLE[nextInd2], ornt))
		{
			if (jumpNext(pnt, ornt, crtEdge, nextInd1, isBackWard))
			{
				continue;
			}

			if (jumpNext(pnt, ornt, crtEdge, nextInd2, isBackWard))
			{
				continue;
			}
		}
		else //下一個點在DIRECTION16另一個方向上找
		{
			if (jumpNext(pnt, ornt, crtEdge, nextInd2, isBackWard))
			{
				continue;
			}

			if (jumpNext(pnt, ornt, crtEdge, nextInd1, isBackWard))
			{
				continue;
			}
		}

		//下一個點在DIRECTION8次優方向上找
		orntInd = int(ornt / PI_QUARTER + 0.5f) % 8;
		nextInd1 = (orntInd + 1) % 8;
		nextInd2 = (orntInd + 7) % 8;

		if (angle(DRT_ANGLE[nextInd1], ornt) < angle(DRT_ANGLE[nextInd2], ornt))
		{
			if (goNext(pnt, ornt, crtEdge, nextInd1, isBackWard))
			{
				continue;
			}

			if (goNext(pnt, ornt, crtEdge, nextInd2, isBackWard))
			{
				continue;
			}
		}
		else //下一個點在DIRECTION8另一個方向上找
		{
			if (goNext(pnt, ornt, crtEdge, nextInd2, isBackWard))
			{
				continue;
			}

			if (goNext(pnt, ornt, crtEdge, nextInd1, isBackWard))
			{
				continue;
			}
		}

		break;//如果ornt附近的三個方向上都沒有的話,結束尋找
	}

	if (isBackWard)
	{
		crtEdge.start = pnt;
	}
	else
	{
		crtEdge.end = pnt;
	}
}


float CmCurveEx::angle(float ornt1, float orn2)
{
	//兩個ornt都必須在[0, 2*PI)之間, 返回值在[0, PI/2)之間
	float agl = ornt1 - orn2;

	if (agl < 0)
	{
		agl += PI2;
	}

	if (agl >= PI_FLOAT)
	{
		agl -= PI_FLOAT;
	}

	if (agl >= PI_HALF)
	{
		agl -= PI_FLOAT;
	}

	return fabs(agl);
}

void CmCurveEx::refreshOrnt(float& ornt, float& newOrnt)
{
	static const float weightOld = 0.0f;
	static const float weightNew = 1.0f - weightOld;
	static const float largeBound = PI_FLOAT + PI_HALF;
	static const float smallBound = PI_HALF;

	if (newOrnt >= ornt + largeBound)
	{
		newOrnt -= PI2;
		ornt = ornt * weightOld + newOrnt * weightNew;

		if (ornt < 0.0f)
		{
			ornt += PI2;
		}
	}
	else if (newOrnt + largeBound <= ornt)
	{
		newOrnt += PI2;
		ornt = ornt * weightOld + newOrnt * weightNew;

		if (ornt >= PI2)
		{
			ornt -= PI2;
		}
	}
	else if (newOrnt >= ornt + smallBound)
	{
		newOrnt -= PI_FLOAT;
		ornt = ornt * weightOld + newOrnt * weightNew;

		if (ornt < 0.0f)
		{
			ornt += PI2;
		}
	}
	else if (newOrnt + smallBound <= ornt)
	{
		newOrnt += PI_FLOAT;
		ornt = ornt * weightOld + newOrnt * weightNew;

		if (ornt >= PI2)
		{
			ornt -= PI2;
		}
	}
	else
	{
		ornt = ornt * weightOld + newOrnt * weightNew;
	}

	newOrnt = ornt;
}

bool CmCurveEx::goNext(cv::Point& pnt, float& ornt, CEdge& crtEdge, int orntInd, bool isBackward)
{
	cv::Point pntN = pnt + DIRECTION8[orntInd];
	int& label = m_pLabel1i.at<int>(pntN);

	//如果該點方向與當前線方向差別比較大則不加入/***********一個可變域值**********************/
	if (CHK_IND(pntN) && (label == IND_NMS || label == IND_SR))
	{
		if (angle(ornt, m_pOrnt1f.at<float>(pntN)) > m_maxAngDif)
		{
			return 0;
		}

		label = crtEdge.index;

		if (isBackward)
		{
			m_pNext1i.at<int>(pntN) = (orntInd + 4) % 8;
		}
		else
		{
			m_pNext1i.at<int>(pnt) = orntInd;
		}

		crtEdge.pointNum++;
		//更新切線方向
		refreshOrnt(ornt, m_pOrnt1f.at<float>(pntN));
		pnt = pntN;
		return true;
	}

	return false;
}

bool CmCurveEx::jumpNext(cv::Point& pnt, float& ornt, CEdge& crtEdge, int orntInd, bool isBackward)
{
	cv::Point pnt2 = pnt + DIRECTION16[orntInd];

	if (CHK_IND(pnt2) && m_pLabel1i.at<int>(pnt2) <= IND_NMS)
	{
		if (angle(ornt, m_pOrnt1f.at<float>(pnt2)) > m_maxAngDif) //如果該點方向與當前線方向差別比較大則不加入
		{
			return false;
		}

		// DIRECTION16方向上的orntInd相當於DIRECTION8方向上兩個orntInd1,orntInd2
		// 的疊加,滿足orntInd = orntInd1 + orntInd2.此處優先選擇使得組合上的點具
		// IND_NMS標記的方向組合。(orntInd1,orntInd2在floor(orntInd/2)和
		// ceil(orntInd/2)中選擇
		int orntInd1 = orntInd >> 1, orntInd2;
		cv::Point pnt1 = pnt + DIRECTION8[orntInd1];

		if (m_pLabel1i.at<int>(pnt1) >= IND_BG && orntInd % 2)
		{
			orntInd1 = ((orntInd + 1) >> 1) % 8;
			pnt1 = pnt + DIRECTION8[orntInd1];
		}

		int& lineIdx1 = m_pLabel1i.at<int>(pnt1);

		if (lineIdx1 != -1) //當前nPos1點為其它線上的點，不能歸入當前線
		{
			return false;
		}

		orntInd2 = orntInd - orntInd1;
		orntInd2 %= 8;
		lineIdx1 = crtEdge.index;
		m_pLabel1i.at<int>(pnt2) = crtEdge.index;

		if (isBackward)
		{
			m_pNext1i.at<int>(pnt1) = (orntInd1 + 4) % 8;
			m_pNext1i.at<int>(pnt2) = (orntInd2 + 4) % 8;
		}
		else
		{
			m_pNext1i.at<int>(pnt) = orntInd1;
			m_pNext1i.at<int>(pnt1) = orntInd2;
		}

		crtEdge.pointNum += 2;
		refreshOrnt(ornt, m_pOrnt1f.at<float>(pnt1));
		refreshOrnt(ornt, m_pOrnt1f.at<float>(pnt2));
		pnt = pnt2;
		return true;
	}

	return false;
}

int ffillMode = 1;
int loDiff = 16, upDiff = 16;
int connectivity = 4;
int isColor = true;
bool useMask = false;
int newMaskVal = 255;

bool MyfloodFill(int x, int y, cv::Mat mask01, cv::Mat mask02, cv::Mat image)
{
	cv::Point seed(x, y);

	if (mask01.at<uchar>(y + 1, x + 1) > 0
	                || mask02.at<uchar>(y , x) > 0)
	{
		return false;
	}

	int lo = loDiff;
	int up = upDiff;
	int flags = connectivity + (newMaskVal << 8) +
	            (ffillMode == 1 ? CV_FLOODFILL_FIXED_RANGE : 0);
	int b = rand() % 255;
	int g = rand() % 255;
	int r = rand() % 255;
	cv::Rect ccomp;
	cv::Scalar newVal(b, g, r);
	cv::Mat dst = image;
	int area;
	threshold(mask01, mask01, 1, 128, CV_THRESH_BINARY);
	area = floodFill(dst, mask01, seed, newVal, &ccomp, cv::Scalar(lo, lo, lo),
	                 cv::Scalar(up, up, up), flags);
// 	cv::imshow("xx", mask01);
// 	cv::waitKey();
	return true;
}

void CmCurveEx::Demo(const cv::Mat& cImg, bool isCartoon)
{
	cv::Mat img1u, cImg2, cImg3, ctmp;
	cImg2 = cImg.clone();
	cImg3 = cImg.clone();
	cImg.convertTo(ctmp, CV_32F, 1.0 / 255);
	cvtColor(cImg, img1u, CV_RGB2HSV);
	cv::Mat srcImg1f, show3u = cv::Mat::zeros(img1u.size(), CV_8UC3);
	img1u.convertTo(srcImg1f, CV_32FC1, 1.0 / 255);
	CmCurveEx dEdge(srcImg1f);
	dEdge.SetColorImage(ctmp);
	if (isCartoon)
	{
		dEdge.CalSecDer(3, 0.01f, 0.2f);
	}
	else
	{
		dEdge.CalFirDer(5, 0.05f, 0.05f);
	}

	dEdge.Link();
	const vector<CEdge>& edges = dEdge.GetEdges();

	for (size_t i = 0; i < edges.size(); i++)
	{
		cv::Vec3b color(rand() % 255, rand() % 255, rand() % 255);
		const vector<cv::Point>& pnts = edges[i].pnts;

		for (size_t j = 0; j < pnts.size(); j++)
		{
			show3u.at<cv::Vec3b>(pnts[j]) = color;
		}
	}
	imshow("Line", show3u);

	//cv::namedWindow("Derivatives", 0);
	imshow("Derivatives", dEdge.GetDer());
	
	//cv::namedWindow("Derivatives2", 0);
	imshow("Derivatives2", dEdge.GetDer2());
	cv::Mat mask01;
	mask01.create(img1u.rows + 2, img1u.cols + 2, CV_8UC1);
	mask01 = cv::Scalar::all(0);
	cv::Mat Der2 = dEdge.GetDer2();
	
	Dilation(Der2, 1, 1);
	loDiff = 10;
	upDiff = 10;

	for (int i = 1; i < img1u.rows - 1; i++)
	{
		for (int j = 1; j < img1u.cols - 1; j++)
		{
			MyfloodFill(j, i, mask01, Der2, cImg2);
		}
	}

	imshow("Image0", cImg);
	Erosion(mask01, 1, 2);

	for (int i = 1; i < mask01.rows - 1; i++)
	{
		for (int j = 1; j < mask01.cols - 1; j++)
		{
			uchar& v = mask01.at<uchar>(i, j);
			uchar& v2 = Der2.at<uchar>(i - 1, j - 1);

			if (v > 0)
			{
				v2 = 0;
			}
			else
			{
				v2 = 255;
			}
		}
	}

//	imshow("mask0", Der2);
	cImg2 = cImg.clone();
	mask01 = cv::Scalar::all(0);
	loDiff = 20;
	upDiff = 20;

	for (int i = 1; i < img1u.rows - 1; i++)
	{
		for (int j = 1; j < img1u.cols - 1; j++)
		{
			MyfloodFill(j, i, mask01, Der2, cImg2);
		}
	}

// 	imshow("mask", mask01);
// 	imshow("Image", cImg2);

	for (int i = 1; i < cImg3.rows - 1; i++)
	{
		for (int j = 1; j < cImg3.cols - 1; j++)
		{
			uchar& v = mask01.at<uchar>(i, j);

			if (v > 0)
			{
				cv::Vec3b& c = cImg3.at<cv::Vec3b>(i - 1, j - 1);
				c[0] = 255;
				c[1] = 255;
				c[2] = 255;
			}
		}
	}
// 	for (int i = 1; i < cImg3.rows - 1; i++)
// 	{
// 		for (int j = 1; j < cImg3.cols - 1; j++)
// 		{
// 			cv::Vec3b& v = cImg3.at<cv::Vec3b>(i, j);
// 			float lv = v[0] * 0.299 + v[1] * 0.587 + v[2] * 0.114;
// 
// 			if (v[0] < 120 || v[1] < 120 || v[2] < 120)
// 			{
// 
// 			}
// 			else
// 			{
// 				v[0] = 255;
// 				v[1] = 255;
// 				v[2] = 255;
// 			}
// 		}
// 	}

//	imshow("Image2", cImg3);
	cv::waitKey(0);
}
