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
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


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

FrameInfo ComputeFrame1FG(cv::Mat img, cv::Mat fg, ColorRegion* cr)
{
	FrameInfo fi;
	Lines decorativeLine;
	vavImage expImg = WhiteBalance(img.clone());
	//expImg = ImgSharpen(expImg);
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
		//IncreaseDensity(blackLine, bLineWidth);
		for(int i = 0; i < blackLine.size(); ++i)
		{
			int count = 0;
			for(int j = 0; j < blackLine[i].size(); ++j)
			{
				Vector2 p = blackLine[i][j];
				if(img.at<cv::Vec3b>(p.y, p.x) == cv::Vec3b(0, 0, 0))
				{
					++count;
				}
			}
			if(count > blackLine[i].size() / 2)
			{
				blackLine.erase(blackLine.begin() + i);
				bLineWidth.erase(bLineWidth.begin() + i);
				--i;
			}
		}
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
		lineColors = GetLinesColor(img.clone(), blackLine);
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
	}
	Lines isosurfaceLines;
	cv::Mat isoimg, reginimg, isolineimg;
	ColorConstraints ccms2;
	cv::Mat cmmsIndexImg;
	Color2SideConstraint finalcolor;
	Lines colorline;
	ColorConstraints finallinecolor;
	LineWidthConstraints finallinewidth;
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
		LineEnds les;
		les = GetLineEnds(blackLine2);
		LinkLineEnds(les, 3, 360);
		ConnectLineEnds5(les, blackLine2);
		les = GetLineEnds(blackLine2);
		LinkLineEnds180(les, 5, 60);
		ConnectLineEnds3_Width(les, blackLine2, tmp_width);
		les = GetLineEnds(blackLine2);
		LinkLineEnds180(les, 6, 60);
		ConnectLineEnds3_Width(les, blackLine2, tmp_width);
		//IncreaseDensity(blackLine2, tmp_width);
		les = GetLineEnds(blackLine2);
		ConnectNearestLines(les, blackLine2, 6, 70);
		blackLine2 = LineSplitAtEndIntersection(blackLine2, 1);
		tmp_width = blackLine2;
		les = GetLineEnds(blackLine2);
		LinkLineEnds(les, 3, 360);
		ConnectLineEnds5(les, blackLine2);
		blackLine2 = SmoothingLen5(blackLine2, 0.5, 8);
	}
	//if(m_BOUNDARY_CURVES &&   m_REGION_GROWING)
	{
		cv::Mat size2 = img.clone();
		cv::resize(size2.clone(), size2, size2.size() * 2, 0, 0, cv::INTER_CUBIC);
		// show patch img
		cv::Mat colorline1 = MakeColorLineImage(img, blackLine2);
		colorline = LineSplitAtIntersection(blackLine2, 1);
		
		ColorConstraints ccms;
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
		cmmsIndexImg = TrapBallMaskAllFG(cmmsIndexImg, size2, fg);
		cmmsIndexImg = FixSpaceMask(cmmsIndexImg);
		cmmsIndexImg = MixTrapBallMask(cmmsIndexImg, size2, 30, 40);
		S6ReColor(cmmsIndexImg, img.clone(), ccms);
// 		{
// 			static int sid = -1;
// 			sid++;
// 			char path[100];
// 			simg = cmmsIndexImg.clone();
// 			FloodFillReColor(simg);
// 			sprintf(path, "cmms2_%d.png", sid);
// 			cv::imwrite(path, simg);
// 		}
//		cv::Mat colorline2 = MakeColorLineImage(img, colorline);
//      cv::Mat show = cmmsIndexImg.clone();
//      for(int i = 0; i < show.rows; ++i)
//      {
//          for(int j = 0; j < show.cols; ++j)
//          {
//              cv::Vec3b& c = cmmsIndexImg.at<cv::Vec3b>(i, j);
//              int idx = c[0];
//              if(idx <= ccms.size())
//              {
//                  show.at<cv::Vec3b>(i, j) = ccms[idx - 1].GetColorCvPoint(j / 2, i / 2);
//              }
//          }
//      }
//      cv::imshow("show", show);
//      cv::waitKey(10);
		Index2Side i2s = GetLinesIndex2SideSmart2(colorline, cmmsIndexImg, cr->markimg);
//      i2s.left = FixIndexs(i2s.left, 100);
//      i2s.right = FixIndexs(i2s.right, 100);
//      i2s.left = Maxmums2(i2s.left, colorline);
//      i2s.right = Maxmums2(i2s.right, colorline);
		i2s.left = MedianLenN(i2s.left, 10);
		i2s.right = MedianLenN(i2s.right, 10);
		Color2Side color2s2 = LinesIndex2Color2(colorline, i2s, ccms, cr->ccms);
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

FrameInfo ComputeFrame1(cv::Mat img, ColorRegion* cr)
{
	FrameInfo fi;
	Lines decorativeLine;
	vavImage expImg = WhiteBalance2(img.clone());
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
		//IncreaseDensity(blackLine, bLineWidth);
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
		//IncreaseDensity(blackLine2, tmp_width);
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
		cv::Mat vecout = img.clone();
		vecout = cv::Scalar(0);
		// show patch img
		cv::Mat colorline1 = MakeColorLineImage(img, blackLine2);
		colorline = LineSplitAtIntersection(blackLine2, 1);
		cv::Mat colorline2 = MakeColorLineImage(img, colorline);
		S6ReColor(cmmsIndexImg, img.clone(), ccms);
		if(cr != NULL)
		{
			cr->ccms = ccms;
			cr->markimg = cmmsIndexImg.clone();
		}
		cv::Mat colormask = cmmsIndexImg.clone();
		FloodFillReColor(colormask);
		//      cv::waitKey(10);
		Index2Side i2s = GetLinesIndex2SideSmart(colorline, cmmsIndexImg);
		i2s.left = FixIndexs(i2s.left, 100);
		i2s.right = FixIndexs(i2s.right, 100);
		//      i2s.left = Maxmums2(i2s.left, colorline);
		//      i2s.right = Maxmums2(i2s.right, colorline);
//      i2s.left = MedianLenN(i2s.left, 10);
//      i2s.right = MedianLenN(i2s.right, 10);
		Color2Side color2s2 = LinesIndex2Color(colorline, i2s, ccms);
		// build model
		// convert Decorative Curves
		MakePureDecorativeLine(blackLine, bLineWidth, lineColors);
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


FrameInfo ComputeFrame2FG(cv::Mat img, cv::Mat fg, ColorRegion* cr)
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
		LineEnds les = GetLineEnds(blackLine);
		LinkLineEnds180(les, 5, 60);
		ConnectLineEnds3(les, blackLine);
		les = GetLineEnds(blackLine);
		LinkLineEnds180(les, 8, 60);
		ConnectLineEnds3(les, blackLine);
		les = GetLineEnds(blackLine);
		ConnectNearestLinesColorCheck(les, blackLine, 12, 20, img.clone());
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
		for(int i = 0; i < blackLine.size(); ++i)
		{
			int count = 0;
			for(int j = 0; j < blackLine[i].size(); ++j)
			{
				Vector2 p = blackLine[i][j];
				if(img.at<cv::Vec3b>(p.y, p.x) == cv::Vec3b(0, 0, 0))
				{
					++count;
				}
			}
			if(count > blackLine[i].size() / 2)
			{
				blackLine.erase(blackLine.begin() + i);
				bLineWidth.erase(bLineWidth.begin() + i);
				--i;
			}
		}
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
		lineColors = GetLinesColor(img.clone(), blackLine);
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
	}
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
		LineEnds les = GetLineEnds(blackLine2);
		LinkLineEnds180(les, 5, 60);
		ConnectLineEnds3_Width(les, blackLine2, tmp_width);
		les = GetLineEnds(blackLine2);
		LinkLineEnds180(les, 8, 60);
		ConnectLineEnds3_Width(les, blackLine2, tmp_width);
		//IncreaseDensity(blackLine2, tmp_width);
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
		// show patch img
		cv::Mat colorline1 = MakeColorLineImage(img, blackLine2);
		colorline = LineSplitAtIntersection(blackLine2, 1);
		ColorConstraints ccms;
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
		cmmsIndexImg = TrapBallMaskAllFG(cmmsIndexImg, size2, fg);
		cmmsIndexImg = FixSpaceMask(cmmsIndexImg);
		cmmsIndexImg = MixTrapBallMask(cmmsIndexImg, size2, 30, 40);
		S6ReColor(cmmsIndexImg, img.clone(), ccms);
		cv::Mat colorline2 = MakeColorLineImage(img, colorline);
		Index2Side i2s = GetLinesIndex2SideSmart2(colorline, cmmsIndexImg, cr->markimg);
		//      i2s.left = FixIndexs(i2s.left, 100);
		//      i2s.right = FixIndexs(i2s.right, 100);
		//      i2s.left = Maxmums2(i2s.left, colorline);
		//      i2s.right = Maxmums2(i2s.right, colorline);
		//      i2s.left = MedianLenN(i2s.left, 10);
		//      i2s.right = MedianLenN(i2s.right, 10);
		Color2Side color2s2 = LinesIndex2Color2(colorline, i2s, ccms, cr->ccms);
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


FrameInfo ComputeFrame2(cv::Mat img, ColorRegion* cr)
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
		LineEnds les = GetLineEnds(blackLine);
		LinkLineEnds180(les, 5, 60);
		ConnectLineEnds3(les, blackLine);
		les = GetLineEnds(blackLine);
		LinkLineEnds180(les, 8, 60);
		ConnectLineEnds3(les, blackLine);
		les = GetLineEnds(blackLine);
		ConnectNearestLinesColorCheck(les, blackLine, 12, 20, img.clone());
		Lines showLines;
		Lines BLineWidth(blackLine.size());
		Lines normals = GetNormalsLen2(blackLine);
		const double blackRadio = 0.6;
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
		//m_BLineWidth = FixLineWidths(m_BLineWidth, 100);
		ClearLineWidthByPercent(bLineWidth, 0.3);
		bLineWidth = FixLineWidths(bLineWidth, 20);
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
		//IncreaseDensity(blackLine2, tmp_width);
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
		blackLine2 = SmoothingLen5(blackLine2, 0, 3);
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
		//cmmsIndexImg = MixTrapBallMask(cmmsIndexImg, size2, 25, 25);
		cv::Mat vecout = img.clone();
		vecout = cv::Scalar(0);
		// show patch img
		cv::Mat colorline1 = MakeColorLineImage(img, blackLine2);
		colorline = LineSplitAtIntersection(blackLine2, 1);
		cv::Mat colorline2 = MakeColorLineImage(img, colorline);
		S6ReColor(cmmsIndexImg, img.clone(), ccms);
		if(cr != NULL)
		{
			cr->ccms = ccms;
			cr->markimg = cmmsIndexImg.clone();
		}
		cv::Mat colormask = cmmsIndexImg.clone();
		FloodFillReColor(colormask);
		//      cv::waitKey(10);
		Index2Side i2s = GetLinesIndex2SideSmart(colorline, cmmsIndexImg);
		i2s.left = FixIndexs(i2s.left, 100);
		i2s.right = FixIndexs(i2s.right, 100);
		//      i2s.left = Maxmums2(i2s.left, colorline);
		//      i2s.right = Maxmums2(i2s.right, colorline);
//      i2s.left = MedianLenN(i2s.left, 10);
//      i2s.right = MedianLenN(i2s.right, 10);
		Color2Side color2s2 = LinesIndex2Color(colorline, i2s, ccms);
		// build model
		// convert Decorative Curves
		MakePureDecorativeLine(blackLine, bLineWidth, lineColors);
		finallinecolor = ConvertToConstraintC(blackLine, lineColors);
		finallinewidth = ConvertToConstraintLW(blackLine, bLineWidth);
		finalcolor = ConvertToConstraintC2(colorline, color2s2);
		fi.curves1 = blackLine;
		fi.color1 = finallinecolor;
		fi.lineWidth = finallinewidth;
		fi.tmplinewidth = bLineWidth;
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
	d3dApp.ClearTriangles();
	d3dApp.ClearSkeletonLines();
	d3dApp.AddDiffusionLines(fi.curves2, fi.GetBoundaryColor());
	d3dApp.AddLinesWidth(fi.curves1, fi.GetLineWidth(), fi.GetLineColor());
	d3dApp.BuildPoint();
	d3dApp.DrawScene();
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

BackGround LoadBGInfos(std::string path)
{
	BackGround frms;
	std::ifstream ofs(path.c_str(), std::ios::binary);
	if(ofs.is_open())
	{
		boost::archive::binary_iarchive oa(ofs);
		oa >> frms;
	}
	ofs.close();
	return frms;
}

void RemoveBGs_FG_Part(FrameInfo& fi, cv::Mat fg)
{
	Lines& lines1 = fi.curves1;
	Lines widths1 = fi.GetLineWidth();
	Vector3s2d colors1 = fi.GetLineColor();
	Lines reslines;
	Lines reswidths;
	Vector3s2d rescolors;
	for(int a = 0; a < lines1.size(); ++a)
	{
		Line nline;
		Line nwidth;
		Vector3s ncolor;
		Line& line = lines1[a];
		for(int b = 0; b < line.size(); ++b)
		{
			uchar v = fg.at<uchar>(line[b].y, line[b].x);
			if(v == 0)
			{
				nline.push_back(line[b]);
				nwidth.push_back(widths1[a][b]);
				ncolor.push_back(colors1[a][b]);
			}
			else
			{
				if(nline.size() > 4)
				{
					reslines.push_back(nline);
					reswidths.push_back(nwidth);
					rescolors.push_back(ncolor);
				}
				nline.clear();
				nwidth.clear();
				ncolor.clear();
			}
		}
		reslines.push_back(nline);
		reswidths.push_back(nwidth);
		rescolors.push_back(ncolor);
	}
	fi.curves1 = reslines;
	fi.lineWidth = ConvertToConstraintLW(reslines, reswidths);
	fi.color1 = ConvertToConstraintC(reslines, rescolors);
	Lines reslines2;
	Lines& lines2 = fi.curves2;
	Color2Side color2 = fi.GetBoundaryColor();
	Color2Side rescolors2;
	for(int a = 0; a < lines2.size(); ++a)
	{
		Line nline;
		Vector3s lcolor;
		Vector3s rcolor;
		Line& line = lines2[a];
		for(int b = 0; b < line.size(); ++b)
		{
			uchar v = fg.at<uchar>(line[b].y, line[b].x);
			if(v == 0)
			{
				nline.push_back(line[b]);
				lcolor.push_back(color2.left[a][b]);
				rcolor.push_back(color2.right[a][b]);
			}
			else
			{
				if(nline.size() > 4)
				{
					reslines2.push_back(nline);
					rescolors2.left.push_back(lcolor);
					rescolors2.right.push_back(rcolor);
				}
				nline.clear();
				lcolor.clear();
				rcolor.clear();
			}
		}
		reslines2.push_back(nline);
		rescolors2.left.push_back(lcolor);
		rescolors2.right.push_back(rcolor);
	}
	fi.curves2 = reslines2;
	fi.color2 = ConvertToConstraintC2(reslines2, rescolors2);
}

void RemoveFGs_BG_Part(FrameInfo& fi, cv::Mat fg)
{
	Lines& lines1 = fi.curves1;
	Lines widths1 = fi.GetLineWidth();
	Vector3s2d colors1 = fi.GetLineColor();
	Lines reslines;
	Lines reswidths;
	Vector3s2d rescolors;
	for(int a = 0; a < lines1.size(); ++a)
	{
		Line nline;
		Line nwidth;
		Vector3s ncolor;
		Line& line = lines1[a];
		for(int b = 0; b < line.size(); ++b)
		{
			uchar v = fg.at<uchar>(line[b].y, line[b].x);
			if(v != 0)
			{
				nline.push_back(line[b]);
				nwidth.push_back(widths1[a][b]);
				ncolor.push_back(colors1[a][b]);
			}
			else
			{
				if(nline.size() > 5)
				{
					reslines.push_back(nline);
					reswidths.push_back(nwidth);
					rescolors.push_back(ncolor);
				}
				nline.clear();
				nwidth.clear();
				ncolor.clear();
			}
		}
		if(nline.size() > 5)
		{
			reslines.push_back(nline);
			reswidths.push_back(nwidth);
			rescolors.push_back(ncolor);
		}
	}
	fi.curves1 = reslines;
	fi.lineWidth = ConvertToConstraintLW(reslines, reswidths);
	fi.color1 = ConvertToConstraintC(reslines, rescolors);
	Lines reslines2;
	Lines& lines2 = fi.curves2;
	Color2Side color2 = fi.GetBoundaryColor();
	Color2Side rescolors2;
	for(int a = 0; a < lines2.size(); ++a)
	{
		Line nline;
		Vector3s lcolor;
		Vector3s rcolor;
		Line& line = lines2[a];
		for(int b = 0; b < line.size(); ++b)
		{
			uchar v = fg.at<uchar>(line[b].y, line[b].x);
			if(v != 0)
			{
				nline.push_back(line[b]);
				lcolor.push_back(color2.left[a][b]);
				rcolor.push_back(color2.right[a][b]);
			}
			else
			{
				if(nline.size() > 4)
				{
					reslines2.push_back(nline);
					rescolors2.left.push_back(lcolor);
					rescolors2.right.push_back(rcolor);
				}
				nline.clear();
				lcolor.clear();
				rcolor.clear();
			}
		}
		reslines2.push_back(nline);
		rescolors2.left.push_back(lcolor);
		rescolors2.right.push_back(rcolor);
	}
	fi.curves2 = reslines2;
	fi.color2 = ConvertToConstraintC2(reslines2, rescolors2);
}

FrameInfo ComputeFrame09(cv::Mat img, ColorRegion* cr /*= NULL*/)
{
	cv::Mat edgesImg, src_gray = img.clone(), src_grayf,
					  oriImg = img.clone(), show_ccp;
	CmCurveEx* dcurve = NULL;
	{
		// curve extration
		cvtColor(src_gray, src_gray, CV_BGR2GRAY);
		// find decorative curve
		src_gray.convertTo(src_grayf, CV_32F, 1.0 / 255);
		dcurve = new CmCurveEx(src_grayf);
		dcurve->CalSecDer(5, 0.001f);
		dcurve->Link();
		show_ccp = img.clone();
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
	}
	edgesImg = TrapBallMaskAll(edgesImg, oriImg);
	edgesImg = FixSpaceMask(edgesImg);
	edgesImg = MixTrapBallMask(edgesImg, oriImg, 30, 40);
	{
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
	//cv::imshow("show_trap", show_trap);
	ImageSpline is = S4GetPatchs(edgesImg, 0, 0);
	Lines colorline;
	for(int i = 0; i < is.m_LineFragments.size(); ++i)
	{
		colorline.push_back(is.m_LineFragments[i].m_Points);
	}
	ColorConstraints ccms0;
	cv::Mat size2 = img.clone();
	cv::resize(edgesImg.clone(), edgesImg, edgesImg.size() * 2, 0, 0, cv::INTER_NEAREST);
	S6ReColor(edgesImg, img.clone(), ccms0);
	if(cr)
	{
		cr->ccms = ccms0;
		cr->markimg = edgesImg.clone();
	}
	//DrawColorLineImage(cmmsIndexImg, colorline);
	cv::Mat colormask = edgesImg.clone();
	FloodFillReColor(colormask);
	Index2Side i2s = GetLinesIndex2SideSmart(colorline, edgesImg);
	i2s.left = FixIndexs(i2s.left, 100);
	i2s.right = FixIndexs(i2s.right, 100);
//  i2s.left = MedianLenN(i2s.left, 10);
//  i2s.right = MedianLenN(i2s.right, 10);
	i2s.left = Maxmums2(i2s.left, colorline);
	i2s.right = Maxmums2(i2s.right, colorline);
	Color2Side color2s2 = LinesIndex2Color(colorline, i2s, ccms0);
	//d3dApp.AddDiffusionLines(colorline, color2s2);
	FrameInfo fi;
	fi.curves2 = colorline;
	fi.color2 = ConvertToConstraintC2(colorline, color2s2);
	return fi;
}

FrameInfo ComputeFrameFG09(cv::Mat img, cv::Mat fg, ColorRegion* cr)
{
	cv::Mat edgesImg, src_gray = img.clone(), src_grayf,
					  oriImg = img.clone(), show_ccp;
	CmCurveEx* dcurve = NULL;
	{
		// curve extration
		cvtColor(src_gray, src_gray, CV_BGR2GRAY);
		// find decorative curve
		src_gray.convertTo(src_grayf, CV_32F, 1.0 / 255);
		dcurve = new CmCurveEx(src_grayf);
		dcurve->CalSecDer(5, 0.001f);
		dcurve->Link();
		show_ccp = img.clone();
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
	}
	edgesImg = TrapBallMaskAllFG(edgesImg, oriImg, fg);
	edgesImg = FixSpaceMask(edgesImg);
	edgesImg = MixTrapBallMask(edgesImg, oriImg, 30, 40);
	{
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
	cv::imshow("show_trap", show_trap);
	ImageSpline is = S4GetPatchs(edgesImg, 0, 0);
	Lines colorline;
	for(int i = 0; i < is.m_LineFragments.size(); ++i)
	{
		colorline.push_back(is.m_LineFragments[i].m_Points);
	}
	ColorConstraints ccms0;
	cv::Mat size2 = img.clone();
	cv::resize(edgesImg.clone(), edgesImg, edgesImg.size() * 2, 0, 0, cv::INTER_NEAREST);
	S6ReColor(edgesImg, img.clone(), ccms0);
	//DrawColorLineImage(cmmsIndexImg, colorline);
	cv::Mat colormask = edgesImg.clone();
	FloodFillReColor(colormask);
	Index2Side i2s = GetLinesIndex2SideSmart2(colorline, edgesImg, cr->markimg);
	i2s.left = FixIndexs(i2s.left, 100);
	i2s.right = FixIndexs(i2s.right, 100);
//  i2s.left = MedianLenN(i2s.left, 10);
//  i2s.right = MedianLenN(i2s.right, 10);
	i2s.left = Maxmums2(i2s.left, colorline);
	i2s.right = Maxmums2(i2s.right, colorline);
	Color2Side color2s2 = LinesIndex2Color2(colorline, i2s, ccms0, cr->ccms);
	//d3dApp.AddDiffusionLines(colorline, color2s2);
	FrameInfo fi;
	fi.curves2 = colorline;
	fi.color2 = ConvertToConstraintC2(colorline, color2s2);
	return fi;
}

