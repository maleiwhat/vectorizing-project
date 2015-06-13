#include "stdafx.h"
#include <fstream>
#include <iostream>
#include "VAV_App.h"
#include "VAV_MainFrm.h"
#include "VAV_View.h"
#include "ConvStr.h"
#include "TriangulationCgal_Sideline.h"
#include "TriangulationCgal_Patch.h"
#include "VoronoiCgal_Patch.h"
#include "CvExtenstion.h"
#include "CvExtenstion2.h"
#include "IFExtenstion.h"
#include "CmCurveEx.h"
#include "Line.h"
#include "CvExtenstion0.h"
#include "zip/ZipFolder.h"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "SavepointImage.h"
#include "L0smoothing.h"
#include <direct.h>
#include "GenerateDiffusion.h"
#include "CurveMatching.h"
#include "cvshowEX.h"
#include "Nodeui.h"


void VAV_MainFrame::OnFileOpenPicture()
{
    CFileDialog dlg(TRUE);
    dlg.m_ofn.lpstrFilter   = L"All Files (*.*)\0*.*\0\0";
    dlg.m_ofn.lpstrTitle    = L"Load File";
    CString filename;
    if(dlg.DoModal() == IDOK)
    {
        filename = dlg.GetPathName(); // return full path and filename
        if(filename.GetLength() > 1)
        {
            D3DApp& d3dApp = GetVavView()->GetD3DApp();
            CMFCRibbonEdit* re;
            CMFCRibbonBaseElement* tmp_ui = 0;
            for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
            {
                tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
                             ID_SPIN_TransparencySelectPatch);
                if(tmp_ui != 0)
                {
                    break;
                }
            }
            re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
            if(NULL != re)
            {
                m_SelectPatchTransparency = atoi(ConvStr::GetStr(
                                                     re->GetEditText().GetString()).c_str());
            }
            d3dApp.SetTransparency_SelectPatch((100 - m_SelectPatchTransparency) * 0.01);
            for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
            {
                tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
                             ID_SPIN_TransparencyPatch);
                if(tmp_ui != 0)
                {
                    break;
                }
            }
            re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
            if(NULL != re)
            {
                m_PatchTransparency = atoi(ConvStr::GetStr(
                                               re->GetEditText().GetString()).c_str());
            }
            d3dApp.SetTransparency_SelectPatch((100 - m_PatchTransparency) * 0.01);
            for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
            {
                tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
                             ID_SPIN_TransparencyTriangleLine);
                if(tmp_ui != 0)
                {
                    break;
                }
            }
            re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
            if(NULL != re)
            {
                m_TriangleLineTransparency = atoi(ConvStr::GetStr(
                                                      re->GetEditText().GetString()).c_str());
            }
            d3dApp.SetTransparency_SelectPatch((100 - m_TriangleLineTransparency) * 0.01);
            for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
            {
                tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
                             ID_SPIN_TransparencyPicture);
                if(tmp_ui != 0)
                {
                    break;
                }
            }
            re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
            if(NULL != re)
            {
                m_PictureTransparency = atoi(ConvStr::GetStr(
                                                 re->GetEditText().GetString()).c_str());
            }
            d3dApp.SetTransparency_Picture((100 - m_PictureTransparency) * 0.01);
            m_vavImage.ReadImage(ConvStr::GetStr(filename.GetString()));
            d3dApp.ClearTriangles();
            GetVavView()->SetPictureSize(m_vavImage.GetWidth(), m_vavImage.GetHeight());
            GetVavView()->SetTexture(m_vavImage.GetDx11Texture());
        }
    }
}

void VAV_MainFrame::OnButtonCanny()
{
    if(!m_vavImage.Vaild())
    {
        return;
    }
    int t1 = 0, t2 = 30, a = 3;
    CMFCRibbonEdit* re;
    CMFCRibbonBaseElement* tmp_ui = 0;
    for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
    {
        tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
                     ID_SPIN_CannyThreshold1);
        if(tmp_ui != 0)
        {
            break;
        }
    }
    re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
    if(NULL != re)
    {
        t1 = atoi(ConvStr::GetStr(re->GetEditText().GetString()).c_str());
    }
    for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
    {
        tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
                     ID_SPIN_CannyThreshold2);
        if(tmp_ui != 0)
        {
            break;
        }
    }
    re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
    if(NULL != re)
    {
        t2 = atoi(ConvStr::GetStr(re->GetEditText().GetString()).c_str());
    }
    for(int i = 0; i < m_wndRibbonBar.GetCategoryCount(); ++i)
    {
        tmp_ui = m_wndRibbonBar.GetCategory(i)->FindByID(
                     ID_SPIN_ApertureSize);
        if(tmp_ui != 0)
        {
            break;
        }
    }
    re = dynamic_cast<CMFCRibbonEdit*>(tmp_ui);
    if(NULL != re)
    {
        a = atoi(ConvStr::GetStr(re->GetEditText().GetString()).c_str());
    }
    m_cannyImage = m_vavImage;
    cv::Mat ce = CannyEdge(m_vavImage, t1, t2, a);
    //  Lines lines1 = ComputeEdgeLine(ce);
    //  Lines lines = m_vavImage.AnimaEdge(5, 0.01, 0.1);
    //  lines = ComputeTrappedBallEdge(m_vavImage, lines, 5);
    //  cv::Mat lineImage = MakeLineImage(m_vavImage, lines);
    //  lines = GetLines(GetSidelines(lineImage));
    //  m_cannyImage.ShowEdgeLine(lines);
    m_cannyImage = ce;
    //  Skeleton(m_cannyImage);
    //  cv::Mat cw, cw2;
    //  cw2 = m_cannyImage;
    //  cw2.convertTo(cw, CV_32FC3);
    //  Collect_Water(cw, cw, 5, 5, m_BlackRegionThreshold * 0.01);
    //  m_cannyImage = cw;
    GetVavView()->SetTexture(m_cannyImage.GetDx11Texture());
}

void VAV_MainFrame::OnButtonSkeleton()
{
    m_cannyImage = m_vavImage;
    //Skeleton(m_cannyImage);
    cv::Mat cw = cv::Mat(m_cannyImage).clone();
    for(int i = 0; i < cw.rows; i++)
    {
        for(int j = 0; j < cw.cols; j++)
        {
            cv::Vec3b& v = cw.at<cv::Vec3b>(i, j);
            v[0] = 255;
            v[1] = 255;
            v[2] = 255;
        }
    }
    m_cannyImage = cw;
    GetVavView()->SetTexture(m_cannyImage.GetDx11Texture());
}

