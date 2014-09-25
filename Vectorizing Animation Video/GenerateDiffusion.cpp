#include "LineDef.h"
#include "CmCurveEx.h"
#include "GenerateDiffusion.h"
#include "CvExtenstion2.h"
#include "CvExtenstion0.h"
#include "IFExtenstion.h"
#include "Line.h"

#include <fstream>
#include <iostream>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

Lines FrameInfo::GetLineWidth()
{
	return ConvertFromConstraintLW(curves1, lineWidth);
}

Vector3s2d FrameInfo::GetLineColor()
{
	return ConvertFromConstraintC(curves1, color1);
}

Color2Side FrameInfo::GetBoundaryColor()
{
	return ConvertFromConstraintC2(curves2, color2);
}

FrameInfo ComputeFrame(cv::Mat img)
{
	FrameInfo fi;
	Lines decorativeLine;
	vavImage expImg = WhiteBalance(img.clone());
	expImg = ImgSharpen(expImg);
	const int MASK1_SIZE = 5;
	const int MASK2_SIZE = 5;
	const float secDer = 0.001f;
	cv::Mat ccp1 = img.clone(), ccp2 = expImg.Clone();
	cv::Mat slicimg;
	CmCurveEx* ccp1_curve = NULL;
	CmCurveEx* ccp2_curve = NULL;
	cv::Mat Bblack_Fred1;
	Lines blackLine, blackLine2;
	Lines bLineWidth;
	//if(m_CONSTRAINT_CURVES_PARAMETER_1)
	{
		cvtColor(ccp1, ccp1, CV_BGR2GRAY);
		ccp1.convertTo(ccp1, CV_32F, 1.0 / 255);
		ccp1_curve = new CmCurveEx(ccp1);
		ccp1_curve->CalSecDer(MASK1_SIZE, secDer);
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
	}
	cv::imshow("Bblack_Fred1", Bblack_Fred1);
	//if(m_CONSTRAINT_CURVES_PARAMETER_1 && m_DECORATIVE_CURVES)
	{
		CvLines tpnts2d;
		const CEdges& edges = ccp1_curve->GetEdges();
		for(size_t i = 0; i < edges.size(); i++)
		{
			tpnts2d.push_back(edges[i].pnts);
		}
		blackLine = GetLines(tpnts2d, 0.5, 0.5);
		blackLine = SmoothingLen5(blackLine);
		Lines showLines;
		Lines BLineWidth(blackLine.size());
		Lines normals = GetNormalsLen2(blackLine);
		const double blackRadio = 0.5;
		for(int idx1 = 0; idx1 < blackLine.size(); ++idx1)
		{
			const Line& nowLine = blackLine[idx1];
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
		bLineWidth = BLineWidth;
		blackLine = GetLines(tpnts2d, 0.5, 0.5);
		blackLine = SmoothingLen5(blackLine, 0.9, 5);
		LineEnds les = GetLineEnds(blackLine);
		LinkLineEnds180(les, 5, 15);
		ConnectLineEnds(les, blackLine, bLineWidth);
		IncreaseDensity(blackLine, bLineWidth);
		les = GetLineEnds(blackLine);
		// 連結角度類似的線
		ConnectNearestLines_Width(les, blackLine, bLineWidth, 5, 25);
		bLineWidth = CleanOrphanedLineWidths(bLineWidth, 5);
		bLineWidth = FixLineWidths(bLineWidth, 50);
		//m_BLineWidth = FixLineWidths(m_BLineWidth, 100);
		ClearLineWidthByPercent(bLineWidth, 0.6);
		bLineWidth = FixLineWidths(bLineWidth, 200);
		bLineWidth = SmoothingHas0Len5(bLineWidth, 0, 5);
		bLineWidth = FixedLineWidth(bLineWidth, 5);
	}
	cv::Mat decorative_Curves_2;
	Vector3s2d lineColors;
	//if(m_CONSTRAINT_CURVES_PARAMETER_1 &&m_DECORATIVE_CURVES &&m_BLACK_LINE_VECTORIZATION)
	{
		lineColors = GetLinesColor(img, blackLine);
		lineColors = SmoothingLen5(lineColors, 0, 10);
		bLineWidth = SmoothingLen5(bLineWidth, 0, 10);
		blackLine = SmoothingLen5(blackLine, 0, 15);
	}
	cv::Mat Bwhith_Fblue2;
	//if(m_CONSTRAINT_CURVES_PARAMETER_2)
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
	}
	cv::imshow("Bwhith_Fblue2", Bwhith_Fblue2);
	//if(m_CONSTRAINT_CURVES_PARAMETER_2 && m_BOUNDARY_CURVES)
	{
		CvLines tpnts2d;
		const CEdges& edges2 = ccp2_curve->GetEdges();
		for(size_t i = 0; i < edges2.size(); i++)
		{
			tpnts2d.push_back(edges2[i].pnts);
		}
		blackLine2 = GetLines(tpnts2d, 0.5, 0.5);
		Lines tmp_width = blackLine2;
		blackLine2 = SmoothingLen5(blackLine2, 0.5, 5);
		cv::Mat colorline0 = MakeColorLineImage(img, blackLine2);
		LineEnds les = GetLineEnds(blackLine2);
		LinkLineEnds180(les, 5, 60);
		ConnectLineEnds3_Width(les, blackLine2, tmp_width);
		les = GetLineEnds(blackLine2);
		LinkLineEnds180(les, 8, 60);
		ConnectLineEnds3_Width(les, blackLine2, tmp_width);
		IncreaseDensity(blackLine2, tmp_width);
		les = GetLineEnds(blackLine2);
		ConnectNearestLinesColorCheck(les, blackLine2, 12, 20, img);
		blackLine2 = LineSplitAtEndIntersection(blackLine2, 1);
		tmp_width = blackLine2;
		les = GetLineEnds(blackLine2);
		LinkLineEnds(les, 3, 360);
		ConnectLineEnds5(les, blackLine2);
		les = GetLineEnds(blackLine2);
		LinkLineEnds(les, 3, 360);
		ConnectLineEnds5(les, blackLine2);
		blackLine2 = SmoothingLen5(blackLine2, 0.5, 8);
	}
	Lines isosurfaceLines;
	cv::Mat isoimg, reginimg, isolineimg;
	ColorConstraints ccms;
	ColorConstraints ccms2;
	cv::Mat cmmsIndexImg;
	Color2SideConstraint finalcolor;
	Lines colorline;
	ColorConstraints finallinecolor;
	LineWidthConstraints finallinewidth;
	//if(m_BOUNDARY_CURVES &&   m_REGION_GROWING)
	{
		cv::Mat size2 = img.clone();
		cv::resize(size2.clone(), size2, size2.size() * 2, 0, 0, cv::INTER_CUBIC);
		cv::Mat simg = size2.clone();
		simg = cv::Scalar(0);
		for(int i = 0; i < blackLine2.size(); ++i)
		{
			Line& nowl = blackLine2[i];
			for(int j = 0; j < nowl.size() - 1; ++j)
			{
				cv::Point p1(nowl[j].x * 2, nowl[j].y * 2);
				cv::Point p2(nowl[j + 1].x * 2, nowl[j + 1].y * 2);
				cv::line(simg, p1, p2, cv::Scalar(255, 255, 255));
			}
		}
		cmmsIndexImg = simg.clone();
		cmmsIndexImg = TrapBallMaskAll(cmmsIndexImg, size2);
		cmmsIndexImg = FixSpaceMask(cmmsIndexImg);
		cmmsIndexImg = MixTrapBallMask(cmmsIndexImg, size2, 30, 40);
		cv::imshow("cmmsIndexImg", cmmsIndexImg);
		cv::Mat vecout = img.clone();
		vecout = cv::Scalar(0);
		// show patch img
		cv::Mat colorline1 = MakeColorLineImage(img, blackLine2);
		colorline = LineSplitAtIntersection(blackLine2, 1);
		cv::Mat colorline2 = MakeColorLineImage(img, colorline);
		S6ReColor(cmmsIndexImg, img.clone(), ccms);
		cv::Mat colormask = cmmsIndexImg.clone();
		FloodFillReColor(colormask);
		cv::imshow("colormask", colormask);
		//      cv::waitKey(10);
		Index2Side i2s = GetLinesIndex2SideSmart(cmmsIndexImg, colorline, ccms);
		i2s.left = FixIndexs(i2s.left, 100);
		i2s.right = FixIndexs(i2s.right, 100);
		//      i2s.left = Maxmums2(i2s.left, colorline);
		//      i2s.right = Maxmums2(i2s.right, colorline);
		i2s.left = MedianLenN(i2s.left, 10);
		i2s.right = MedianLenN(i2s.right, 10);
		Color2Side color2s2 = LinesIndex2Color(colorline, i2s, ccms);
		// build model
		// convert Decorative Curves
		finallinecolor = ConvertToConstraintC(blackLine, lineColors);
		finallinewidth = ConvertToConstraintLW(blackLine, bLineWidth);
		finalcolor = ConvertToConstraintC2(colorline, color2s2);
		fi.curves1 = blackLine;
		fi.color1 = finallinecolor;
		fi.lineWidth = finallinewidth;
		fi.curves2 = colorline;
		fi.color2 = finalcolor;
	}
	return fi;
}

