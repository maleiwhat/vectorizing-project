#pragma once

#include <opencv2\core\core.hpp>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include <vector>
#include "RBFMesh.h"


class ColorConstraintMyRBF
{
public:
    ColorConstraintMyRBF();
	~ColorConstraintMyRBF();
    void AddPoint(double x, double y, const cv::Vec3b& p);
    void AddPoint(double x, double y, const Vector3& p);
    void Clear();
    Vector3 GetColorVector3();
    Vector3 GetColorVector3(double x, double y);
	Vector3 GetColorVector3(double x1, double y1, double x2, double y2);
    Vector3 GetColorVector3Reverse(double x, double y);
    cv::Vec3b GetColorCvPoint();
    cv::Vec3b GetColorCvPoint(double x, double y);
	void blurC2();
    int Size();
    void BuildModel();
private:
    void RemoveNoise();
    Vector3s m_Colors;
    Vector3s m_ColorsOri;
    Vector2s m_Pos;
    Vector3  m_Median;
	RBFMesh  m_rbfmesh;
    bool m_NeedComputeModel;
private:
    friend class boost::serialization::access;
    template<class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar& m_Colors;
        ar& m_ColorsOri;
        ar& m_Pos;
        ar& m_Median;
        ar& m_NeedComputeModel;
    }
    template<class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        ar& m_Colors;
        ar& m_ColorsOri;
        ar& m_Pos;
        ar& m_Median;
        ar& m_NeedComputeModel;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};
typedef std::vector<ColorConstraintMyRBF> ColorConstraintMyRBFs;