cv::Vec2f getMoveVectorBySIFT(cv::Mat a, cv::Mat b)
{
    int minHessian = 400;
    cv::SurfFeatureDetector detector(minHessian);
    std::vector<cv::KeyPoint> keypoints_1, keypoints_2;
    detector.detect(a, keypoints_1);
    detector.detect(b, keypoints_2);
    cv::SurfDescriptorExtractor extractor;
    cv::Mat descriptors_1, descriptors_2;
    extractor.compute(a, keypoints_1, descriptors_1);
    extractor.compute(b, keypoints_2, descriptors_2);
    cv::FlannBasedMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(descriptors_1, descriptors_2, matches);
    double max_dist = 0;
    double min_dist = 100;
    //-- Quick calculation of max and min distances between keypoints
    for(int i = 0; i < descriptors_1.rows; i++)
    {
        double dist = matches[i].distance;
        if(dist < min_dist)
        {
            min_dist = dist;
        }
        if(dist > max_dist)
        {
            max_dist = dist;
        }
    }
    std::vector<cv::DMatch> good_matches;
    for(int i = 0; i < descriptors_1.rows; i++)
    {
        if(matches[i].distance <= std::max(min_dist * 2, 0.02))
        {
            good_matches.push_back(matches[i]);
        }
    }
    cv::Vec2f mv(0, 0);
    for(int i = 0; i < good_matches.size(); i++)
    {
        cv::Point2f v = keypoints_2[good_matches[i].trainIdx].pt - keypoints_1[good_matches[i].queryIdx].pt;
        mv[0] += v.x;
        mv[1] += v.y;
    }
    mv /= (float)good_matches.size();
    mv[0] *= 10;
    mv[0] = ((int)mv[0]) * 0.1;
    mv[1] *= 10;
    mv[1] = ((int)mv[1]) * 0.1;
    if(mv[0] > 1000 || mv[0] < -1000)
    {
        mv[0] = 0;
    }
    if(mv[1] > 1000 || mv[1] < -1000)
    {
        mv[1] = 0;
    }
    return mv;
}

const double UNKNOWN_FLOW_THRESH = 1e9;

std::vector<cv::Point> markerPoints;
std::vector<cv::Point> outPoints;
cv::Vec2f getMoveVectorByFlow(cv::Mat flow)
{
    cv::Vec2f res;
    outPoints.clear();
    double sumx = 0, sumy = 0;
    int ct = 0;
    for(int i = 0; i < flow.rows; ++i)
    {
        for(int j = 0; j < flow.cols; ++j)
        {
            ct++;
            cv::Vec2f fp = flow.at<cv::Vec2f>(i, j);
            if((fabs(fp[0]) <  UNKNOWN_FLOW_THRESH))
            {
                sumx += fp[0];
            }
            if((fabs(fp[1]) <  UNKNOWN_FLOW_THRESH))
            {
                sumy += fp[1];
            }
        }
    }
    //  cv::Mat flowcopy;
    //  flowcopy.create(flow.rows, flow.cols, CV_8UC3);
    //  flowcopy = cv::Scalar(0);
    //  for(int i = 0; i < flow.rows; i += 20)
    //  {
    //      for(int j = 0; j < flow.cols; j += 20)
    //      {
    //          cv::Vec2f fp = flow.at<cv::Vec2f>(i, j);
    //          cv::Scalar c(0, 255, 0);
    //          cv::line(flowcopy, cv::Point(j, i), cv::Point(j+fp[0]*2, i+fp[1]*2), c);
    //          flowcopy.at<cv::Vec3b>(i, j)=cv::Vec3b(0, 200, 255);
    //      }
    //  }
    //  imshow("flowcopy", flowcopy);
    //  cv::waitKey();
    sumx /= ct;
    sumy /= ct;
    sumx *= 10;
    sumx = ((int)sumx) * 0.1;
    sumy *= 10;
    sumy = ((int)sumy) * 0.1;
    res[0] = sumx;
    res[1] = sumy;
    return res;
}

bool compareVec3b(const cv::Vec3b& a, const cv::Vec3b& b)
{
    if(a[0] < b[0])
    {
        return true;
    }
    if(a[0] > b[0])
    {
        return false;
    }
    if(a[1] < b[1])
    {
        return true;
    }
    if(a[1] > b[1])
    {
        return false;
    }
    if(a[2] < b[2])
    {
        return true;
    }
    if(a[2] > b[2])
    {
        return false;
    }
    return true;
}


void MakeStaticBackGroundByMean(Mats& m_Video, Vec2fs& m_Moves)
{
    if(m_Video.size() != m_Moves.size())
    {
        printf("m_Video.size() %d  m_Moves.size() %d\n", m_Video.size(), m_Moves.size());
        return;
    }
    else
    {
        int maxX = 0, maxY = 0, minX = 0, minY = 0;
        for(int i = 0; i < m_Moves.size(); ++i)
        {
            if(m_Moves[i][0] > maxX)
            {
                maxX = m_Moves[i][0] + 1;
            }
            if(m_Moves[i][1] > maxY)
            {
                maxY = m_Moves[i][1] + 1;
            }
            if(m_Moves[i][0] < minX)
            {
                minX = m_Moves[i][0] - 1;
            }
            if(m_Moves[i][1] < minY)
            {
                minY = m_Moves[i][1] - 1;
            }
        }
        int ow = m_Video.back().cols;
        int oh = m_Video.back().rows;
        const int finalW = ow + maxX - minX + 1;
        const int finalH = oh + maxY - minY + 1;
        cv::Mat bgimg, bgcount;
        Mats timgs;
        bgimg.create(finalH, finalW, CV_32FC3);
        bgcount.create(finalH, finalW, CV_32FC1);
        bgimg = cv::Scalar(0);
        bgcount = cv::Scalar(0);
        for(int a = 0; a < m_Video.size(); ++a)
        {
            cv::Mat timg;
            timg.create(finalH, finalW, CV_8UC3);
            timg = cv::Scalar(0);
            cv::Mat img = m_Video[a];
            cv::Vec2f mv = m_Moves[a];
            double x = mv[0] - minX;
            double y = mv[1] - minY;
            int fx = floor(x);
            int fy = floor(y);
            double lw = 1 - (x - floor(x));
            double rw = 1 - lw;
            double tw = 1 - (y - floor(y));
            double bw = 1 - tw;
            double lt = lw * tw, lb = lw * bw, rt = rw * tw, rb = rw * bw;
            printf("x %.2f y %.2f lw %.2f rw %.2f tw %.2f bw %.2f\n", x, y, lw, rw, tw, bw);
            for(int i = 0; i < img.rows; ++i)
            {
                for(int j = 0; j < img.cols; ++j)
                {
                    cv::Vec3b& tv00 = timg.at<cv::Vec3b>(fy + i, fx + j);
                    cv::Vec3b& tv10 = timg.at<cv::Vec3b>(fy + i + 1, fx + j);
                    cv::Vec3b& tv01 = timg.at<cv::Vec3b>(fy + i, fx + j + 1);
                    cv::Vec3b& tv11 = timg.at<cv::Vec3b>(fy + i + 1, fx + j + 1);
                    tv00[0] += img.at<cv::Vec3b>(i, j)[0] * lt;
                    tv00[1] += img.at<cv::Vec3b>(i, j)[1] * lt;
                    tv00[2] += img.at<cv::Vec3b>(i, j)[2] * lt;
                    tv10[0] += img.at<cv::Vec3b>(i, j)[0] * lb;
                    tv10[1] += img.at<cv::Vec3b>(i, j)[1] * lb;
                    tv10[2] += img.at<cv::Vec3b>(i, j)[2] * lb;
                    tv01[0] += img.at<cv::Vec3b>(i, j)[0] * rt;
                    tv01[1] += img.at<cv::Vec3b>(i, j)[1] * rt;
                    tv01[2] += img.at<cv::Vec3b>(i, j)[2] * rt;
                    tv11[0] += img.at<cv::Vec3b>(i, j)[0] * rb;
                    tv11[1] += img.at<cv::Vec3b>(i, j)[1] * rb;
                    tv11[2] += img.at<cv::Vec3b>(i, j)[2] * rb;
                }
            }
            timgs.push_back(timg);
        }
        cv::Mat meanimg;
        meanimg.create(finalH, finalW, CV_8UC3);
        for(int i = 0; i < bgimg.rows; ++i)
        {
            for(int j = 0; j < bgimg.cols; ++j)
            {
                Vec3bs colors;
                for(int k = 0; k < timgs.size(); ++k)
                {
                    cv::Vec3b c = timgs[k].at<cv::Vec3b>(i, j);
                    if(c != cv::Vec3b(0, 0, 0))
                    {
                        colors.push_back(c);
                    }
                }
                if(!colors.empty())
                {
                    std::sort(colors.begin(), colors.end(), compareVec3b);
                    meanimg.at<cv::Vec3b>(i, j) = colors[colors.size() / 2];
                }
            }
        }
        cv::imshow("meanimg", meanimg);
        printf("minX %d maxX %d minY %d maxY %d \n", minX, maxX, minY, maxY);
        cv::waitKey();
    }
}