void SaveFrameInfos(std::string path, FrameInfos& frms)
{
	std::ofstream ofs(path.c_str(), std::ios::binary);
	if(ofs.is_open())
	{
		boost::archive::binary_oarchive oa(ofs);
		oa << frms;
	}
	ofs.close();
}

FrameInfos LoadFrameInfos(std::string path)
{
	FrameInfos frms;
	std::ifstream ofs(path.c_str(), std::ios::binary);
	if(ofs.is_open())
	{
		boost::archive::binary_iarchive oa(ofs);
		oa >> frms;
	}
	ofs.close();
	return frms;
}

void SetDrawFrame(D3DApp& d3dApp, FrameInfo& fi)
{
	d3dApp.AddDiffusionLines(fi.curves2, fi.GetBoundaryColor());
	d3dApp.AddLinesWidth(fi.curves1, fi.GetLineWidth(), fi.GetLineColor());
}

void SaveBGInfos(std::string path, BackGround& frms)
{
	std::ofstream ofs(path.c_str(), std::ios::binary);
	if(ofs.is_open())
	{
		boost::archive::binary_oarchive oa(ofs);
		oa << frms;
	}
	ofs.close();
}

FrameInfos LoadBGInfos(std::string path)
{
	FrameInfos frms;
	std::ifstream ofs(path.c_str(), std::ios::binary);
	if(ofs.is_open())
	{
		boost::archive::binary_iarchive oa(ofs);
		oa >> frms;
	}
	ofs.close();
	return frms;
}
