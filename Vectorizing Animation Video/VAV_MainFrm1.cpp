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

void VAV_MainFrame::OnButton_BuildVectorization()
{
	D3DApp& d3dApp = GetVavView()->GetD3DApp();
	Lines decorativeLine;
	Lines decorativeLineWidth;
	vavImage expImg = m_vavImage.Clone();
	expImg.ToExpImage();
	cv::imshow("Origin Image", m_vavImage.GetCvMat());
	cv::imshow("Exponential Image", expImg.GetCvMat());
	cv::Mat ccp1 = expImg.Clone(), ccp2 = expImg.Clone();
	CmCurveEx* ccp1_curve = NULL;
	CmCurveEx* ccp2_curve = NULL;
	if (m_CONSTRAINT_CURVES_PARAMETER_1)
	{
		cvtColor(ccp1, ccp1, CV_BGR2GRAY);
		ccp1.convertTo(ccp1, CV_32F, 1.0 / 255);
		ccp1_curve = new CmCurveEx(ccp1);
		ccp1_curve->CalSecDer(3, 0.0001f);
		ccp1_curve->Link();
		cv::Mat show_ccp1 = expImg.Clone();
		const std::vector<CEdge>& edges = ccp1_curve->GetEdges();
		for (size_t i = 0; i < edges.size(); i++)
		{
			cv::Vec3b color(255, 0, 0);
			const std::vector<cv::Point>& pnts = edges[i].pnts;
			for (size_t j = 0; j < pnts.size(); j++)
			{
				show_ccp1.at<cv::Vec3b>(pnts[j]) = color;
			}
		}
		imshow("Constraint Curves Parameter 1", show_ccp1);
	}
	if (m_CONSTRAINT_CURVES_PARAMETER_1 &&
			m_DECORATIVE_CURVES &&
			ccp1_curve)
	{
		CvLines tpnts2d;
		const CEdges& edges = ccp1_curve->GetEdges();
		for (size_t i = 0; i < edges.size(); i++)
		{
			tpnts2d.push_back(edges[i].pnts);
		}
		m_BlackLine = GetLines(tpnts2d, 0.5, 0.5);
		m_BlackLine = SmoothingLen5(m_BlackLine);
		Lines showLines;
		Lines BLineWidth(m_BlackLine.size());
		Lines normals = GetNormalsLen2(m_BlackLine);
		const double blackRadio = 0.5;
		for (int idx1 = 0; idx1 < m_BlackLine.size(); ++idx1)
		{
			const Line& nowLine = m_BlackLine[idx1];
			const Line& nowNormals = normals[idx1];
			Line& lineWidths = BLineWidth[idx1];
			lineWidths.clear();
			for (int idx2 = 0; idx2 < nowLine.size() - 1; ++idx2)
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
				if (width1.size() >= 2 && width2.size() >= 2 && abs(width2[0] - width2[1]) < 1)
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
		LinkLineEnds(les, 5, 20);
		ConnectSimilarLines(les, m_BlackLine, m_BLineWidth);
		IncreaseDensity(m_BlackLine, m_BLineWidth);
		les = GetLineEnds(m_BlackLine);
		ConnectNearestLines(les, m_BlackLine, m_BLineWidth, 20, 15, 15);
		m_BLineWidth = CleanOrphanedLineWidths(m_BLineWidth, 5);
		m_BLineWidth = FixLineWidths(m_BLineWidth, 50);
		decorativeLine = m_BlackLine;
		decorativeLineWidth = m_BLineWidth;
		Vector3s2d lineColors;
		lineColors = GetLinesColor(m_vavImage, m_BlackLine);
		lineColors = SmoothingLen5(lineColors, 0, 10);
		m_BlackLine = SmoothingLen5(m_BlackLine, 0, 10);
		d3dApp.AddLinesWidth(m_BlackLine, m_BLineWidth, lineColors);
		d3dApp.AddDiffusionLines(m_BlackLine, lineColors);
		vavImage tmp = m_vavImage.Clone();
		tmp.GetCvMat() = cv::Scalar(255, 255, 255);
		GetVavView()->SetTexture(tmp.GetDx11Texture());
		d3dApp.SetScaleTemporary(1);
		d3dApp.BuildPoint();
		d3dApp.InterSetRenderTransparencyOutput1();
		cv::Mat simg = d3dApp.DrawSceneToCvMat();
		imshow("decorative Curves", simg);
		d3dApp.SetScaleRecovery();
		d3dApp.ClearSkeletonLines();
		GetVavView()->SetTexture(m_vavImage.GetDx11Texture());
	}
	if (m_CONSTRAINT_CURVES_PARAMETER_2)
	{
		cvtColor(ccp2, ccp2, CV_BGR2GRAY);
		ccp2.convertTo(ccp2, CV_32F, 1.0 / 255);
		ccp2_curve = new CmCurveEx(ccp2);
		ccp2_curve->CalSecDer(5, 0.0001f);
		ccp2_curve->Link();
		cv::Mat show_ccp2 = expImg.Clone();
		const std::vector<CEdge>& edges = ccp2_curve->GetEdges();
		for (size_t i = 0; i < edges.size(); i++)
		{
			cv::Vec3b color(255, 0, 0);
			const std::vector<cv::Point>& pnts = edges[i].pnts;
			for (size_t j = 0; j < pnts.size(); j++)
			{
				show_ccp2.at<cv::Vec3b>(pnts[j]) = color;
			}
		}
		imshow("Constraint Curves Parameter 2", show_ccp2);
	}
	if (m_CONSTRAINT_CURVES_PARAMETER_2 &&
			m_BOUNDARY_CURVES)
	{
		ccp2_curve->CalSecDer2(5, 0.001f);
		ccp2_curve->Link();
		CvLines tpnts2d;
		const CEdges& edges2 = ccp2_curve->GetEdges();
		for (size_t i = 0; i < edges2.size(); i++)
		{
			tpnts2d.push_back(edges2[i].pnts);
		}
		m_BlackLine2 = GetLines(tpnts2d, 0.5, 0.5);
		Lines tmp_width = m_BlackLine2;
		//m_BlackLine2 = SmoothingLen5(m_BlackLine2, 0.5, 5);
		m_BlackLine2 = SmoothingLen5(m_BlackLine2, 0, 15);
		LineEnds les = GetLineEnds(m_BlackLine2);
		LinkLineEnds(les, 5, 20);
		ConnectSimilarLines(les, m_BlackLine2, tmp_width);
		IncreaseDensity(m_BlackLine2, tmp_width);
		les = GetLineEnds(m_BlackLine2);
		ConnectNearestLines(les, m_BlackLine2, tmp_width, 10, 5, 20);
		d3dApp.AddLines(m_BlackLine2);
		d3dApp.SetScaleTemporary(1);
		d3dApp.BuildPoint();
		d3dApp.InterSetRenderTransparencyOutput2();
		cv::Mat simg = d3dApp.DrawSceneToCvMat(), stmp;
		d3dApp.SetScaleRecovery();
		d3dApp.ClearSkeletonLines();
		stmp = simg.clone();
		cvtColor(simg, simg, CV_BGR2GRAY);
		cv::Mat curveExtration;
		curveExtration = simg.clone();
		Dilation(curveExtration, 2, 1);
		cvtColor(curveExtration, curveExtration, CV_GRAY2BGR);
		cv::Mat tmpimg = m_vavImage.Clone();
		cv::Mat sampleimg = m_vavImage.Clone();
		cv::Mat   isoimg = MakeIsoSurfaceImg(tmpimg, 32);
		for (int i = 0; i < curveExtration.rows; i++)
		{
			for (int j = 0; j < curveExtration.cols ; j++)
			{
				cv::Vec3b& v = curveExtration.at<cv::Vec3b>(i, j);
				if (v[0] > 0)
				{
					cv::Vec3b& sam = sampleimg.at<cv::Vec3b>(i, j);
					sam[0] = 0;
					sam[1] = 0;
					sam[2] = 0;
				}
			}
		}
		cvtColor(curveExtration, curveExtration, CV_BGR2GRAY);
		Dilation(curveExtration, 2, 2);
		cvtColor(curveExtration, curveExtration, CV_GRAY2BGR);
		for (int i = 0; i < curveExtration.rows - 1; i++)
		{
			for (int j = 0; j < curveExtration.cols - 1; j++)
			{
				cv::Vec3b& v = curveExtration.at<cv::Vec3b>(i, j);
				if (v[0] > 0)
				{
					cv::Vec3b& dst = isoimg.at<cv::Vec3b>(i , j);
					dst[0] = 1;
					dst[1] = 1;
					dst[2] = 1;
				}
			}
		}
		Lines ColorWidth;
		Color2Side color2s;
		color2s = GetLinesColor2SideSmart3(m_vavImage, sampleimg, m_BlackLine2, 30);
		//color2s = GetLinesColor2Side(m_vavImage, m_BlackLine2, 3);
		color2s.left = FixLineColors(color2s.left, 600, 10);
		color2s.right = FixLineColors(color2s.right, 600, 10);
		d3dApp.AddDiffusionLines(m_BlackLine2, color2s);
		d3dApp.SetScaleTemporary(1);
		d3dApp.BuildPoint();
		d3dApp.InterSetRenderTransparencyDefault();
		simg = d3dApp.DrawSceneToCvMat();
		cv::imshow("Boundary Curves", simg);
		d3dApp.SetScaleRecovery();
		d3dApp.ClearSkeletonLines();
		d3dApp.InterSetRenderTransparencyDefault();
	}
	if (m_CONSTRAINT_CURVES_PARAMETER_2 &&
			m_BOUNDARY_CURVES &&
			m_BOUNDARY_CURVES_CONSTRAINT)
	{

	}
	if (m_CONSTRAINT_CURVES_PARAMETER_1 &&
			m_DECORATIVE_CURVES &&
			m_BLACK_LINE_VECTORIZATION)
	{
	}
	if (m_ISOSURFACE_REGION)
	{
	}
	if (m_ISOSURFACE_CONSTRAINT)
	{
	}
	if (m_ISOSURFACE_CONSTRAINT &&
			m_ISOSURFACE_CONSTRAINT_VECTORIZATION)
	{
	}
	GetVavView()->OnDraw(0);
}