double ColorDistanceAfterMove(cv::Mat& a, cv::Mat& b, double x)
{
    if(a.cols == b.cols && a.rows == b.rows)
    {
        cv::Vec3b black(0, 0, 0);
        int count = 0;
        cv::Vec3d diff;
        double y = 0;
        int fx = -floor(x);
        int fy = -floor(y);
        double lw = 1 - (x - floor(x));
        double rw = 1 - lw;
        double tw = 1 - (y - floor(y));
        double bw = 1 - tw;
        double lt = lw * tw, lb = lw * bw, rt = rw * tw, rb = rw * bw;
        //printf("x %.2f y %.2f lw %.2f rw %.2f tw %.2f bw %.2f\n", x, y, lw, rw, tw, bw);
        int ow = a.cols;
        int oh = a.rows;
        const int finalW = ow + abs(fx) + 1;
        const int finalH = oh + abs(fy) + 1;
        cv::Mat bgimg, bgcount, timg1, timg2;
        timg1.create(finalH, finalW, CV_8UC3);
        timg2.create(finalH, finalW, CV_8UC3);
        bgimg.create(finalH, finalW, CV_32FC3);
        bgcount.create(finalH, finalW, CV_32FC1);
        bgimg = cv::Scalar(0);
        bgcount = cv::Scalar(0);
        timg1 = cv::Scalar(0);
        timg2 = cv::Scalar(0);
        for(int i = 0; i < a.rows; ++i)
        {
            for(int j = 0; j < a.cols; ++j)
            {
                cv::Vec3b& tv00 = timg1.at<cv::Vec3b>(fy + i, fx + j);
                cv::Vec3b& tv10 = timg1.at<cv::Vec3b>(fy + i + 1, fx + j);
                cv::Vec3b& tv01 = timg1.at<cv::Vec3b>(fy + i, fx + j + 1);
                cv::Vec3b& tv11 = timg1.at<cv::Vec3b>(fy + i + 1, fx + j + 1);
                float& c00 = bgcount.at<float>(fy + i, fx + j);
                float& c10 = bgcount.at<float>(fy + i + 1, fx + j);
                float& c01 = bgcount.at<float>(fy + i, fx + j + 1);
                float& c11 = bgcount.at<float>(fy + i + 1, fx + j + 1);
                c00 += lt;
                c10 += lb;
                c01 += rt;
                c11 += rb;
                tv00[0] += a.at<cv::Vec3b>(i, j)[0] * lt;
                tv00[1] += a.at<cv::Vec3b>(i, j)[1] * lt;
                tv00[2] += a.at<cv::Vec3b>(i, j)[2] * lt;
                tv10[0] += a.at<cv::Vec3b>(i, j)[0] * lb;
                tv10[1] += a.at<cv::Vec3b>(i, j)[1] * lb;
                tv10[2] += a.at<cv::Vec3b>(i, j)[2] * lb;
                tv01[0] += a.at<cv::Vec3b>(i, j)[0] * rt;
                tv01[1] += a.at<cv::Vec3b>(i, j)[1] * rt;
                tv01[2] += a.at<cv::Vec3b>(i, j)[2] * rt;
                tv11[0] += a.at<cv::Vec3b>(i, j)[0] * rb;
                tv11[1] += a.at<cv::Vec3b>(i, j)[1] * rb;
                tv11[2] += a.at<cv::Vec3b>(i, j)[2] * rb;
            }
        }
        for(int i = 0; i < a.rows + 1; ++i)
        {
            for(int j = 0; j < a.cols + 1; ++j)
            {
                float& c00 = bgcount.at<float>(fy + i, fx + j);
                if(abs(c00 - 1) > 0.01)
                {
                    cv::Vec3b& tv00 = timg1.at<cv::Vec3b>(fy + i, fx + j);
                    tv00 /= c00;
                }
            }
        }
        for(int i = 0; i < b.rows; ++i)
        {
            for(int j = 0; j < b.cols; ++j)
            {
                cv::Vec3b& tv00 = timg2.at<cv::Vec3b>(i, j);
                tv00 = b.at<cv::Vec3b>(i, j);
            }
        }
        for(int i = 0; i < timg2.rows; ++i)
        {
            for(int j = 0; j < timg2.cols; ++j)
            {
                cv::Vec3b& ca = timg1.at<cv::Vec3b>(i, j);
                cv::Vec3b& cb = timg2.at<cv::Vec3b>(i, j);
                if(ca != black && cb != black)
                {
                    count++;
                    diff[0] += abs(ca[0] - cb[0]);
                    diff[1] += abs(ca[1] - cb[1]);
                    diff[2] += abs(ca[2] - cb[2]);
                }
            }
        }
        diff /= count;
        return diff[0] + diff[1] + diff[2];
    }
    return 0;
}


double ColorDistance(cv::Mat& a, cv::Mat& b)
{
    if(a.cols == b.cols && a.rows == b.rows)
    {
        cv::Vec3b black(0, 0, 0);
        int count = 0;
        cv::Vec3d diff;
        for(int i = 0; i < a.rows; ++i)
        {
            for(int j = 0; j < b.cols; ++j)
            {
                cv::Vec3b& ca = a.at<cv::Vec3b>(i, j);
                cv::Vec3b& cb = b.at<cv::Vec3b>(i, j);
                if(ca != black && cb != black)
                {
                    count++;
                    diff[0] += abs(ca[0] - cb[0]);
                    diff[1] += abs(ca[1] - cb[1]);
                    diff[2] += abs(ca[2] - cb[2]);
                }
            }
        }
        diff /= count;
        return diff[0] + diff[1] + diff[2];
    }
    return 0;
}

void ImgFillBlack(cv::Mat& a, cv::Mat& b)
{
    if(a.cols == b.cols && a.rows == b.rows)
    {
        cv::Vec3b black(0, 0, 0);
        for(int i = 0; i < a.rows; ++i)
        {
            for(int j = 0; j < b.cols; ++j)
            {
                cv::Vec3b& ca = a.at<cv::Vec3b>(i, j);
                cv::Vec3b& cb = b.at<cv::Vec3b>(i, j);
                if(ca != black && cb == black)
                {
                    cb = ca;
                }
                //              else if(ca != black && cb != black)
                //              {
                //                  cb = ca * 0.9 + cb * 0.1;
                //              }
            }
        }
    }
}

