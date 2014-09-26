#include "stdafx.h"
#include "VAV_MainFrm.h"
#include "VAV_View.h"
#include "ConvStr.h"
#include "Line.h"
#include "CvExtenstion0.h"
#include "CvExtenstion.h"
#include "CvExtenstion2.h"
#include "IFExtenstion.h"
#include "CmCurveEx.h"
#include <windows.h>
#include "cvshowEX.h"
#include "TriangulationCgal_Patch.h"
#include "TriangulationCgal_Sideline.h"

#include <fstream>
#include <iostream>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "LineDef.h"
#include <boost/timer/timer.hpp>
#include "GenerateDiffusion.h"

cv::Mat VAV_MainFrame::Do_SLIC(double m_compatcness, double m_spcount, cv::Mat img)
{
	using namespace cv;
	m_SLIC_Img = img.clone();
	m_SLIC = new MySLIC(m_SLIC_Img);
	double compactness = m_compatcness; //40.0; /// 1~40
	const int Width = m_SLIC_Img.cols;
	const int Height = m_SLIC_Img.rows;
	int size = Width * Height;
	int spcount = (int)log((double)size) * m_spcount ;/*100*/ //150;//
	m_LabelsMap.resize(size);
	m_Labels.clear();
	m_LPatchs.clear();
	m_NumLabels = 0;
	///RGB轉UINT  因SLIC中所使用的顏色為ARGB，故先將原圖形做轉換後存至img以利計算
	m_Buff.resize(size);
	for(int j = 0; j < Height; j++)
	{
		for(int i = 0; i < Width; i++)
		{
			cv::Vec3b salColor = m_SLIC_Img.at<cv::Vec3b>(j, i);   //CV_IMAGE_ELEM(image,element type,row,col)
			m_Buff[j * Width + i] = salColor[0] + salColor[1] * 256 + salColor[2] * 256 * 256;
		}
	}
	int* lm = &m_LabelsMap[0];
	///--------SLIC -> previous version--------
	m_SLIC->DoSuperpixelSegmentation_ForGivenSuperpixelSize(lm, m_Labels, m_NumLabels,
			spcount, compactness);
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			m_SLIC->FindNeighbors(m_Labels, lm);
		}
		#pragma omp section
		{
			m_SLIC->FindBoxBoundary(m_Labels);
		}
	}
	m_SLIC_Contour.resize(size, 0);
	unsigned int* slicC = &m_SLIC_Contour[0];
	m_SLIC->DrawContoursAroundSegments(slicC, lm, Width, Height, 0, 0);
	for(int i = 1; i < Height - 1; i++)
	{
		for(int j = 1; j < Width - 1; j++)
		{
			if(m_SLIC_Contour[i * Width + j] != 0)
			{
				m_SLIC_Img.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
			}
		}
	}
	m_SLIC->ComputePatchFromColor(m_Labels, m_LPatchs, 10);
	cv::Mat res = img.clone();
	//m_ColorConstraints = m_SLIC->BuildColorConstraints(m_Labels, m_LPatchs, res);
	for(int i = 0; i < m_LPatchs.size(); ++i)
	{
		RGBColor rgb = LAB2RGB(m_Labels[m_LPatchs[i].idxs[0]].seed_lab);
		cv::Vec3b color(rgb.r, rgb.g, rgb.b);
		for(int j = 0; j < m_LPatchs[i].idxs.size(); ++j)
		{
			int lid = m_LPatchs[i].idxs[j];
			for(int k = 0; k < m_Labels[lid].pixels.size(); ++k)
			{
				MySLIC::Pixel& p = m_Labels[lid].pixels[k];
				m_SLIC_Img.at<cv::Vec3b>(p.y, p.x) = color;
			}
		}
	}
	//g_cvshowEX.AddShow("slic img", m_SLIC_Img);
	return res;
}

cv::Mat markerMask, img, oriimg, g_region;
cv::Point prevPt(-1, -1);
ColorConstraints ccms3;
CvPatchs cps;

static void onMouse(int event, int x, int y, int flags, void*)
{
	printf("x %d y %d\n", x, y);
	if(x < 0 || x >= img.cols || y < 0 || y >= img.rows)
	{
		return;
	}
	img = oriimg.clone();
	if(event == CV_EVENT_LBUTTONDOWN)
	{
		for(int cc = 1; cc < cps.size(); ++cc)
		{
			if(cv::pointPolygonTest(cps[cc].Outer2(), cv::Point2f(x, y), false) > 0)
			{
				markerMask = cv::Scalar(0);
				CvLines tlines;
				tlines.push_back(cps[cc].Outer2());
				drawContours(markerMask, tlines, 0, cv::Scalar(255), -1, 8);
				for(int i = 0; i < img.rows; ++i)
				{
					for(int j = 0; j < img.cols; ++j)
					{
						if(markerMask.at<uchar>(i, j))
						{
							img.at<cv::Vec3b>(i, j) = ccms3[cc].GetColorCvPoint(j, i);
						}
					}
				}
				for(int i = 0; i < img.rows; ++i)
				{
					for(int j = 0; j < img.cols; ++j)
					{
						if(g_region.at<cv::Vec3b>(i, j)[0])
						{
							markerMask.at<uchar>(i, j) = 128;
						}
					}
				}
				g_cvshowEX.AddShow("hello", img);
				g_cvshowEX.AddShow("markerMask", markerMask);
				break;
			}
		}
	}
}

