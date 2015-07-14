
#include "ColorConstraintMyRBF.h"
#include <opencv2\core\core.hpp>
#include "TriangulationCgal_SeedPoint.h"

void ColorConstraintMyRBF::AddPoint(double x, double y, const cv::Vec3b& p)
{
    m_NeedComputeModel = true;
    m_Pos.push_back(Vector2(x, y));
    m_Colors.push_back(Vector3(p[0], p[1], p[2]));
}

void ColorConstraintMyRBF::AddPoint(double x, double y, const Vector3& p)
{
    m_NeedComputeModel = true;
    m_Pos.push_back(Vector2(x, y));
    m_Colors.push_back(p);
}

void ColorConstraintMyRBF::Clear()
{
    m_NeedComputeModel = true;
    m_Colors.clear();
    m_ColorsOri.clear();
    m_Pos.clear();
}

Vector3 ColorConstraintMyRBF::GetColorVector3()
{
    if(m_NeedComputeModel)
    {
        BuildModel();
    }
    return m_Median;
}

Vector3 ColorConstraintMyRBF::GetColorVector3(double x, double y)
{
    if(m_NeedComputeModel)
    {
        BuildModel();
    }
    if(m_Colors.size() < 10)
    {
        return m_Median;
    }
    return m_rbfmesh.GetColorVector3(x, y);
}

Vector3 ColorConstraintMyRBF::GetColorVector3(double x1, double y1, double x2, double y2)
{
	if(m_NeedComputeModel)
	{
		BuildModel();
	}
	if(m_Colors.size() < 10)
	{
		return m_Median;
	}
	return m_rbfmesh.GetColorVector3(x1, y1, x2, y2);
}

Vector3 ColorConstraintMyRBF::GetColorVector3Reverse(double x, double y)
{
    if(m_NeedComputeModel)
    {
        BuildModel();
    }
    if(m_Colors.size() >= 10)
    {
        m_Median = m_rbfmesh.GetColorVector3(x, y);
    }
    Vector3 tmp(m_Median[2], m_Median[1], m_Median[0]);
    return tmp;
}

cv::Vec3b ColorConstraintMyRBF::GetColorCvPoint()
{
    if(m_NeedComputeModel)
    {
        BuildModel();
    }
    cv::Vec3b res;
    res[0] = m_Median[0];
    res[1] = m_Median[1];
    res[2] = m_Median[2];
    return res;
}

cv::Vec3b ColorConstraintMyRBF::GetColorCvPoint(double x, double y)
{
    if(m_NeedComputeModel)
    {
        BuildModel();
    }
    cv::Vec3b res;
    res[0] = m_Median[0];
    res[1] = m_Median[1];
    res[2] = m_Median[2];
    return res;
}

ColorConstraintMyRBF::ColorConstraintMyRBF(): m_NeedComputeModel(true),
    m_Median(0, 0, 0)
{
}

void ColorConstraintMyRBF::BuildModel()
{
    m_NeedComputeModel = false;
	if (m_Colors.empty())
	{
		return;
	}
    if(m_Colors.size() < 10)
    {
        Vector3 sum;
        for(int i = 0; i < m_Colors.size(); i++)
        {
            sum += m_Colors[i];
        }
        m_Median = sum / m_Colors.size();
    }
    else
    {
//         cv::Mat model(m_Colors.size(), 5, CV_32F);
//         for(int i = 0; i < m_Colors.size(); ++i)
//         {
//             Vector3& v = m_Colors[i];
//             Vector2& p = m_Pos[i];
//             model.at<float>(i, 0) = v[0] * 10;
//             model.at<float>(i, 1) = v[1] * 10;
//             model.at<float>(i, 2) = v[2] * 10;
//             model.at<float>(i, 3) = p[0];
//             model.at<float>(i, 4) = p[1];
//         }
//         const int K = 5; //要分幾群
//         cv::Mat cluster; //會跑出結果，紀錄每個row 最後是分配到哪一個cluster
//         int attempts = 3; //應該是執行次數
//         cv::Mat centers; //記錄那個cluster的值
// 
//         //使用k means分群
//         cv::kmeans(model, K, cluster, cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 1),
//                    attempts, cv::KMEANS_PP_CENTERS, centers);
//         int countMAX = 0;
//         int MAXIndex;
//         int tempSum[K] = {0};
//         for(int i = 0; i < m_Colors.size(); i++)
//         {
//             tempSum[cluster.at<int>(i, 0)]++;
//             if(tempSum[cluster.at<int>(i, 0)] > countMAX)
//             {
//                 countMAX = tempSum[cluster.at<int>(i, 0)];
//                 MAXIndex = cluster.at<int>(i, 0);
//             }
//         }
//         // flag check this cluster color can add
//         bool canadd[K] = {0};
//         canadd[MAXIndex] = true;
//         // RemoveNoise
//         for(int i = 0; i < K; ++i)
//         {
//             if(MAXIndex != i)
//             {
//                 if(tempSum[MAXIndex] <= tempSum[i] * 3)
//                 {
//                     canadd[i] = true;
//                 }
//             }
//         }
        TriangulationCgal_SeedPoint triangulation;
        for(int i = 0; i < m_Colors.size(); ++i)
        {
            //if(canadd[cluster.at<int>(i, 0)])
            {
                Vector2& p = m_Pos[i];
                triangulation.AddPointIdx(p[0], p[1], i);
            }
        }
        m_rbfmesh.clear();
        m_rbfmesh.ReadFromSeedpoint(&triangulation, m_Colors);
// 		m_rbfmesh.blurC2();
// 		m_rbfmesh.lightblurC2();
// 		m_rbfmesh.lightblurC2();
    }
}

int ColorConstraintMyRBF::Size()
{
    return m_Colors.size();
}

void ColorConstraintMyRBF::RemoveNoise()
{

}

ColorConstraintMyRBF::~ColorConstraintMyRBF()
{
}

void ColorConstraintMyRBF::blurC2()
{
	m_rbfmesh.blurC2();
}