Mats MakeStaticBackGroundByMove(Mats& m_Video, Vec2fs& m_Moves, cv::Mat& background, cv::Mat& foreground)
{
    if(m_Video.size() != m_Moves.size())
    {
        printf("m_Video.size() %d  m_Moves.size() %d\n", m_Video.size(), m_Moves.size());
        return Mats();
    }
    else
    {
        int maxX = 0, maxY = 0, minX = 0, minY = 0;
        for(int i = 0; i < m_Moves.size(); ++i)
        {
            if(m_Moves[i][0] > maxX)
            {
                maxX = m_Moves[i][0] + 1;
            }
            if(m_Moves[i][1] > maxY)
            {
                maxY = m_Moves[i][1] + 1;
            }
            if(m_Moves[i][0] < minX)
            {
                minX = m_Moves[i][0] - 1;
            }
            if(m_Moves[i][1] < minY)
            {
                minY = m_Moves[i][1] - 1;
            }
        }
        int ow = m_Video.back().cols;
        int oh = m_Video.back().rows;
        const int finalW = ow + maxX - minX + 1;
        const int finalH = oh + maxY - minY + 1;
        printf("finalW %d finalH %d\n", finalW, finalH);
        cv::Mat bgimg, bgcount;
        Mats timgs;
        bgimg.create(finalH, finalW, CV_32FC3);
        bgcount.create(finalH, finalW, CV_32FC1);
        bgimg = cv::Scalar(0);
        for(int a = 0; a < m_Video.size(); ++a)
        {
            bgcount = cv::Scalar(0);
            cv::Mat timg;
            timg.create(finalH, finalW, CV_8UC3);
            timg = cv::Scalar(0);
            cv::Mat img = m_Video[a];
            cv::Vec2f mv = m_Moves[a];
            double x = mv[0] - minX;
            double y = mv[1] - minY;
            int fx = floor(x);
            int fy = floor(y);
            double lw = 1 - (x - floor(x));
            double rw = 1 - lw;
            double tw = 1 - (y - floor(y));
            double bw = 1 - tw;
            double lt = lw * tw, lb = lw * bw, rt = rw * tw, rb = rw * bw;
            printf("x %.2f y %.2f lw %.2f rw %.2f tw %.2f bw %.2f\n", x, y, lw, rw, tw, bw);
            for(int i = 0; i < img.rows; ++i)
            {
                for(int j = 0; j < img.cols; ++j)
                {
                    cv::Vec3b& tv00 = timg.at<cv::Vec3b>(fy + i, fx + j);
                    cv::Vec3b& tv10 = timg.at<cv::Vec3b>(fy + i + 1, fx + j);
                    cv::Vec3b& tv01 = timg.at<cv::Vec3b>(fy + i, fx + j + 1);
                    cv::Vec3b& tv11 = timg.at<cv::Vec3b>(fy + i + 1, fx + j + 1);
                    float& c00 = bgcount.at<float>(fy + i, fx + j);
                    float& c10 = bgcount.at<float>(fy + i + 1, fx + j);
                    float& c01 = bgcount.at<float>(fy + i, fx + j + 1);
                    float& c11 = bgcount.at<float>(fy + i + 1, fx + j + 1);
                    c00 += lt;
                    c10 += lb;
                    c01 += rt;
                    c11 += rb;
                    tv00[0] += img.at<cv::Vec3b>(i, j)[0] * lt;
                    tv00[1] += img.at<cv::Vec3b>(i, j)[1] * lt;
                    tv00[2] += img.at<cv::Vec3b>(i, j)[2] * lt;
                    tv10[0] += img.at<cv::Vec3b>(i, j)[0] * lb;
                    tv10[1] += img.at<cv::Vec3b>(i, j)[1] * lb;
                    tv10[2] += img.at<cv::Vec3b>(i, j)[2] * lb;
                    tv01[0] += img.at<cv::Vec3b>(i, j)[0] * rt;
                    tv01[1] += img.at<cv::Vec3b>(i, j)[1] * rt;
                    tv01[2] += img.at<cv::Vec3b>(i, j)[2] * rt;
                    tv11[0] += img.at<cv::Vec3b>(i, j)[0] * rb;
                    tv11[1] += img.at<cv::Vec3b>(i, j)[1] * rb;
                    tv11[2] += img.at<cv::Vec3b>(i, j)[2] * rb;
                }
            }
            for(int i = 0; i < img.rows + 1; ++i)
            {
                for(int j = 0; j < img.cols + 1; ++j)
                {
                    float& c00 = bgcount.at<float>(fy + i, fx + j);
                    if(abs(c00 - 1) > 0.01)
                    {
                        cv::Vec3b& tv00 = timg.at<cv::Vec3b>(fy + i, fx + j);
                        tv00 /= c00;
                    }
                }
            }
            timgs.push_back(timg);
        }
        cv::Mat meanimg;
        meanimg = timgs.front().clone();
        ints useids;
        ImgFillBlack(timgs.front(), meanimg);
        ImgFillBlack(timgs.back(), meanimg);
        cv::imwrite("meanimg.png", meanimg);
        for(int k = 1; k < timgs.size() - 1; ++k)
        {
            double mindis = ColorDistance(meanimg, timgs[k + 1]);
            int idx = k + 1;
            for(int j = k + 2; j <= k + 10 && j < timgs.size(); ++j)
            {
                double dis = ColorDistance(meanimg, timgs[k + 1]);
                if(dis < mindis)
                {
                    mindis = dis;
                    idx = j;
                }
            }
            k = idx;
            ImgFillBlack(timgs[idx], meanimg);
            useids.push_back(idx);
        }
        cv::Mat errimg;
        errimg.create(finalH, finalW, CV_32FC1);
        cv::Vec3b black(0, 0, 0);
        for(int k = 0; k < timgs.size() - 1; ++k)
        {
            for(int i = 0; i < errimg.rows; ++i)
            {
                for(int j = 0; j < errimg.cols; ++j)
                {
                    cv::Vec3b tc1 = timgs[k].at<cv::Vec3b>(i, j);
                    cv::Vec3b tc2 = timgs[k + 1].at<cv::Vec3b>(i, j);
                    if(tc1 != black && tc2 != black)
                    {
                        float aa = abs(tc2[0] - tc1[0]);
                        float bb = abs(tc2[1] - tc1[1]);
                        float cc = abs(tc2[2] - tc1[2]);
                        errimg.at<float>(i, j) += aa * aa;
                        errimg.at<float>(i, j) += bb * bb;
                        errimg.at<float>(i, j) += cc * cc;
                    }
                }
            }
        }
        normalize(errimg, errimg, 0, 1, cv::NORM_MINMAX);
        for(int i = 0; i < errimg.rows; ++i)
        {
            for(int j = 0; j < errimg.cols; ++j)
            {
                if(errimg.at<float>(i, j) > 0.15)
                {
                    errimg.at<float>(i, j) = 1;
                }
                else
                {
                    errimg.at<float>(i, j) = 0;
                }
            }
        }
        cv::Mat forDilation;
        errimg.convertTo(forDilation, CV_8UC1, 255);
		Dilation(forDilation);
        foreground = forDilation;
        background = meanimg;
        printf("minX %d maxX %d minY %d maxY %d \n", minX, maxX, minY, maxY);
        return timgs;
    }
    return Mats();
}