bool checkmask3x3(cv::Mat img, int x, int y)
{
	for(int a = y - 1; a <= y + 1; ++a)
	{
		for(int b = x - 1; b <= x + 1; ++b)
		{
			if(a >= 0 && b >= 0 && a < img.rows && b < img.cols)
			{
				if(img.at<cv::Vec3b>(a, b)[2] == 0)
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool drawmask5x5(cv::Mat img, int x, int y, cv::Vec3b color, cv::Mat dst)
{
	for(int a = y - 1; a <= y + 1; ++a)
	{
		for(int b = x - 1; b <= x + 1; ++b)
		{
			if(a >= 0 && b >= 0 && a < dst.rows && b < dst.cols)
			{
				//if(img.at<cv::Vec3b>(a, b)[2] != 0)
				{
					dst.at<cv::Vec3b>(a, b) = color;
				}
			}
		}
	}
	return true;
}


void VAV_MainFrame::OnButton_BuildVectorization()
{	
	D3DApp& d3dApp = GetVavView()->GetD3DApp();
	FrameInfo fi = ComputeFrameFG(m_vavImage, m_vavImage);
	SetDrawFrame(d3dApp, fi);
	GetVavView()->OnDraw(0);
	return;
	static int iinit = 0;
	if(!iinit)
	{
		iinit = 1;
		g_cvshowEX.Init();
	}
	cv::imshow("Origin Image", m_vavImage.GetCvMat());
	Lines decorativeLine;
	vavImage expImg = WhiteBalance(m_vavImage.Clone());
	expImg = ImgSharpen(expImg);
	cv::waitKey();
	if(0)
	{
		cv::Mat edgesImg, src_gray = m_vavImage.Clone(), src_grayf,
						  oriImg = m_vavImage.Clone(), show_ccp;
		CmCurveEx* dcurve = NULL;
		{
			// curve extration
			cvtColor(src_gray, src_gray, CV_BGR2GRAY);
			// find decorative curve
			src_gray.convertTo(src_grayf, CV_32F, 1.0 / 255);
			dcurve = new CmCurveEx(src_grayf);
			dcurve->CalSecDer(5, 0.001f);
			dcurve->Link();
			show_ccp = m_vavImage.Clone();
			show_ccp = cv::Scalar(0);
			const std::vector<CEdge>& edges = dcurve->GetEdges();
			for(size_t i = 0; i < edges.size(); i++)
			{
				const std::vector<cv::Point>& pnts = edges[i].pnts;
				for(size_t j = 0; j < pnts.size(); j++)
				{
					show_ccp.at<cv::Vec3b>(pnts[j]) = cv::Vec3b(0, 0, 255);
				}
			}
		}
		{
			// canny
			/// Reduce noise with a kernel 3x3
			edgesImg = src_gray.clone();
			cv::blur(src_gray, edgesImg, cv::Size(3, 3));
			/// Canny detector
			int ratio = 3;
			int kernel_size = 3;
			int lowThreshold = 18;
			cv::Canny(edgesImg, edgesImg, lowThreshold, lowThreshold * ratio, kernel_size);
		}
		{
			// 修 decorative curve
			cv::Vec3b red(0, 0, 255), white(255, 255, 255), black(0, 0, 0);
			cvtColor(edgesImg, edgesImg, CV_GRAY2BGR);
			g_cvshowEX.AddShow("edgesImg", edgesImg);
			for(int i = 1; i < show_ccp.rows - 1; i++)
			{
				for(int j = 1; j < show_ccp.cols - 1 ; j++)
				{
					cv::Vec3b& v1 = show_ccp.at<cv::Vec3b>(i, j);
					cv::Vec3b& v2 = edgesImg.at<cv::Vec3b>(i, j);
					if(v2 == white && v1[2] == 0)
					{
						v1 = white;
					}
				}
			}
			g_cvshowEX.AddShow("dcurve0", show_ccp);
			for(int n = 0; n < 2; ++n)
			{
				for(int i = 1; i < show_ccp.rows - 1; i++)
				{
					for(int j = 1; j < show_ccp.cols - 1 ; j++)
					{
						cv::Vec3b& v = show_ccp.at<cv::Vec3b>(i, j);
						if(v == black)
						{
							{
								cv::Vec3b& v1 = show_ccp.at<cv::Vec3b>(i, j + 1);
								cv::Vec3b& v2 = show_ccp.at<cv::Vec3b>(i, j - 1);
								if((v1 == red && v2 == white)
										|| (v2 == red && v1 == white)
										|| (v2 == white && v1 == white)
										|| (v2 == red && v1 == red))
								{
									v = white;
								}
							}
							{
								cv::Vec3b& v1 = show_ccp.at<cv::Vec3b>(i + 1, j);
								cv::Vec3b& v2 = show_ccp.at<cv::Vec3b>(i - 1, j);
								if((v1 == red && v2 == white)
										|| (v2 == red && v1 == white)
										|| (v2 == white && v1 == white)
										|| (v2 == red && v1 == red))
								{
									v = white;
								}
							}
						}
					}
				}
			}
			g_cvshowEX.AddShow("dcurve1", show_ccp);
		}
		ints2d isdcurve(dcurve->GetEdges().size());
		{
			// detection decorative curve
			const std::vector<CEdge>& edges = dcurve->GetEdges();
			for(size_t i = 0; i < edges.size(); i++)
			{
				const std::vector<cv::Point>& pnts = edges[i].pnts;
				isdcurve[i].resize(pnts.size());
				for(size_t j = 0; j < pnts.size(); j++)
				{
					if(checkmask3x3(show_ccp, pnts[j].x, pnts[j].y))
					{
						isdcurve[i][j] = 1;
					}
				}
			}
			cv::Vec3b linecolor(255, 255, 255);
			for(size_t i = 0; i < edges.size(); i++)
			{
				const std::vector<cv::Point>& pnts = edges[i].pnts;
				int pcount = 0;
				for(size_t j = 0; j < pnts.size(); j++)
				{
					if(isdcurve[i][j])
					{
						pcount++;
						drawmask5x5(show_ccp, pnts[j].x, pnts[j].y, linecolor, edgesImg);
					}
				}
				if(pcount * 2 > pnts.size())
				{
					for(size_t j = 0; j < pnts.size(); j++)
					{
						isdcurve[i][j] = 1;
						drawmask5x5(show_ccp, pnts[j].x, pnts[j].y, linecolor, edgesImg);
					}
				}
			}
			g_cvshowEX.AddShow("dcurve2", show_ccp);
			g_cvshowEX.AddShow("edgesImg0", edgesImg);
		}
		edgesImg = TrapBallMaskAll(edgesImg, oriImg);
		g_cvshowEX.AddShow("TrapBallMaskAll", edgesImg);
		edgesImg = FixSpaceMask(edgesImg);
		g_cvshowEX.AddShow("FixSpaceMask", edgesImg);
		edgesImg = MixTrapBallMask(edgesImg, oriImg, 30, 40);
		g_cvshowEX.AddShow("MixTrapBallMask", edgesImg);
		{
//          for(size_t i = 0; i < isdcurve.size(); i++)
//          {
//              int mlen = 5;
//              for(size_t j = 0; j < isdcurve[i].size(); j++)
//              {
//                  if(isdcurve[i][j]==0)
//                  {
//                      bool lok = false, rok = false;
//                      for(int q = 1; q < mlen; ++q)
//                      {
//                          if(isdcurve[i][j - q])
//                          {
//                              lok = true;
//                          }
//                          if(isdcurve[i][j + q])
//                          {
//                              rok = true;
//                          }
//                      }
//                      if(rok && lok)
//                      {
//                          isdcurve[i][j] = 1;
//                      }
//                  }
//              }
//          }
			const std::vector<CEdge>& edges = dcurve->GetEdges();
			cv::Vec3b linecolor(rand() % 256, rand() % 256, rand() % 200 + 55);
			for(size_t i = 0; i < edges.size(); i++)
			{
				linecolor = cv::Vec3b(rand() % 256, rand() % 256, rand() % 200 + 55);
				const std::vector<cv::Point>& pnts = edges[i].pnts;
				for(size_t j = 0; j < pnts.size(); j++)
				{
					if(isdcurve[i][j])
					{
						drawmask5x5(show_ccp, pnts[j].x, pnts[j].y, linecolor, edgesImg);
					}
					else
					{
					}
				}
			}
		}
		edgesImg = FixSpaceLine(edgesImg, oriImg);
		cv::Mat show_trap = edgesImg.clone();
		FloodFillReColor(show_trap);
		g_cvshowEX.AddShow("show_trap", show_trap);
		ImageSpline is = S4GetPatchs(edgesImg, 0, 0);
		Lines colorline;
		for(int i = 0; i < is.m_LineFragments.size(); ++i)
		{
			colorline.push_back(is.m_LineFragments[i].m_Points);
		}
		ColorConstraints ccms0;
		cv::Mat size2 = m_vavImage.Clone();
		cv::resize(edgesImg.clone(), edgesImg, edgesImg.size() * 2, 0, 0, cv::INTER_NEAREST);
		S6ReColor(edgesImg, m_vavImage.Clone(), ccms0);
		//DrawColorLineImage(cmmsIndexImg, colorline);
		cv::Mat colormask = edgesImg.clone();
		FloodFillReColor(colormask);
		g_cvshowEX.AddShow("edgesImg", colormask);
		cv::imwrite("cmmsIndexImg.png", colormask);
		Index2Side i2s = GetLinesIndex2SideSmart(edgesImg, colorline, ccms0);
		i2s.left = FixIndexs(i2s.left, 100);
		i2s.right = FixIndexs(i2s.right, 100);
		i2s.left = MedianLenN(i2s.left, 10);
		i2s.right = MedianLenN(i2s.right, 10);
		Color2Side color2s2 = LinesIndex2Color(colorline, i2s, ccms0);
		d3dApp.AddDiffusionLines(colorline, color2s2);
		d3dApp.AddLines(colorline);
		return;
	}
	const int MASK1_SIZE = 5;
	const int MASK2_SIZE = 5;
	const float secDer = 0.001f;
//  g_cvshowEX.AddShow("Origin Image", m_vavImage.GetCvMat());
//  g_cvshowEX.AddShow("Exponential Image", expImg.GetCvMat());
	g_cvshowEX.AddShow("Origin Image", m_vavImage.GetCvMat());
	g_cvshowEX.AddShow("Exponential Image", expImg.GetCvMat());
	cv::Mat ccp1 = m_vavImage.Clone(), ccp2 = expImg.Clone();
	cv::Mat slicimg;
	cv::Mat regionColor = m_SLIC_Img;
	CmCurveEx* ccp1_curve = NULL;
	CmCurveEx* ccp2_curve = NULL;
	cv::Mat Bblack_Fred1;
	if(m_CONSTRAINT_CURVES_PARAMETER_1)
	{
		cvtColor(ccp1, ccp1, CV_BGR2GRAY);
		ccp1.convertTo(ccp1, CV_32F, 1.0 / 255);
		ccp1_curve = new CmCurveEx(ccp1);
		ccp1_curve->CalSecDer(MASK1_SIZE, secDer, 0.05);
		ccp1_curve->Link();
		cv::Mat show_ccp1 = expImg.Clone();
		Bblack_Fred1 = expImg.Clone();
		Bblack_Fred1 = cv::Scalar(0);
		const std::vector<CEdge>& edges = ccp1_curve->GetEdges();
		for(size_t i = 0; i < edges.size(); i++)
		{
			const std::vector<cv::Point>& pnts = edges[i].pnts;
			for(size_t j = 0; j < pnts.size(); j++)
			{
				show_ccp1.at<cv::Vec3b>(pnts[j]) = cv::Vec3b(0, 0, 255);
				Bblack_Fred1.at<cv::Vec3b>(pnts[j]) = cv::Vec3b(0, 0, 255);
			}
		}
		g_cvshowEX.AddShow("Constraint Curves Parameter 1_0", show_ccp1);
		g_cvshowEX.AddShow("Constraint Curves Parameter 1_1", Bblack_Fred1);
	}
	if(m_CONSTRAINT_CURVES_PARAMETER_1 &&
			m_DECORATIVE_CURVES)
	{
		CvLines tpnts2d;
		const CEdges& edges = ccp1_curve->GetEdges();
		for(size_t i = 0; i < edges.size(); i++)
		{
			tpnts2d.push_back(edges[i].pnts);
		}
		m_BlackLine = GetLines(tpnts2d, 0.5, 0.5);
		m_BlackLine = SmoothingLen5(m_BlackLine);
		Lines showLines;
		Lines BLineWidth(m_BlackLine.size());
		Lines normals = GetNormalsLen2(m_BlackLine);
		const double blackRadio = 0.5;
		for(int idx1 = 0; idx1 < m_BlackLine.size(); ++idx1)
		{
			const Line& nowLine = m_BlackLine[idx1];
			const Line& nowNormals = normals[idx1];
			Line& lineWidths = BLineWidth[idx1];
			lineWidths.clear();
			for(int idx2 = 0; idx2 < nowLine.size() - 1; ++idx2)
			{
				const double LINE_WIDTH = 5;
				Vector2 start(nowLine[idx2] - nowNormals[idx2] * LINE_WIDTH);
				Vector2 end(nowLine[idx2] + nowNormals[idx2] * LINE_WIDTH);
				Vector2 start2(nowLine[idx2 + 1] - nowNormals[idx2 + 1] * LINE_WIDTH);
				Vector2 end2(nowLine[idx2 + 1] + nowNormals[idx2 + 1] * LINE_WIDTH);
				double_vector line1 = expImg.GetLineLight(start.x, start.y, end.x, end.y,
									  360);
				double_vector line2 = expImg.GetLineLight(start2.x, start2.y, end2.x, end2.y,
									  360);
				double_vector width1 = GetLineWidth(ConvertToSquareWave(ConvertToAngle(line1),
													15, 50), LINE_WIDTH * 2);
				double_vector width2 = GetLineWidth(ConvertToSquareWave(ConvertToAngle(line2),
													15, 50), LINE_WIDTH * 2);
				if(width1.size() >= 2 && width2.size() >= 2 && abs(width2[0] - width2[1]) < 1.5)
				{
					Line line1;
					line1.push_back(nowLine[idx2] - nowNormals[idx2] * width1[0] * blackRadio);
					line1.push_back(nowLine[idx2 + 1] - nowNormals[idx2 + 1] * width2[0] *
									blackRadio);
					line1 = GetLine(line1, 0.5, 0.5);
					Line line2;
					line2.push_back(nowLine[idx2] + nowNormals[idx2] * width1[1] * blackRadio);
					line2.push_back(nowLine[idx2 + 1] + nowNormals[idx2 + 1] * width2[1] *
									blackRadio);
					line2 = GetLine(line2, 0.5, 0.5);
					showLines.push_back(line1);
					showLines.push_back(line2);
					// save line width
					lineWidths.push_back(Vector2(width1[0] * blackRadio, width1[1] * blackRadio));
				}
				else
				{
					lineWidths.push_back(Vector2());
				}
			}
			lineWidths.push_back(Vector2());
		}
		m_BLineWidth = BLineWidth;
		m_BlackLine = GetLines(tpnts2d, 0.5, 0.5);
		m_BlackLine = SmoothingLen5(m_BlackLine, 0.9, 5);
		LineEnds les = GetLineEnds(m_BlackLine);
		LinkLineEnds180(les, 5, 15);
		ConnectLineEnds(les, m_BlackLine, m_BLineWidth);
		IncreaseDensity(m_BlackLine, m_BLineWidth);
		les = GetLineEnds(m_BlackLine);
		// 連結角度類似的線
		ConnectNearestLines_Width(les, m_BlackLine, m_BLineWidth, 5, 25);
		m_BLineWidth = CleanOrphanedLineWidths(m_BLineWidth, 5);
		m_BLineWidth = FixLineWidths(m_BLineWidth, 50);
		//m_BLineWidth = FixLineWidths(m_BLineWidth, 100);
		ClearLineWidthByPercent(m_BLineWidth, 0.6);
		m_BLineWidth = FixLineWidths(m_BLineWidth, 200);
		m_BLineWidth = SmoothingHas0Len5(m_BLineWidth, 0, 5);
		m_BLineWidth = FixedLineWidth(m_BLineWidth, 5);
		//m_BlackLine = SmoothingLen5(m_BlackLine, 0, 5);
		d3dApp.AddLines(m_BlackLine);
		vavImage tmp = m_vavImage.Clone();
		tmp.GetCvMat() = cv::Scalar(255, 255, 255);
		GetVavView()->SetTexture(tmp.GetDx11Texture());
		d3dApp.SetScaleTemporary(1);
		d3dApp.BuildPoint();
		d3dApp.InterSetRenderTransparencyOutput2();
		cv::Mat simg = d3dApp.DrawSceneToCvMat();
		simg = cv::Scalar(255, 255, 255);
		for(int i = 0; i < m_BlackLine.size(); i++)
		{
			for(int j = 0; j < m_BlackLine[i].size() ; j++)
			{
				Vector2 p = m_BlackLine[i][j];
				cv::Vec3b& v = simg.at<cv::Vec3b>(p.y, p.x);
				if(m_BLineWidth[i][j][0] > 0.2)
				{
					v = cv::Vec3b(0, 0, 0);
				}
			}
		}
		cv::Mat visualize = m_vavImage.Clone();
		for(int i = 0; i < Bblack_Fred1.rows; i++)
		{
			for(int j = 0; j < Bblack_Fred1.cols ; j++)
			{
				cv::Vec3b& v = Bblack_Fred1.at<cv::Vec3b>(i, j);
				if(v[0] > 0 || v[2] > 0)
				{
					cv::Vec3b& sam = visualize.at<cv::Vec3b>(i, j);
					sam = v;
				}
			}
		}
		g_cvshowEX.AddShow("decorative Curves_0", simg);
		g_cvshowEX.AddShow("decorative Curves_1", visualize);
		d3dApp.SetScaleRecovery();
		d3dApp.ClearSkeletonLines();
		GetVavView()->SetTexture(m_vavImage.GetDx11Texture());
	}
	cv::Mat decorative_Curves_2;
	Vector3s2d lineColors;
	if(m_CONSTRAINT_CURVES_PARAMETER_1 &&
			m_DECORATIVE_CURVES &&
			m_BLACK_LINE_VECTORIZATION)
	{
		lineColors = GetLinesColor(m_vavImage, m_BlackLine);
		lineColors = SmoothingLen5(lineColors, 0, 10);
		m_BLineWidth = SmoothingLen5(m_BLineWidth, 0, 10);
		m_BlackLine = SmoothingLen5(m_BlackLine, 0, 15);
		d3dApp.AddLinesWidth(m_BlackLine, m_BLineWidth, lineColors);
		d3dApp.SetScaleTemporary(2);
		d3dApp.BuildPoint();
		d3dApp.InterSetRenderTransparencyOutput3();
		decorative_Curves_2 = d3dApp.DrawSceneToCvMat(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		for(int j = 0; j < decorative_Curves_2.rows; j++)
		{
			for(int i = 0; i < decorative_Curves_2.cols; i++)
			{
				if(decorative_Curves_2.at<cv::Vec3b>(j, i) == cv::Vec3b(0, 0, 0))
				{
					decorative_Curves_2.at<cv::Vec3b>(j, i) = cv::Vec3b(255, 255, 255);
				}
			}
		}
		d3dApp.SetScaleRecovery();
		g_cvshowEX.AddShow("decorative Curves_2", decorative_Curves_2);
	}
	cv::Mat Bwhith_Fblue2;
	if(m_CONSTRAINT_CURVES_PARAMETER_2)
	{
		cvtColor(ccp2, ccp2, CV_BGR2GRAY);
		ccp2.convertTo(ccp2, CV_32F, 1.0 / 255);
		ccp2_curve = new CmCurveEx(ccp2);
		ccp2_curve->CalSecDer2(MASK2_SIZE, secDer);
		ccp2_curve->Link();
		cv::Mat show_ccp2 = expImg.Clone();
		Bwhith_Fblue2 = expImg.Clone();
		Bwhith_Fblue2 = cv::Scalar(0);
		const std::vector<CEdge>& edges = ccp2_curve->GetEdges();
		for(size_t i = 0; i < edges.size(); i++)
		{
			const std::vector<cv::Point>& pnts = edges[i].pnts;
			for(size_t j = 0; j < pnts.size() - 1; j++)
			{
				cv::line(show_ccp2, cv::Point(pnts[j].x, pnts[j].y),
						 cv::Point(pnts[j + 1].x, pnts[j + 1].y), cv::Scalar(0, 0, 255));
				//show_ccp2.at<cv::Vec3b>(pnts[j]) = cv::Vec3b(0, 0, 255);
				Bwhith_Fblue2.at<cv::Vec3b>(pnts[j]) = cv::Vec3b(0, 0, 255);
			}
		}
		g_cvshowEX.AddShow("Constraint Curves Parameter 2_0", show_ccp2);
		g_cvshowEX.AddShow("Constraint Curves Parameter 2_1", Bwhith_Fblue2);
	}
	cv::Mat ccp2_simg;
	if(m_CONSTRAINT_CURVES_PARAMETER_2 &&
			m_BOUNDARY_CURVES)
	{
		CvLines tpnts2d;
		const CEdges& edges2 = ccp2_curve->GetEdges();
		for(size_t i = 0; i < edges2.size(); i++)
		{
			tpnts2d.push_back(edges2[i].pnts);
		}
		m_BlackLine2 = GetLines(tpnts2d, 0.5, 0.5);
		Lines tmp_width = m_BlackLine2;
		m_BlackLine2 = SmoothingLen5(m_BlackLine2, 0.5, 5);
		cv::Mat colorline0 = MakeColorLineImage(m_vavImage, m_BlackLine2);
		g_cvshowEX.AddShow("colorline0", colorline0);
		LineEnds les = GetLineEnds(m_BlackLine2);
		LinkLineEnds180(les, 5, 60);
		ConnectLineEnds3_Width(les, m_BlackLine2, tmp_width);
		les = GetLineEnds(m_BlackLine2);
		LinkLineEnds180(les, 8, 60);
		ConnectLineEnds3_Width(les, m_BlackLine2, tmp_width);
//      les = GetLineEnds(m_BlackLine2);
//      LinkLineEnds180(les, 15, 60);
//      ConnectLineEnds3_Width(les, m_BlackLine2, tmp_width);
//      les = GetLineEnds(m_BlackLine2);
//      LinkLineEnds180(les, 20, 60);
//      ConnectLineEnds3_Width(les, m_BlackLine2, tmp_width);
		//無條件黏接
//      les = GetLineEnds(m_BlackLine2);
//      LinkLineEnds(les, 4, 180);
//      ConnectLineEnds3(les, m_BlackLine2, tmp_width);
		IncreaseDensity(m_BlackLine2, tmp_width);
		les = GetLineEnds(m_BlackLine2);
		//ConnectNearestLines(les, m_BlackLine2, 8, 25);
		ConnectNearestLinesColorCheck(les, m_BlackLine2, 12, 20, m_vavImage);
//      les = GetLineEnds(m_BlackLine2);
//      ConnectNearestLines(les, m_BlackLine2, 5, 30);
		m_BlackLine2 = LineSplitAtEndIntersection(m_BlackLine2, 1);
		tmp_width = m_BlackLine2;
		les = GetLineEnds(m_BlackLine2);
		LinkLineEnds(les, 3, 360);
		ConnectLineEnds5(les, m_BlackLine2);
		les = GetLineEnds(m_BlackLine2);
		LinkLineEnds(les, 3, 360);
		ConnectLineEnds5(les, m_BlackLine2);
//      cv::Mat LineSplit = MakeColorLineImage(m_vavImage, m_BlackLine2);
//      g_cvshowEX.AddShow("before LineSplit", LineSplit);
//      m_BlackLine2 = LineSplitAtTurning(m_BlackLine2, 5);
//      LineSplit = MakeColorLineImage(m_vavImage, m_BlackLine2);
//      g_cvshowEX.AddShow("after LineSplit", LineSplit);
//      tmp_width = m_BlackLine2;
		//
//      les = GetLineEnds(m_BlackLine2);
//      LinkLineEnds(les, 3, 180);
//      ConnectLineEnds3(les, m_BlackLine2, tmp_width);
//      les = GetLineEnds(m_BlackLine2);
//      ConnectLineEnds3(les, m_BlackLine2, tmp_width);
		m_BlackLine2 = SmoothingLen5(m_BlackLine2, 0.5, 8);
		d3dApp.AddLines(m_BlackLine2);
		d3dApp.SetScaleTemporary(1);
		d3dApp.BuildPoint();
		d3dApp.InterSetRenderTransparencyOutput2();
		ccp2_simg = d3dApp.DrawSceneToCvMat();
		d3dApp.SetScaleRecovery();
//      les = GetLineEnds(m_BlackLine2);
//      for(int i = 0; i < les.size(); ++i)
//      {
//          Vector2 pb = les[i].beg;
//          Vector2 pe = les[i].end;
//          ccp2_simg.at<cv::Vec3b>(pb.y, pb.x) = cv::Vec3b(0, 255, 0);
//          ccp2_simg.at<cv::Vec3b>(pe.y, pe.x) = cv::Vec3b(255, 128, 0);
//      }
		for(int j = 0; j < ccp2_simg.rows; j++)
		{
			for(int i = 0; i < ccp2_simg.cols; i++)
			{
				if(ccp2_simg.at<cv::Vec3b>(j, i) == cv::Vec3b(0, 0, 0))
				{
					ccp2_simg.at<cv::Vec3b>(j, i) = cv::Vec3b(255, 255, 255);
				}
				else
				{
					ccp2_simg.at<cv::Vec3b>(j, i) = cv::Vec3b(0, 55, 0);
				}
			}
		}
		g_cvshowEX.AddShow("boundary curves", ccp2_simg);
		cv::Mat region = m_vavImage.Clone();
		cv::resize(region, region, region.size() * 2);
		for(int j = 0; j < region.rows; j++)
		{
			for(int i = 0; i < region.cols; i++)
			{
				if(decorative_Curves_2.at<cv::Vec3b>(j, i) != cv::Vec3b(255, 255, 255))
				{
					region.at<cv::Vec3b>(j, i) = cv::Vec3b(255, 255, 255);
				}
			}
		}
		cv::Mat show_ccp2 = expImg.Clone();
		for(size_t i = 0; i < m_BlackLine2.size(); i++)
		{
			const Line& pnts = m_BlackLine2[i];
			for(size_t j = 0; j < pnts.size() - 1; j++)
			{
				cv::line(show_ccp2, cv::Point(pnts[j].x, pnts[j].y),
						 cv::Point(pnts[j + 1].x, pnts[j + 1].y), cv::Scalar(0, 0, 255));
				//show_ccp2.at<cv::Vec3b>(pnts[j].y, pnts[j].x) = cv::Vec3b(0, 0, 255);
			}
		}
		g_cvshowEX.AddShow("Constraint Curves Parameter 2_2", show_ccp2);
//      DrawWhiteLineImage(region, m_BlackLine);
//      {
//          cv::Mat refimg = m_vavImage.Clone();
//          cv::resize(refimg, refimg, refimg.size() * 2);
//          for (int i = 0; i < m_BlackLine2.size(); ++i)
//          {
//              double var = GetLineColorVariance(m_BlackLine2[i], refimg);
//              if (var > 60)
//              {
//                  cv::Mat black = refimg.clone();
//                  printf("pts:%d  var:%f\n", m_BlackLine2[i].size(), var);
//                  cv::Vec3b cc;
//                  cc[0] = 0;
//                  cc[1] = 0;
//                  cc[2] = 255;
//                  Lines::const_iterator it = m_BlackLine2.begin() + i;
//                  {
//                      for (int j = 0; j < it->size() - 1; ++j)
//                      {
//                          cv::Point p1((*it)[j].x * 2, (*it)[j].y * 2);
//                          cv::Point p2((*it)[j + 1].x * 2, (*it)[j + 1].y * 2);
//                          cv::line(black, p1, p2, cv::Scalar(cc), 1);
//                      }
//                  }
//                  cv::imshow("line", black);
//                  cv::Mat linecolor = ShowColorToLine(m_BlackLine2[i], refimg);
//                  cv::imshow("linec", linecolor);
//                  cv::waitKey();
//              }
//          }
//      }
		g_cvshowEX.AddShow("region1", region);
		DrawWhiteLineImage(region, m_BlackLine2);
		g_cvshowEX.AddShow("region2", region);
		region = cv::Scalar(0);
		for(int j = 0; j < region.rows; j++)
		{
			region.at<cv::Vec3b>(j, 0) = cv::Vec3b(255, 255, 255);
			region.at<cv::Vec3b>(j, region.cols - 1) = cv::Vec3b(255, 255, 255);
			region.at<cv::Vec3b>(j, 1) = cv::Vec3b(255, 255, 255);
			region.at<cv::Vec3b>(j, region.cols - 2) = cv::Vec3b(255, 255, 255);
		}
		for(int i = 0; i < region.cols; i++)
		{
			region.at<cv::Vec3b>(0, i) = cv::Vec3b(255, 255, 255);
			region.at<cv::Vec3b>(region.rows - 1, i) = cv::Vec3b(255, 255, 255);
			region.at<cv::Vec3b>(1, i) = cv::Vec3b(255, 255, 255);
			region.at<cv::Vec3b>(region.rows - 2, i) = cv::Vec3b(255, 255, 255);
		}
		DrawWhiteLineImage(region, m_BlackLine);
		DrawWhiteLineImage(region, m_BlackLine2);
		cv::Mat bndy = cv::imread("153_bndy.png");
		for(int j = 0; j < bndy.rows; j++)
		{
			for(int i = 0; i < bndy.cols; i++)
			{
				if(bndy.at<cv::Vec3b>(j, i) == cv::Vec3b(255, 255, 255))
				{
					bndy.at<cv::Vec3b>(j, i) = cv::Vec3b(0, 0, 0);
				}
				else
				{
					bndy.at<cv::Vec3b>(j, i) = cv::Vec3b(1, 1, 1);
				}
			}
		}
		cvtColor(bndy, bndy, CV_BGR2GRAY);
		cvThin(bndy, bndy, 1);
		for(int j = 0; j < bndy.rows; j++)
		{
			for(int i = 0; i < bndy.cols; i++)
			{
				if(bndy.at<uchar>(j, i) == 1)
				{
					//region.at<cv::Vec3b>(j, i) = cv::Vec3b(255, 255, 255);
				}
			}
		}
		Dilation(region, 2, 2);
		Erosion(region, 2, 2);
		cvtColor(region, region, CV_BGR2GRAY);
		normalize(region, region, 0, 1, cv::NORM_MINMAX);
		cvThin(region, region, 10);
		normalize(region, region, 0, 255, cv::NORM_MINMAX);
		cvtColor(region, region, CV_GRAY2BGR);
		cv::Mat vecout, vin = m_vavImage.Clone();
		cv::resize(vin, vin, vin.size() * 2);
		oriimg = vin.clone();
		vecout = vin.clone();
		vecout = cv::Scalar(0);
//      cps = S2_3GetPatchs(region, vin, ccms3, vecout);
//      for (int j = 0; j < bndy.rows; j++)
//      {
//          for (int i = 0; i < bndy.cols; i++)
//          {
//              if (bndy.at<uchar>(j, i) == 1)
//              {
//                  for (int ci = 1; ci < cps.size() && ci < 20; ++ci)
//                  {
//                      if (cps[ci].Outer2().size() > 100 && cps[ci].Inside(i, j))
//                      {
//                          region.at<cv::Vec3b>(j, i) = cv::Vec3b(255, 255, 255);
//                      }
//                  }
//              }
//          }
//      }
//      Dilation(region, 2, 1);
//      cvtColor(region, region, CV_BGR2GRAY);
//      normalize(region, region, 0, 1, cv::NORM_MINMAX);
//      cvThin(region, region, 10);
//      normalize(region, region, 0, 255, cv::NORM_MINMAX);
//      cvtColor(region, region, CV_GRAY2BGR);
		//cps = S2_3GetPatchs(region, vin, ccms3, vecout);
		img = region.clone();
		markerMask.create(region.rows + 2, region.cols + 2, CV_8UC1);
		g_cvshowEX.AddShow("region", region);
		g_cvshowEX.AddShow("markerMask", region);
		g_region = region;
		cv::setMouseCallback("markerMask", onMouse, 0);
		cv::setMouseCallback("region", onMouse, 0);
		g_cvshowEX.AddShow("vecout", vecout);
		//cv::waitKey();
	}
	Lines isosurfaceLines;
	cv::Mat isoimg, reginimg, isolineimg;
	if(m_ISOSURFACE_REGION)
	{
		isoimg = m_vavImage.Clone();
		//cv::GaussianBlur(isoimg, isoimg, cv::Size(5, 5), 0, 0);
		isoimg = MakeIsoSurfaceImg(isoimg, 16);
		g_cvshowEX.AddShow("IsoSurface", isoimg);
	}
	ColorConstraints ccms;
	ColorConstraints ccms2;
	cv::Mat cmmsIndexImg;
	Color2SideConstraint finalcolor;
	Lines colorline;
	ColorConstraints finallinecolor;
	LineWidthConstraints finallinewidth;
	if(m_BOUNDARY_CURVES &&
			m_REGION_GROWING)
	{
		d3dApp.SetScaleTemporary(2);
		d3dApp.BuildPoint();
		d3dApp.InterSetRenderTransparencyOutput2();
		cv::Mat simg = d3dApp.DrawSceneToCvMat();
		d3dApp.SetScaleRecovery();
		cmmsIndexImg = simg.clone();
		cv::Mat size2 = m_vavImage.Clone();
		cv::resize(size2.clone(), size2, size2.size() * 2, 0, 0, cv::INTER_CUBIC);
		cmmsIndexImg = TrapBallMaskAll(cmmsIndexImg, size2);
		cmmsIndexImg = FixSpaceMask(cmmsIndexImg);
		cmmsIndexImg = MixTrapBallMask(cmmsIndexImg, size2, 30, 40);
		cv::Mat vecout = m_vavImage.Clone();
		vecout = cv::Scalar(0);
//		Dilation(simg, 1, 2);
		//cmmsIndexImg = MakeIsoSurfaceImg(m_vavImage, 32);
//      CvPatchs cps = S2_2GetPatchs(cmmsIndexImg, m_vavImage, ccms2, vecout);
//      g_cvshowEX.AddShow("vecout", vecout);
		// set draw patch line to Diffusion
//      Lines patchlines = GetLinesFromCvPatch(cps);
//      Color2Side patchColor = GetLinesColor(patchlines, ccms2);
//      patchlines = SmoothingLen5(patchlines, 0.5, 5);
//		d3dApp.AddDiffusionLines(patchlines, patchColor);
		// set click patch
//      GetVavView()->m_patchs = cps;徐蚌會戰
//      GetVavView()->m_patchcolor = patchColor;
//      GetVavView()->m_patchlines = patchlines;
		// show patch img
		cv::Mat colorline1 = MakeColorLineImage(m_vavImage, m_BlackLine2);
		colorline = LineSplitAtIntersection(m_BlackLine2, 1);
		cv::Mat colorline2 = MakeColorLineImage(m_vavImage, colorline);
		g_cvshowEX.AddShow("color line 1", colorline1);
		g_cvshowEX.AddShow("color line 2", colorline2);
		S6ReColor(cmmsIndexImg, m_vavImage.Clone(), ccms);
		//DrawColorLineImage(cmmsIndexImg, colorline);
		cv::Mat colormask = cmmsIndexImg.clone();
		FloodFillReColor(colormask);
		g_cvshowEX.AddShow("cmmsIndexImg", colormask);
		cv::imwrite("cmmsIndexImg.png", colormask);
//      cv::waitKey(10);
		Index2Side i2s = GetLinesIndex2SideSmart(cmmsIndexImg, colorline, ccms);
		i2s.left = FixIndexs(i2s.left, 100);
		i2s.right = FixIndexs(i2s.right, 100);
//      i2s.left = Maxmums2(i2s.left, colorline);
//      i2s.right = Maxmums2(i2s.right, colorline);
		i2s.left = MedianLenN(i2s.left, 10);
		i2s.right = MedianLenN(i2s.right, 10);
		Color2Side color2s2 = LinesIndex2Color(colorline, i2s, ccms);
//      color2s2.left = FixLineColors(color2s2.left, 600, 10);
//      color2s2.right = FixLineColors(color2s2.right, 600, 10);
		{
			// build model
			// convert Decorative Curves
			finallinecolor = ConvertToConstraintC(m_BlackLine, lineColors);
			lineColors = ConvertFromConstraintC(m_BlackLine, finallinecolor);
			finallinewidth = ConvertToConstraintLW(m_BlackLine, m_BLineWidth);
			m_BLineWidth = ConvertFromConstraintLW(m_BlackLine, finallinewidth);
			finalcolor = ConvertToConstraintC2(colorline, color2s2);
			color2s2 = ConvertFromConstraintC2(colorline, finalcolor);
		}
		d3dApp.AddDiffusionLines(colorline, color2s2);
		d3dApp.AddLines(m_BlackLine2);
	}
	std::ofstream ofs("curve.txt", std::ios::binary);
	if(ofs.is_open())
	{
		boost::archive::binary_oarchive oa(ofs);
		oa << finalcolor;
		oa << colorline;
		oa << m_BlackLine;
		oa << finallinecolor;
		oa << finallinewidth;
	}
	ofs.close();
	/*
	if (m_ISOSURFACE_REGION &&
	        m_ISOSURFACE_CONSTRAINT)
	{
	    d3dApp.ClearSkeletonLines();
	    cv::Mat curveExtration = ccp2_simg.clone();
	    if (m_BOUNDARY_CURVES)
	    {
	        cvtColor(curveExtration, curveExtration, CV_BGR2GRAY);
	        Dilation(curveExtration, 2, 2);
	        //          isosurfaceLines = S7GetPatchs(isoimg, curveExtration, 0, 0);
	        isosurfaceLines = S6GetPatchs(isoimg, 0, 0, reginimg);
	    }
	    else
	    {
	        isosurfaceLines = S6GetPatchs(isoimg, 0, 0, reginimg);
	    }
	    isosurfaceLines = SmoothingLen5(isosurfaceLines, 0, 2);
	    d3dApp.AddLines(isosurfaceLines);
	    d3dApp.SetScaleTemporary(1);
	    d3dApp.BuildPoint();
	    d3dApp.InterSetRenderTransparencyOutput2();
	    isolineimg = d3dApp.DrawSceneToCvMat();
	    d3dApp.SetScaleRecovery();
	    g_cvshowEX.AddShow("Isosurface Constraint0", isolineimg);
	    cvtColor(isolineimg, isolineimg, CV_BGR2GRAY);
	    if (m_BOUNDARY_CURVES)
	    {
	        for (int i = 0; i < curveExtration.rows ; i++)
	        {
	            for (int j = 0; j < curveExtration.cols ; j++)
	            {
	                uchar& v1 = curveExtration.at<uchar>(i, j);
	                uchar& v2 = isolineimg.at<uchar>(i, j);
	                if (v1 == 0 && v2 > 0)
	                {
	                    //v2 = 250;
	                }
	                else
	                {
	                    v2 = 0;
	                }
	            }
	        }
	    }
	    //      cvThin(isolineimg, isolineimg, 10);
	    //      normalize(isolineimg, isolineimg, 0, 255, cv::NORM_MINMAX);
	    g_cvshowEX.AddShow("Isosurface Constraint", isolineimg);
	    cvtColor(isolineimg, isolineimg, CV_GRAY2BGR);
	    d3dApp.SetScaleRecovery();
	}
	if (m_CONSTRAINT_CURVES_PARAMETER_2 &&
	        m_BOUNDARY_CURVES &&
	        m_BOUNDARY_CURVES_CONSTRAINT)
	{
	    cv::Mat stmp = ccp2_simg.clone();
	    cv::Mat sampleimg = m_vavImage.Clone();
	    for (int i = 0; i < sampleimg.rows; i++)
	    {
	        for (int j = 0; j < sampleimg.cols ; j++)
	        {
	            cv::Vec3b& v = stmp.at<cv::Vec3b>(i, j);
	            if (v[0] > 0)
	            {
	                cv::Vec3b& sam = sampleimg.at<cv::Vec3b>(i, j);
	                sam[0] = 0;
	                sam[1] = 0;
	                sam[2] = 0;
	            }
	        }
	    }
	    Lines ColorWidth;
	    Color2Side color2s;
	    color2s = GetLinesColor2SideSmart3(m_vavImage, sampleimg, m_BlackLine2, 3);
	    //color2s = GetLinesColor2Side(m_vavImage, m_BlackLine2, 3);
	    color2s.left = FixLineColors(color2s.left, 600, 10);
	    color2s.right = FixLineColors(color2s.right, 600, 10);
	    //d3dApp.AddDiffusionLines(m_BlackLine2, color2s);
	    d3dApp.ClearSkeletonLines();
	    d3dApp.SetScaleTemporary(1);
	    d3dApp.BuildPoint();
	    d3dApp.InterSetRenderTransparencyOutput1();
	    g_cvshowEX.AddShow("Boundary Curves constraint", d3dApp.DrawSceneToCvMat());
	    d3dApp.SetScaleRecovery();
	    d3dApp.ClearSkeletonLines();
	    d3dApp.InterSetRenderTransparencyDefault();
	}
	if (m_ISOSURFACE_REGION &&
	        m_ISOSURFACE_CONSTRAINT &&
	        m_ISOSURFACE_CONSTRAINT_VECTORIZATION &&
	        m_BOUNDARY_CURVES &&
	        m_REGION_GROWING)
	{
	    isosurfaceLines = GetAllLineFromLineImage(isolineimg);
	    d3dApp.ClearSkeletonLines();
	    d3dApp.AddLines(isosurfaceLines);
	    Vector3s2d colors;
	    colors = GetLinesColor(m_vavImage, isosurfaceLines);
	    colors = FixLineColors(colors, 400, 10);
	    colors = MedianLen(colors, 20, 3);
	    colors = SmoothingLen5(colors, 0, 10);
	    d3dApp.AddDiffusionLines(isosurfaceLines, colors);
	//      ints2d id2d = GetLinesIndex(cmmsIndexImg, isosurfaceLines);
	//      id2d = FixIndexs(id2d, 30);
	//      id2d = MedianLen5(id2d, 3);
	//      Vector3s2d color2s = LinesIndex2Color(isosurfaceLines, id2d, ccms);
	//      color2s = FixLineColors(color2s, 600, 10);
	//      for (int i = 0; i < colors.size(); ++i)
	//      {
	//          for (int j = 0; j < colors[i].size(); ++j)
	//          {
	//              color2s[i][j] = (colors[i][j] + color2s[i][j]) * 0.5;
	//          }
	//      }
	//       d3dApp.AddDiffusionLines(isosurfaceLines, color2s);
	    cv::waitKey(10);
	}
	*/
	if(0)
	{
		// ISO CONSTRAINT
		cv::Mat   isoimg = MakeIsoSurfaceImg(m_vavImage, 24);
		for(int i = 0; i < ccp2_simg.rows - 1; i++)
		{
			for(int j = 0; j < ccp2_simg.cols - 1; j++)
			{
				cv::Vec3b& v = ccp2_simg.at<cv::Vec3b>(i, j);
				if(v[0] < 128)
				{
					v[0] = 255;
					v[1] = 255;
					v[2] = 255;
				}
				else
				{
					v[0] = 0;
					v[1] = 0;
					v[2] = 0;
				}
			}
		}
		cvtColor(ccp2_simg, ccp2_simg, CV_BGR2GRAY);
		Dilation(ccp2_simg, 2, 2);
		cvtColor(ccp2_simg, ccp2_simg, CV_GRAY2BGR);
		for(int i = 0; i < ccp2_simg.rows - 1; i++)
		{
			for(int j = 0; j < ccp2_simg.cols - 1; j++)
			{
				cv::Vec3b& v = ccp2_simg.at<cv::Vec3b>(i, j);
				if(v[0] > 128)
				{
					cv::Vec3b& dst = isoimg.at<cv::Vec3b>(i , j);
					dst[0] = 2;
					dst[1] = 2;
					dst[2] = 2;
				}
			}
		}
		g_cvshowEX.AddShow("isoimg", isoimg);
		Lines diffusionConstrant = S6GetPatchs(isoimg, 0, 0);
		//diffusionConstrant = SmoothingLen5(diffusionConstrant, 0, 1);
		Color2Side color2s2;
		color2s2 = GetLinesColor2Side(m_vavImage, diffusionConstrant, 0.5);
		Vector3s2d colors;
		colors = GetLinesColor(m_vavImage, diffusionConstrant);
		//      123
		colors = FixLineColors(colors, 400, 10);
		colors = MedianLen(colors, 20, 3);
		colors = SmoothingLen5(colors, 0, 2);
		d3dApp.AddDiffusionLines(diffusionConstrant, colors);
	}
	GetVavView()->OnDraw(0);
	g_cvshowEX.Render();
}