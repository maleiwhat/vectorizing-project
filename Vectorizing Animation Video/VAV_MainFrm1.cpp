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
	vavImage expImg = m_vavImage.Clone();
	expImg.ToExpImage();
	expImg.ToExpImage();
	const int MASK1_SIZE = 5;
	const int MASK2_SIZE = 5;
	const float secDer = 0.001f;
	cv::imshow("Origin Image", m_vavImage.GetCvMat());
	cv::imshow("Exponential Image", expImg.GetCvMat());
	cv::Mat ccp1 = expImg.Clone(), ccp2 = expImg.Clone();
	CmCurveEx* ccp1_curve = NULL;
	CmCurveEx* ccp2_curve = NULL;
	cv::Mat Bblack_Fred1;
	if (m_CONSTRAINT_CURVES_PARAMETER_1)
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
		for (size_t i = 0; i < edges.size(); i++)
		{
			const std::vector<cv::Point>& pnts = edges[i].pnts;
			for (size_t j = 0; j < pnts.size(); j++)
			{
				show_ccp1.at<cv::Vec3b>(pnts[j]) = cv::Vec3b(0, 0, 255);
				Bblack_Fred1.at<cv::Vec3b>(pnts[j]) = cv::Vec3b(0, 0, 255);
			}
		}
		imshow("Constraint Curves Parameter 1_0", show_ccp1);
		imshow("Constraint Curves Parameter 1_1", Bblack_Fred1);
	}
	if (m_CONSTRAINT_CURVES_PARAMETER_1 &&
			m_DECORATIVE_CURVES)
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
				if (width1.size() >= 2 && width2.size() >= 2 && abs(width2[0] - width2[1]) < 1.5)
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
		ConnectNearestLines(les, m_BlackLine, m_BLineWidth, 5, 15);
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
		for (int i = 0; i < Bblack_Fred1.rows; i++)
		{
			for (int j = 0; j < Bblack_Fred1.cols ; j++)
			{
				cv::Vec3b& v = Bblack_Fred1.at<cv::Vec3b>(i, j);
				if (v[2] > 0)
				{
					cv::Vec3b& sam = simg.at<cv::Vec3b>(i, j);
					sam[0] = 128;
					sam[1] = 128;
					sam[2] = 0;
				}
			}
		}
		cv::Mat visualize = m_vavImage.Clone();
		for (int i = 0; i < Bblack_Fred1.rows; i++)
		{
			for (int j = 0; j < Bblack_Fred1.cols ; j++)
			{
				cv::Vec3b& v = Bblack_Fred1.at<cv::Vec3b>(i, j);
				if (v[0] > 0 || v[2] > 0)
				{
					cv::Vec3b& sam = visualize.at<cv::Vec3b>(i, j);
					sam = v;
				}
			}
		}
		imshow("decorative Curves_0", simg);
		imshow("decorative Curves_1", visualize);
		d3dApp.SetScaleRecovery();
		d3dApp.ClearSkeletonLines();
		GetVavView()->SetTexture(m_vavImage.GetDx11Texture());
	}
	if (m_CONSTRAINT_CURVES_PARAMETER_1 &&
			m_DECORATIVE_CURVES &&
			m_BLACK_LINE_VECTORIZATION)
	{
		Vector3s2d lineColors;
		lineColors = GetLinesColor(m_vavImage, m_BlackLine);
		lineColors = SmoothingLen5(lineColors, 0, 10);
		m_BLineWidth = SmoothingLen5(m_BLineWidth, 0, 10);
		m_BlackLine = SmoothingLen5(m_BlackLine, 0, 15);
     for (int i = 0; i < m_BlackLine.size(); ++i)
     {
         for (int j = 0; j < m_BlackLine[i].size(); ++j)
         {
             m_BlackLine[i][j] *= 2;
             m_BLineWidth[i][j] *= 2;
         }
     }
		d3dApp.AddLinesWidth(m_BlackLine, m_BLineWidth, lineColors);
		d3dApp.SetScaleTemporary(1);
		d3dApp.BuildPoint();
		d3dApp.InterSetRenderTransparencyOutput3();
		cv::Mat simg = d3dApp.DrawSceneToCvMat(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		d3dApp.SetScaleRecovery();
		imshow("decorative Curves_2", simg);
	}
	cv::Mat Bwhith_Fblue2;
	if (m_CONSTRAINT_CURVES_PARAMETER_2)
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
		for (size_t i = 0; i < edges.size(); i++)
		{
			const std::vector<cv::Point>& pnts = edges[i].pnts;
			for (size_t j = 0; j < pnts.size(); j++)
			{
				show_ccp2.at<cv::Vec3b>(pnts[j]) = cv::Vec3b(0, 0, 255);
				Bwhith_Fblue2.at<cv::Vec3b>(pnts[j]) = cv::Vec3b(0, 0, 255);
			}
		}
		imshow("Constraint Curves Parameter 2_0", show_ccp2);
		imshow("Constraint Curves Parameter 2_1", Bwhith_Fblue2);
	}
	cv::Mat ccp2_simg;
	if (m_CONSTRAINT_CURVES_PARAMETER_2 &&
			m_BOUNDARY_CURVES)
	{
		CvLines tpnts2d;
		const CEdges& edges2 = ccp2_curve->GetEdges();
		for (size_t i = 0; i < edges2.size(); i++)
		{
			tpnts2d.push_back(edges2[i].pnts);
		}
		m_BlackLine2 = GetLines(tpnts2d, 0.5, 0.5);
		Lines tmp_width = m_BlackLine2;
		m_BlackLine2 = SmoothingLen5(m_BlackLine2, 0.5, 5);
		//m_BlackLine2 = SmoothingLen5(m_BlackLine2, 0, 5);
		LineEnds les = GetLineEnds(m_BlackLine2);
		LinkLineEnds180(les, 5, 60);
		ConnectLineEnds3(les, m_BlackLine2, tmp_width);
		les = GetLineEnds(m_BlackLine2);
		LinkLineEnds180(les, 5, 60);
		ConnectLineEnds3(les, m_BlackLine2, tmp_width);
		les = GetLineEnds(m_BlackLine2);
		LinkLineEnds180(les, 10, 40);
		ConnectLineEnds3(les, m_BlackLine2, tmp_width);
		les = GetLineEnds(m_BlackLine2);
		LinkLineEnds180(les, 10, 40);
		ConnectLineEnds3(les, m_BlackLine2, tmp_width);
		les = GetLineEnds(m_BlackLine2);
		LinkLineEnds180(les, 15, 20);
		ConnectLineEnds3(les, m_BlackLine2, tmp_width);
		les = GetLineEnds(m_BlackLine2);
		LinkLineEnds180(les, 15, 30);
		ConnectLineEnds3(les, m_BlackLine2, tmp_width);
		les = GetLineEnds(m_BlackLine2);
		LinkLineEnds(les, 5, 30);
		ConnectLineEnds3(les, m_BlackLine2, tmp_width);
//      les = GetLineEnds(m_BlackLine2);
//      LinkLineEnds(les, 5, 180);
//      ConnectLineEnds3(les, m_BlackLine2, tmp_width);
//      les = GetLineEnds(m_BlackLine2);
//      LinkLineEnds(les, 5, 180);
		IncreaseDensity(m_BlackLine2, tmp_width);
		les = GetLineEnds(m_BlackLine2);
		ConnectNearestLines(les, m_BlackLine2, tmp_width, 10, 20);
//      les = GetLineEnds(m_BlackLine2);
//      ConnectLineEnds3(les, m_BlackLine2, tmp_width);
		m_BlackLine2 = SmoothingLen5(m_BlackLine2, 0.4, 5);
		d3dApp.AddLines(m_BlackLine2);
		d3dApp.SetScaleTemporary(1);
		d3dApp.BuildPoint();
		d3dApp.InterSetRenderTransparencyOutput2();
		ccp2_simg = d3dApp.DrawSceneToCvMat();
		d3dApp.SetScaleRecovery();
		les = GetLineEnds(m_BlackLine2);
		for (int i = 0; i < les.size(); ++i)
		{
			Vector2 pb = les[i].beg;
			Vector2 pe = les[i].end;
			ccp2_simg.at<cv::Vec3b>(pb.y, pb.x) = cv::Vec3b(0, 255, 0);
			ccp2_simg.at<cv::Vec3b>(pe.y, pe.x) = cv::Vec3b(255, 128, 0);
		}
		cv::imshow("boundary curves", ccp2_simg);
	}
	cv::waitKey(10);
	Lines isosurfaceLines;
	cv::Mat isoimg, reginimg;
	if (m_ISOSURFACE_REGION)
	{
		isoimg = m_vavImage.Clone();
		//cv::GaussianBlur(isoimg, isoimg, cv::Size(5, 5), 0, 0);
		isoimg = MakeIsoSurfaceImg(isoimg, 32);
		cv::imshow("IsoSurface", isoimg);
	}
	if (m_ISOSURFACE_REGION &&
			m_ISOSURFACE_CONSTRAINT)
	{
		d3dApp.ClearSkeletonLines();
		cv::Mat curveExtration = ccp2_simg.clone();
		if (m_BOUNDARY_CURVES)
		{
			cvtColor(curveExtration, curveExtration, CV_BGR2GRAY);
			Dilation(curveExtration, 2, 1);
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
		cv::Mat isoline = d3dApp.DrawSceneToCvMat();
		d3dApp.SetScaleRecovery();
		cvtColor(isoline, isoline, CV_BGR2GRAY);
		if (m_BOUNDARY_CURVES)
		{
			for (int i = 0; i < curveExtration.rows ; i++)
			{
				for (int j = 0; j < curveExtration.cols ; j++)
				{
					uchar& v1 = curveExtration.at<uchar>(i, j);
					uchar& v2 = isoline.at<uchar>(i, j);
					if (v1 > 0 || v2 > 0)
					{
						v2 = 1;
					}
					else
					{
						v2 = 0;
					}
				}
			}
		}
		cvThin(isoline, isoline, 10);
		normalize(isoline, isoline, 0, 255, cv::NORM_MINMAX);
		cv::imshow("Isosurface Constraint", isoline);
		d3dApp.SetScaleRecovery();
	}
	ColorConstraints ccms;
	ColorConstraints ccms2;
	cv::Mat cmmsIndexImg;
	if (m_BOUNDARY_CURVES &&
			m_REGION_GROWING)
	{
		d3dApp.SetScaleTemporary(2);
		d3dApp.BuildPoint();
		d3dApp.InterSetRenderTransparencyOutput2();
		cv::Mat simg = d3dApp.DrawSceneToCvMat();
		d3dApp.SetScaleRecovery();
		cmmsIndexImg = simg.clone();
		
		cmmsIndexImg = TrapBallMaskAll(cmmsIndexImg);
		cv::Mat vecout = m_vavImage.Clone();
		vecout = cv::Scalar(0);
		cv::Mat ShowRegion = m_vavImage.Clone();
//		Dilation(simg, 1, 2);
		for (int i = 0; i < simg.rows; ++i)
		{
			for (int j = 0; j < simg.cols; ++j)
			{
				if (simg.at<cv::Vec3b>(i, j)[2] > 0)
				{
					ShowRegion.at<cv::Vec3b>(i/2, j/2) = cv::Vec3b(255, 255, 255);
				}
			}
		}
		cv::imshow("ShowRegion", ShowRegion);
		CvPatchs cps = S2_2GetPatchs(cmmsIndexImg, m_vavImage, ccms2, vecout);
		cv::imshow("vecout", vecout);
		//cv::Mat isoimg = MakeIsoSurfaceImg(vecout, 32);
		cv::waitKey(10);
		// set draw patch line to Diffusion
		Lines patchlines = GetLinesFromCvPatch(cps);
		Color2Side patchColor = GetLinesColor(patchlines, ccms2);
		patchlines = SmoothingLen5(patchlines, 0.5, 5);
		d3dApp.AddDiffusionLines(patchlines, patchColor);
		// set click patch
		GetVavView()->m_patchs = cps;
		GetVavView()->m_patchcolor = patchColor;
		GetVavView()->m_patchlines = patchlines;
		// show patch img
		cv::Mat tmp = cmmsIndexImg.clone(), tmp2;
		cv::RNG rng(12345);
		tmp = cv::Scalar(0);
		tmp2 = tmp.clone();
//      for (int i = 0; i < cps.size(); ++i)
//      {
//          cv::Scalar color = cv::Scalar(rng.uniform(0, 200) + 50, rng.uniform(0, 200) + 50, rng.uniform(0,
//                                        200) + 50);
//          cv::Scalar color2 = cv::Scalar(i % 256, (i / 256) % 256, i / (256 * 256));
//          CvLines cltmp;
//          cltmp.push_back(cps[i].Outer2());
//          drawContours(tmp, cltmp, 0, color, -1, 8);
//          drawContours(tmp2, cltmp, 0, color2, -1, 8);
//          //drawContours(tmp, cltmp, 0, cv::Scalar(255, 255, 255), 0, 8);
//      }
		cv::imshow("drawContours", tmp);
		
		S6ReColor(cmmsIndexImg, m_vavImage.Clone(), ccms);
		GetVavView()->m_indexImg = cmmsIndexImg.clone();
		cv::imshow("Trap-Ball Region ReColor", cmmsIndexImg);
		GetVavView()->SetTexture(vavImage(cmmsIndexImg).GetDx11Texture());
		//cmmsIndexImg = ConvertToIndex(cmmsIndexImg, m_vavImage.Clone(), ccms);
		//Index2Side i2s = GetLinesIndex2Side(cmmsIndexImg, m_BlackLine2, 2);
		Index2Side i2s = GetLinesIndex2SideSmart(cmmsIndexImg, m_BlackLine2, ccms);
		i2s.left = FixIndexs(i2s.left, 100);
		i2s.right = FixIndexs(i2s.right, 100);
// 		i2s.left = MedianLenN(i2s.left, 10, 3);
// 		i2s.right = MedianLenN(i2s.right, 10, 3);
		Color2Side color2s2 = LinesIndex2Color(m_BlackLine2, i2s, ccms2);
		color2s2.left = FixLineColors(color2s2.left, 600, 10);
		color2s2.right = FixLineColors(color2s2.right, 600, 10);
		//d3dApp.AddDiffusionLines(m_BlackLine2, color2s2);
		d3dApp.AddLines(m_BlackLine2);
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
		d3dApp.AddDiffusionLines(m_BlackLine2, color2s);
		d3dApp.ClearSkeletonLines();
		d3dApp.SetScaleTemporary(1);
		d3dApp.BuildPoint();
		d3dApp.InterSetRenderTransparencyOutput1();
		cv::imshow("Boundary Curves constraint", d3dApp.DrawSceneToCvMat());
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
//      Vector3s2d colors;
//      colors = GetLinesColor(m_vavImage, isosurfaceLines);
//      colors = FixLineColors(colors, 400, 10);
//      colors = MedianLen(colors, 20, 3);
//      colors = SmoothingLen5(colors, 0, 10);
//      d3dApp.AddDiffusionLines(isosurfaceLines, colors);
		ints2d id2d = GetLinesIndex(cmmsIndexImg, isosurfaceLines);
		id2d = FixIndexs(id2d, 30);
		id2d = MedianLen5(id2d, 3);
		Vector3s2d color2s = LinesIndex2Color(isosurfaceLines, id2d, ccms);
		color2s = FixLineColors(color2s, 600, 10);
//      for (int i = 0; i < colors.size(); ++i)
//      {
//          for (int j = 0; j < colors[i].size(); ++j)
//          {
//              color2s[i][j] = (colors[i][j] + color2s[i][j]) * 0.5;
//          }
//      }
		d3dApp.AddDiffusionLines(isosurfaceLines, color2s);
		cv::waitKey(10);
	}
	GetVavView()->OnDraw(0);
}