void MakeStaticBackGroundByAvg(Mats& m_Video, Vec2fs& m_Moves, cv::Mat& background)
{
    if(m_Video.size() != m_Moves.size())
    {
        printf("m_Video.size() %d  m_Moves.size() %d\n", m_Video.size(), m_Moves.size());
        return;
    }
    else
    {
        int maxX = 0, maxY = 0, minX = 0, minY = 0;
        for(int i = 0; i < m_Moves.size(); ++i)
        {
            if(m_Moves[i][0] > maxX)
            {
                maxX = m_Moves[i][0] + 1;
            }
            if(m_Moves[i][1] > maxY)
            {
                maxY = m_Moves[i][1] + 1;
            }
            if(m_Moves[i][0] < minX)
            {
                minX = m_Moves[i][0] - 1;
            }
            if(m_Moves[i][1] < minY)
            {
                minY = m_Moves[i][1] - 1;
            }
        }
        int ow = m_Video.back().cols;
        int oh = m_Video.back().rows;
        const int finalW = ow + maxX - minX + 1;
        const int finalH = oh + maxY - minY + 1;
        cv::Mat bgimg, bgcount;
        Mats timgs;
        bgimg.create(finalH, finalW, CV_32FC3);
        bgcount.create(finalH, finalW, CV_32FC1);
        bgimg = cv::Scalar(0);
        bgcount = cv::Scalar(0);
        for(int a = 0; a < m_Video.size(); ++a)
        {
            cv::Mat timg;
            timg.create(finalH, finalW, CV_8UC3);
            timg = cv::Scalar(0);
            cv::Mat img = m_Video[a];
            cv::Vec2f mv = m_Moves[a];
            double x = mv[0] - minX;
            double y = mv[1] - minY;
            int fx = floor(x);
            int fy = floor(y);
            double lw = 1 - (x - floor(x));
            double rw = 1 - lw;
            double tw = 1 - (y - floor(y));
            double bw = 1 - tw;
            double lt = lw * tw, lb = lw * bw, rt = rw * tw, rb = rw * bw;
            printf("x %.2f y %.2f lw %.2f rw %.2f tw %.2f bw %.2f\n", x, y, lw, rw, tw, bw);
            for(int i = 0; i < img.rows; ++i)
            {
                for(int j = 0; j < img.cols; ++j)
                {
                    cv::Vec3f& v00 = bgimg.at<cv::Vec3f>(fy + i, fx + j);
                    cv::Vec3f& v10 = bgimg.at<cv::Vec3f>(fy + i + 1, fx + j);
                    cv::Vec3f& v01 = bgimg.at<cv::Vec3f>(fy + i, fx + j + 1);
                    cv::Vec3f& v11 = bgimg.at<cv::Vec3f>(fy + i + 1, fx + j + 1);
                    float& c00 = bgcount.at<float>(fy + i, fx + j);
                    float& c10 = bgcount.at<float>(fy + i + 1, fx + j);
                    float& c01 = bgcount.at<float>(fy + i, fx + j + 1);
                    float& c11 = bgcount.at<float>(fy + i + 1, fx + j + 1);
                    c00 += lt;
                    c10 += lb;
                    c01 += rt;
                    c11 += rb;
                    v00[0] += img.at<cv::Vec3b>(i, j)[0] * lt;
                    v00[1] += img.at<cv::Vec3b>(i, j)[1] * lt;
                    v00[2] += img.at<cv::Vec3b>(i, j)[2] * lt;
                    v10[0] += img.at<cv::Vec3b>(i, j)[0] * lb;
                    v10[1] += img.at<cv::Vec3b>(i, j)[1] * lb;
                    v10[2] += img.at<cv::Vec3b>(i, j)[2] * lb;
                    v01[0] += img.at<cv::Vec3b>(i, j)[0] * rt;
                    v01[1] += img.at<cv::Vec3b>(i, j)[1] * rt;
                    v01[2] += img.at<cv::Vec3b>(i, j)[2] * rt;
                    v11[0] += img.at<cv::Vec3b>(i, j)[0] * rb;
                    v11[1] += img.at<cv::Vec3b>(i, j)[1] * rb;
                    v11[2] += img.at<cv::Vec3b>(i, j)[2] * rb;
                }
            }
            timgs.push_back(timg);
        }
        for(int i = 0; i < bgimg.rows; ++i)
        {
            for(int j = 0; j < bgimg.cols; ++j)
            {
                bgimg.at<cv::Vec3f>(i, j) /= bgcount.at<float>(i, j);
            }
        }
        cv::Mat showbg;
        bgimg.convertTo(showbg, CV_8UC3);
		background = showbg;
        printf("minX %d maxX %d minY %d maxY %d \n", minX, maxX, minY, maxY);
    }
}

void MakeStaticBackGround(Mats& m_Video, Vec2fs& m_Moves)
{
    if(m_Video.size() != m_Moves.size())
    {
        printf("m_Video.size() %d  m_Moves.size() %d\n", m_Video.size(), m_Moves.size());
        return;
    }
    else
    {
        int maxX = 0, maxY = 0, minX = 0, minY = 0;
        for(int i = 0; i < m_Moves.size(); ++i)
        {
            if(m_Moves[i][0] > maxX)
            {
                maxX = m_Moves[i][0] + 1;
            }
            if(m_Moves[i][1] > maxY)
            {
                maxY = m_Moves[i][1] + 1;
            }
            if(m_Moves[i][0] < minX)
            {
                minX = m_Moves[i][0] - 1;
            }
            if(m_Moves[i][1] < minY)
            {
                minY = m_Moves[i][1] - 1;
            }
        }
        int ow = m_Video.back().cols;
        int oh = m_Video.back().rows;
        cv::Mat bgimg, bgcount;
        Mats timgs;
        const int finalW = ow + maxX - minX + 1;
        const int finalH = oh + maxY - minY + 1;
        bgimg.create(finalH, finalW, CV_32FC3);
        bgcount.create(finalH, finalW, CV_32FC1);
        bgimg = cv::Scalar(0);
        bgcount = cv::Scalar(0);
        for(int a = 0; a < m_Video.size(); ++a)
        {
            cv::Mat timg;
            timg.create(finalH, finalW, CV_8UC3);
            timg = cv::Scalar(0);
            cv::Mat img = m_Video[a];
            cv::Vec2f mv = m_Moves[a];
            double x = mv[0] - minX;
            double y = mv[1] - minY;
            int fx = floor(x);
            int fy = floor(y);
            double lw = 1 - (x - floor(x));
            double rw = 1 - lw;
            double tw = 1 - (y - floor(y));
            double bw = 1 - tw;
            double lt = lw * tw, lb = lw * bw, rt = rw * tw, rb = rw * bw;
            printf("x %.2f y %.2f lw %.2f rw %.2f tw %.2f bw %.2f\n", x, y, lw, rw, tw, bw);
            for(int i = 0; i < img.rows; ++i)
            {
                for(int j = 0; j < img.cols; ++j)
                {
                    cv::Vec3b& tv00 = timg.at<cv::Vec3b>(fy + i, fx + j);
                    cv::Vec3b& tv10 = timg.at<cv::Vec3b>(fy + i + 1, fx + j);
                    cv::Vec3b& tv01 = timg.at<cv::Vec3b>(fy + i, fx + j + 1);
                    cv::Vec3b& tv11 = timg.at<cv::Vec3b>(fy + i + 1, fx + j + 1);
                    cv::Vec3f& v00 = bgimg.at<cv::Vec3f>(fy + i, fx + j);
                    cv::Vec3f& v10 = bgimg.at<cv::Vec3f>(fy + i + 1, fx + j);
                    cv::Vec3f& v01 = bgimg.at<cv::Vec3f>(fy + i, fx + j + 1);
                    cv::Vec3f& v11 = bgimg.at<cv::Vec3f>(fy + i + 1, fx + j + 1);
                    float& c00 = bgcount.at<float>(fy + i, fx + j);
                    float& c10 = bgcount.at<float>(fy + i + 1, fx + j);
                    float& c01 = bgcount.at<float>(fy + i, fx + j + 1);
                    float& c11 = bgcount.at<float>(fy + i + 1, fx + j + 1);
                    c00 += lt;
                    c10 += lb;
                    c01 += rt;
                    c11 += rb;
                    v00[0] += img.at<cv::Vec3b>(i, j)[0] * lt;
                    v00[1] += img.at<cv::Vec3b>(i, j)[1] * lt;
                    v00[2] += img.at<cv::Vec3b>(i, j)[2] * lt;
                    v10[0] += img.at<cv::Vec3b>(i, j)[0] * lb;
                    v10[1] += img.at<cv::Vec3b>(i, j)[1] * lb;
                    v10[2] += img.at<cv::Vec3b>(i, j)[2] * lb;
                    v01[0] += img.at<cv::Vec3b>(i, j)[0] * rt;
                    v01[1] += img.at<cv::Vec3b>(i, j)[1] * rt;
                    v01[2] += img.at<cv::Vec3b>(i, j)[2] * rt;
                    v11[0] += img.at<cv::Vec3b>(i, j)[0] * rb;
                    v11[1] += img.at<cv::Vec3b>(i, j)[1] * rb;
                    v11[2] += img.at<cv::Vec3b>(i, j)[2] * rb;
                    tv00[0] += img.at<cv::Vec3b>(i, j)[0] * lt;
                    tv00[1] += img.at<cv::Vec3b>(i, j)[1] * lt;
                    tv00[2] += img.at<cv::Vec3b>(i, j)[2] * lt;
                    tv10[0] += img.at<cv::Vec3b>(i, j)[0] * lb;
                    tv10[1] += img.at<cv::Vec3b>(i, j)[1] * lb;
                    tv10[2] += img.at<cv::Vec3b>(i, j)[2] * lb;
                    tv01[0] += img.at<cv::Vec3b>(i, j)[0] * rt;
                    tv01[1] += img.at<cv::Vec3b>(i, j)[1] * rt;
                    tv01[2] += img.at<cv::Vec3b>(i, j)[2] * rt;
                    tv11[0] += img.at<cv::Vec3b>(i, j)[0] * rb;
                    tv11[1] += img.at<cv::Vec3b>(i, j)[1] * rb;
                    tv11[2] += img.at<cv::Vec3b>(i, j)[2] * rb;
                }
            }
            timgs.push_back(timg);
        }
        cv::Mat meanimg;
        meanimg.create(finalH, finalW, CV_8UC3);
        for(int i = 0; i < bgimg.rows; ++i)
        {
            for(int j = 0; j < bgimg.cols; ++j)
            {
                Vec3bs colors;
                for(int k = 0; k < timgs.size(); ++k)
                {
                    cv::Vec3b c = timgs[k].at<cv::Vec3b>(i, j);
                    if(c != cv::Vec3b(0, 0, 0))
                    {
                        colors.push_back(c);
                    }
                }
                if(!colors.empty())
                {
                    std::sort(colors.begin(), colors.end(), compareVec3b);
                    meanimg.at<cv::Vec3b>(i, j) = colors[colors.size() / 2];
                }
            }
        }
        for(int i = 0; i < bgimg.rows; ++i)
        {
            for(int j = 0; j < bgimg.cols; ++j)
            {
                bgimg.at<cv::Vec3f>(i, j) /= bgcount.at<float>(i, j);
            }
        }
        cv::Mat showbg;
        bgimg.convertTo(showbg, CV_8UC3);
        //normalize(showbg, showbg, 0, 255, cv::NORM_MINMAX);
        cv::imshow("showbg", showbg);
        cv::imshow("meanimg", meanimg);
        cv::waitKey();
        printf("minX %d maxX %d minY %d maxY %d \n", minX, maxX, minY, maxY);
    }
}

