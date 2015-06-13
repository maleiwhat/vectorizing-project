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
    int spcount = (int) log((double) size) * m_spcount ;/*100*/        //150;//
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
            cv::Vec3b salColor = m_SLIC_Img.at<cv::Vec3b> (j, i);   //CV_IMAGE_ELEM(image,element type,row,col)
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
                m_SLIC_Img.at<cv::Vec3b> (i, j) = cv::Vec3b(0, 0, 0);
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
                m_SLIC_Img.at<cv::Vec3b> (p.y, p.x) = color;
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
                        if(markerMask.at<uchar> (i, j))
                        {
                            img.at<cv::Vec3b> (i, j) = ccms3[cc].GetColorCvPoint(j, i);
                        }
                    }
                }
                for(int i = 0; i < img.rows; ++i)
                {
                    for(int j = 0; j < img.cols; ++j)
                    {
                        if(g_region.at<cv::Vec3b> (i, j) [0])
                        {
                            markerMask.at<uchar> (i, j) = 128;
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

void SetDrawFrame(D3DApp& d3dApp, FrameInfo& fi)
{
    d3dApp.ClearTriangles();
    d3dApp.ClearSkeletonLines();
//     d3dApp.AddDiffusionLines(fi.curves2, fi.ocolor2);
    d3dApp.AddLines(fi.curves1);
    d3dApp.AddLinesWidth(fi.curves1, fi.tmplinewidth, fi.ocolor1);
    d3dApp.AddColorTriangles(fi.picmesh1.m_Trangles);
    d3dApp.AddTrianglesLine(fi.picmesh1.m_Trangles);
    d3dApp.BuildPoint();
    d3dApp.DrawScene();
}


void VAV_MainFrame::OnButton_BuildVectorization()
{
    D3DApp& d3dApp = GetVavView()->GetD3DApp();
    FrameInfo fi = ComputeFrame2(m_vavImage);
    m_BackGround.m_FI = fi;
    SetDrawFrame(d3dApp, fi);
    GetVavView()->OnDraw(0);
}


void VAV_MainFrame::OnButton_Zhang09()
{
    D3DApp& d3dApp = GetVavView()->GetD3DApp();
    FrameInfo fi = ComputeFrame09(m_vavImage);
    d3dApp.AddColorTriangles(fi.picmesh1.m_Trangles);
    d3dApp.AddTrianglesLine(fi.picmesh1.m_Trangles);
//  d3dApp.m_RegionDiffusion.SetDiffusionRegions(1);
//  d3dApp.m_RegionDiffusion.SetRegions(fi.tris2d);
//  d3dApp.AddDiffusionLines(fi.curves2, fi.GetLine2Color());
}