void VAV_MainFrame::OnFileOpenVideo()
{
    CFileDialog dlg(TRUE);
    dlg.m_ofn.lpstrFilter   = L"All Files (*.*)\0*.*\0\0";
    dlg.m_ofn.lpstrTitle    = L"Load File";
    CString filepath;
    CString filename;
    CString filefolder;
    if(dlg.DoModal() == IDOK)
    {
        filepath = dlg.GetPathName(); // return full path and filename
        filefolder = dlg.GetFolderPath();
        filename = dlg.GetFileName();
        if(filepath.GetLength() > 1)
        {
            std::string path = ConvStr::GetStr(filepath.GetString());
            std::string folder = ConvStr::GetStr(filefolder.GetString());
            std::string name = ConvStr::GetStr(filename.GetString());
            name.resize(name.size() - 4);
            folder += "\\tmp";
            _mkdir(folder.c_str());
            folder += "\\";
            m_Video.clear();
            m_Moves.clear();
            m_ReadVideo.Read(path);
            std::string bgfilename = folder + name + "_MSBGBM_BG.png";
            std::string fgfilename = folder + name + "_MSBGBM_FG.png";
            SavepointImage si_bg(bgfilename);
            SavepointImage si_fg(fgfilename);
            cv::Mat fg, bg;
            Mats timgs;
            //          if(si_bg.hasImage())
            //          {
            //              bg = si_bg.ReadImage();
            //              fg = si_fg.ReadImage();
            //              cvtColor(fg, fg, CV_BGR2GRAY);
            //          }
            //          else
            {
                cv::Mat img;
                path.resize(path.size() - 4);
                std::string video0str = folder + name + "_0000.png";
                SavepointImage vimg0(video0str);
                if(vimg0.hasImage())
                {
                    img = vimg0.ReadImage();
                }
                else
                {
                    img = m_ReadVideo.GetFrame();
                    vimg0.SaveImage(img);
                }
                GetVavView()->SetPictureSize(img.cols, img.rows);
                m_Video.push_back(img);
                m_Moves.push_back(cv::Vec2f(0, 0));
                cv::Mat prevgray, gray, flow, showflow;
                cvtColor(img, prevgray, CV_BGR2GRAY);
                // 讀幾個frame
                for(int i = 1; i <= 100; ++i)
                {
                    char tmppath[100];
                    sprintf(tmppath, "_%04d.png", i);
                    SavepointImage vimgx(folder + name + tmppath);
                    cv::Mat imgx;
                    if(vimgx.hasImage())
                    {
                        imgx = vimgx.ReadImage();
                    }
                    else
                    {
                        imgx = m_ReadVideo.GetFrame();
                        vimgx.SaveImage(imgx);
                    }
                    if(imgx.cols > 0)
                    {
                        m_Video.push_back(imgx);
                        cvtColor(imgx, gray, CV_BGR2GRAY);
                        cv::Vec2f move2 = getMoveVectorBySIFT(prevgray, gray);
                        m_Moves.push_back(-move2);
                        //printf("avg_vel:%f,%f\n", move[0], move[1]);
                        printf("Frame %03d   ", i);
                        printf("avg_vel2:%f,%f\n", move2[0], move2[1]);
                        //prevgray = gray.clone();
                    }
                }
                timgs = MakeStaticBackGroundByMove(m_Video, m_Moves, bg, fg);
                //Dilation(fg, 2, 3);
                if(si_bg.hasImage())
                {
                    bg = si_bg.ReadImage();
                    fg = si_fg.ReadImage();
                    cvtColor(fg, fg, CV_BGR2GRAY);
                }
                else
                {
                    si_bg.SaveImage(bg);
                    si_fg.SaveImage(fg);
                }
                m_FrameInfos.clear();
                cv::Vec3b black(0, 0, 0);
                cv::Mat bigfg = fg.clone();
                Dilation(bigfg, 2, 4);
                for(int t = 0; t < timgs.size(); ++t)
                {
                    cv::Mat timg = timgs[t];
                    char tmppath[100];
                    sprintf(tmppath, "_FG_%04d.png", t);
                    cv::imwrite(folder + name + tmppath, timg);
                    for(int i = 0; i < bigfg.rows; ++i)
                    {
                        for(int j = 0; j < bigfg.cols; ++j)
                        {
                            if(bigfg.at<uchar>(i, j) == 0) // || timg.at<cv::Vec3b>(i, j) == black)
                            {
                                //timg.at<cv::Vec3b>(i, j) = bg.at<cv::Vec3b>(i, j);
                                timg.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
                            }
                        }
                    }
                    sprintf(tmppath, "_FGx_%04d.png", t);
                    cv::imwrite(folder + name + tmppath, timg);
                }
            }
            cv::Mat l0bg;
            std::string l0bgfilename = folder + name + "_L0S_BG.png";
            SavepointImage si_l0bg(l0bgfilename);
            if(si_l0bg.hasImage())
            {
                l0bg = si_l0bg.ReadImage();
            }
            else
            {
                l0bg = L0Smoothing(bg, 0.001);
                si_l0bg.SaveImage(l0bg);
            }
            double minx = m_Moves.front()[0];
            double miny = m_Moves.front()[1];
            for(int i = 1; i < m_Moves.size(); ++i)
            {
                if(minx > m_Moves[i][0])
                {
                    minx = m_Moves[i][0];
                }
                if(miny > m_Moves[i][1])
                {
                    miny = m_Moves[i][1];
                }
            }
            for(int i = 1; i < m_Moves.size(); ++i)
            {
                m_Moves[i][0] -= minx;
                m_Moves[i][1] -= miny;
            }
            D3DApp& d3dApp = GetVavView()->GetD3DApp();
            ColorRegion cr;
            FrameInfo fi;
            fi = ComputeFrame2(l0bg, &cr);
            printf("cr.ccms.size(%d)\n", cr.ccms.size());
            cv::Mat xfg = fg.clone();
            Dilation(xfg, 2, 1);
            RemoveBGs_FG_Part(fi, xfg);
            {
                cv::Mat tfg = fg.clone();
                Dilation(tfg, 2, 5);
                for(int t = 0; t < timgs.size(); ++t)
                {
                    printf("%d ", t);
                    cv::Mat timg = timgs[t];
                    FrameInfo fgfi = ComputeFrame2FG(timg, tfg, &cr);
                    //FrameInfo fgfi = ComputeFrameFGIsoline(timg);
                    m_FrameInfos.push_back(fgfi);
                    {
                        // show line for RemoveFGs_BG_Part
                        cv::Mat simg = timg.clone();
                        cv::resize(simg.clone(), simg, simg.size() * 2);
                        simg = cv::Scalar(0);
                        for(int i = 0; i < fgfi.curves1.size(); ++i)
                        {
                            Line& nowl = fgfi.curves1[i];
                            for(int j = 0; j < nowl.size() - 1; ++j)
                            {
                                cv::Point p1(nowl[j].x * 2, nowl[j].y * 2);
                                cv::Point p2(nowl[j + 1].x * 2, nowl[j + 1].y * 2);
                                cv::line(simg, p1, p2, cv::Scalar(255, 255, 255));
                            }
                        }
//                      static int sid1 = -1;
//                      sid1++;
//                      char path[100];
//                      FloodFillReColor(simg);
//                      sprintf(path, "cmms1_%d.png", sid1);
//                      cv::imwrite(path, simg);
                    }
                }
                //SaveFrameInfos("fg.gg", m_FrameInfos);
                //m_FrameInfos = LoadFrameInfos("fg.gg");
            }
            BackGround bgdata;
            bgdata.m_FI = fi;
            bgdata.m_Moves = m_Moves;
            SaveBGInfos("bgdata.gg", bgdata);
            //bgdata = LoadBGInfos("bgdata.gg");
            m_BackGround = bgdata;
        }
        GetVavView()->SetTimer(100, 80, 0);
    }
}


void VAV_MainFrame::OnFileOpenVideo2()
{
    CFileDialog dlg(TRUE);
    dlg.m_ofn.lpstrFilter   = L"All Files (*.*)\0*.*\0\0";
    dlg.m_ofn.lpstrTitle    = L"Load File";
    CString filepath;
    CString filename;
    CString filefolder;
    Mats timgs;
    if(dlg.DoModal() == IDOK)
    {
        cv::Mat fg, bg;
        filepath = dlg.GetPathName(); // return full path and filename
        filefolder = dlg.GetFolderPath();
        filename = dlg.GetFileName();
        std::string bgfilename;
        std::string fgfilename;
        if(filepath.GetLength() > 1)
        {
            std::string path = ConvStr::GetStr(filepath.GetString());
            std::string folder = ConvStr::GetStr(filefolder.GetString());
            std::string name = ConvStr::GetStr(filename.GetString());
            name.resize(name.size() - 4);
            folder += "\\tmp";
            _mkdir(folder.c_str());
            folder += "\\";
            m_Video.clear();
            m_Moves.clear();
            m_ReadVideo.Read(path);
            bgfilename = folder + name + "_MSBGBM_BG.png";
            fgfilename = folder + name + "_MSBGBM_FG.png";
            //          if(si_bg.hasImage())
            //          {
            //              bg = si_bg.ReadImage();
            //              fg = si_fg.ReadImage();
            //              cvtColor(fg, fg, CV_BGR2GRAY);
            //          }
            //          else
            {
                cv::Mat img;
                path.resize(path.size() - 4);
                std::string video0str = folder + name + "_0000.png";
                SavepointImage vimg0(video0str);
                if(vimg0.hasImage())
                {
                    img = vimg0.ReadImage();
                }
                else
                {
                    img = m_ReadVideo.GetFrame();
                    vimg0.SaveImage(img);
                }
                GetVavView()->SetPictureSize(img.cols, img.rows);
                m_Video.push_back(img);
                m_Moves.push_back(cv::Vec2f(0, 0));
                cv::Mat prevgray, gray, flow, showflow;
                cvtColor(img, prevgray, CV_BGR2GRAY);
                // 讀幾個frame
                cv::Mat lastimg;
				int forloop = 10;
				//std::cin >> forloop;
                for(int i = 1; i <= forloop; ++i)
                {
                    char tmppath[100];
                    sprintf(tmppath, "_%04d.png", i);
                    SavepointImage vimgx(folder + name + tmppath);
                    cv::Mat imgx;
                    if(vimgx.hasImage())
                    {
                        imgx = vimgx.ReadImage();
                    }
                    else
                    {
                        imgx = m_ReadVideo.GetFrame();
                        vimgx.SaveImage(imgx);
                    }
                    if(imgx.cols > 0 && i > 1)
                    {
                        m_Video.push_back(imgx);
                        timgs.push_back(imgx);
                        cvtColor(imgx, gray, CV_BGR2GRAY);
                        cv::Vec2f move2 = getMoveVectorBySIFT(prevgray, gray);
                        //prevgray = gray.clone();
                        m_Moves.push_back(move2);
                        //printf("avg_vel:%f,%f\n", move[0], move[1]);
                        printf("Frame %03d   ", i);
                        printf("avg_vel2:%f,%f\n", move2[0], move2[1]);
                        FrameInfo fi = ComputeFrame2(imgx);
                        m_FrameInfos.push_back(fi);
                        lastimg = imgx;
                    }
                }
            }
        }
        Vec2fs      moves;
        double mx = 0, my = 0;
        for(int i = 0; i < m_Moves.size(); ++i)
        {
            moves.push_back(-m_Moves[i]);
        }
        double minx = moves.front()[0];
        double miny = moves.front()[1];
        for(int i = 1; i < moves.size(); ++i)
        {
            if(minx > moves[i][0])
            {
                minx = moves[i][0];
            }
            if(miny > moves[i][1])
            {
                miny = moves[i][1];
            }
        }
        MakeStaticBackGroundByMove(m_Video, moves, bg, fg);
		//MakeStaticBackGroundByAvg(m_Video, moves, bg);
        SavepointImage si_bg(bgfilename);
        SavepointImage si_fg(fgfilename);
        si_bg.SaveImage(bg);
        si_fg.SaveImage(fg);
        FrameInfo bgfi = ComputeFrame2(bg);
//         m_FrameInfos[0].picmesh1.MappingMesh(m_FrameInfos[0].picmesh1, 0, 0);
//         m_FrameInfos[0].picmesh1.MakeColor6(timgs[0]);
        Vector3s colors;
        for(int i = 0; i < 256; ++i)
        {
            colors.push_back(Vector3(rand() % 256, rand() % 256, rand() % 256));
        }
        //bgfi.picmesh1.MakeColor1();
        D3DApp& d3dApp = GetVavView()->GetD3DApp();
        d3dApp.SetPictureSize(bg.cols, bg.rows);

        d3dApp.SetPictureSize(m_Video[0].cols, m_Video[0].rows);
        g_Nodeui.m_viewer->m_maxFrame = m_FrameInfos.size() - 1;

		// make key frame color
		bgfi.picmesh1.MakeColor6(bg);
        m_FrameInfos.push_back(bgfi);
        for(int i = 0; i < m_FrameInfos.size() - 2; ++i)
        {
//             movex += -m_Moves[i + 1][0];
//             movey += -m_Moves[i + 1][1];
            g_Nodeui.m_viewer->AddNodeLine(bgfi.picmesh1.m_Regions.size());
            m_FrameInfos[i].picmesh1.MappingMeshByColor(bgfi.picmesh1, moves[i + 1][0] - minx, moves[i + 1][1] - miny,
				m_Video[i+1], bg);
            if(i > 0)
            {
                g_Nodeui.m_viewer->AddLink(m_FrameInfos[i - 1].picmesh1.m_MapingRegionIDs,
                                           m_FrameInfos[i].picmesh1.m_MapingRegionIDs);
            }
            //m_FrameInfos[i].picmesh1.MappingMeshMappingMeshByMidPointm_FrameInfos[i-1].picmesh1, -m_Moves[i+1][0], -m_Moves[i+1][1]);

            m_FrameInfos[i].picmesh1.MakeColor6(m_Video[i+1]);
            //m_FrameInfos[i].picmesh1.MakeColorX1(colors);
        }

        int csize = m_FrameInfos[0].picmesh1.m_Regions.size();
        ColorConstraints Colors = m_FrameInfos[0].picmesh1.m_ColorConstraint;
        Vector3s Colors2;
        ints idxs;
        for(int i = 0; i < csize; ++i)
        {
            Colors2.push_back(Vector3(rand() % 256, rand() % 256, rand() % 256));
            idxs.push_back(i);
        }
        movex = -m_Moves.back()[0];
        movey = -m_Moves.back()[1];
        
        GetVavView()->OnTimer(0);
        GetVavView()->SetTimer(30, 50, 0);
        Beep(750, 300);
        Beep(1750, 300);
        Beep(10750, 300);
    }
}


void VAV_MainFrame::OnButton_Curvematching()
{
    CFileDialog dlg(TRUE);
    dlg.m_ofn.lpstrFilter   = L"All Files (*.*)\0*.*\0\0";
    dlg.m_ofn.lpstrTitle    = L"Load File";
    CString filepath;
    CString filename;
    CString filefolder;
    if(dlg.DoModal() == IDOK)
    {
        filepath = dlg.GetPathName(); // return full path and filename
        filefolder = dlg.GetFolderPath();
        filename = dlg.GetFileName();
        if(filepath.GetLength() > 1)
        {
            std::string path = ConvStr::GetStr(filepath.GetString());
            std::string folder = ConvStr::GetStr(filefolder.GetString());
            std::string name = ConvStr::GetStr(filename.GetString());
            name.resize(name.size() - 4);
            folder += "\\tmp";
            _mkdir(folder.c_str());
            folder += "\\";
            m_Video.clear();
            m_Moves.clear();
            m_ReadVideo.Read(path);
            m_ReadVideo.GetFrame();
            m_ReadVideo.GetFrame();
            cv::Mat frame1 = m_ReadVideo.GetFrame();
            cv::Mat frame2 = m_ReadVideo.GetFrame();
            cv::Mat img1 = frame1.clone();
            cv::Mat img2 = frame2.clone();
            cv::Mat oimg1 = frame1.clone();
            cv::Mat oimg2 = frame2.clone();
            cv::Mat gimg1 = frame1.clone();
            cv::Mat gimg2 = frame2.clone();
            img1 = cv::Scalar(0);
            img2 = cv::Scalar(0);
            cvtColor(gimg1, gimg1, CV_BGR2GRAY);
            cvtColor(gimg2, gimg2, CV_BGR2GRAY);
            cv::Vec2f move2 = getMoveVectorBySIFT(gimg1, gimg2);
            FrameInfo fi1 = ComputeFrame2(frame1);
            FrameInfo fi2 = ComputeFrame2(frame2);
            CurveMatching cm = ComputeMatching(fi1, fi2, move2);
            for(int i = 0; i < cm.curve1.size(); ++i)
            {
                cv::Scalar color(rand() % 256, rand() % 256, rand() % 256);
                int id1 = cm.curve1[i];
                int id2 = cm.curve2[i];
                Line& line1 = fi1.curves1[id1];
                for(int j = 0; j < line1.size() - 1; ++j)
                {
                    cv::line(img1, cv::Point(line1[j].x, line1[j].y), cv::Point(line1[j + 1].x, line1[j + 1].y), color);
                    cv::line(frame1, cv::Point(line1[j].x, line1[j].y), cv::Point(line1[j + 1].x, line1[j + 1].y), color);
                }
                Line& line2 = fi2.curves1[id2];
                for(int j = 0; j < line2.size() - 1; ++j)
                {
                    cv::line(img2, cv::Point(line2[j].x, line2[j].y), cv::Point(line2[j + 1].x, line2[j + 1].y), color);
                    cv::line(frame2, cv::Point(line2[j].x, line2[j].y), cv::Point(line2[j + 1].x, line2[j + 1].y), color);
                }
            }
            for(int i = 0; i < fi1.curves1.size(); ++i)
            {
                cv::Scalar color(rand() % 256, rand() % 256, rand() % 256);
                Line& line1 = fi1.curves1[i];
                if(line1.size() > 2)
                {
                    for(int j = 0; j < line1.size() - 1; ++j)
                    {
                        cv::line(oimg1, cv::Point(line1[j].x, line1[j].y), cv::Point(line1[j + 1].x, line1[j + 1].y), color);
                    }
                }
            }
            for(int i = 0; i < fi2.curves1.size(); ++i)
            {
                cv::Scalar color(rand() % 256, rand() % 256, rand() % 256);
                Line& line2 = fi2.curves1[i];
                if(line2.size() > 2)
                {
                    for(int j = 0; j < line2.size() - 1; ++j)
                    {
                        cv::line(oimg2, cv::Point(line2[j].x, line2[j].y), cv::Point(line2[j + 1].x, line2[j + 1].y), color);
                    }
                }
            }
            cv::imshow("oimg1", oimg1);
            cv::imshow("oimg2", oimg2);
            cv::imshow("img1", img1);
            cv::imshow("img2", img2);
            cv::imshow("frame1", frame1);
            cv::imshow("frame2", frame2);
        }
    }
}

