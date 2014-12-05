#pragma once
#include "LineDef.h"
#include "LineWidthConstraint.h"
#include "TriangulationBase.h"
#include "PicMesh.h"

struct FrameInfo
{
	// decorative
	Lines curves1;
	Vector3s2d ocolor1;
	ColorConstraints color1;
	LineWidthConstraints lineWidth;
	Lines tmplinewidth;
	// diffusion
	Color2Side ocolor2;
	Color2SideConstraint color2;
	Lines curves2;
	// picmesh
	PicMesh picmesh1;
	PicMesh picmesh2;
	// isoline
	Lines curves3;
	Vector3s2d ocolor3;
	ColorConstraints color3;
	//ColorTriangles	ctris;
	Triangles2d		tris2d;
	Lines GetLine1Width();
	Vector3s2d GetLine1Color();
	Color2Side GetLine2Color();
	Vector3s2d GetLine3Color();
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& curves1;
		ar& curves2;
		ar& lineWidth;
		ar& color1;
		ar& color2;
	}
};
typedef std::vector<FrameInfo> FrameInfos;

typedef std::vector<cv::Vec2f> Vec2fs;
struct BackGround
{
	FrameInfo m_FI;
	Vec2fs  m_Moves;
	cv::Mat m_FG;
private:
	friend class boost::serialization::access;
	template<class Archive>
	void save(Archive& ar, const unsigned int version) const
	{
		ar& m_FI;
		int s1 = m_Moves.size();
		ar& s1;
		for(int i = 0; i < m_Moves.size(); ++i)
		{
			ar& m_Moves[i][0];
			ar& m_Moves[i][1];
		}
		int w = m_FG.cols;
		int h = m_FG.rows;
	}
	template<class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		ar& m_FI;
		int s1;
		ar& s1;
		m_Moves.resize(s1);
		for(int i = 0; i < m_Moves.size(); ++i)
		{
			ar& m_Moves[i][0];
			ar& m_Moves[i][1];
		}
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
};

struct ColorRegion
{
	ColorConstraints ccms;
	cv::Mat markimg;
};

void SaveBGInfos(std::string path, BackGround& frms);
BackGround LoadBGInfos(std::string path);
void SaveFrameInfos(std::string path, FrameInfos& frms);
FrameInfos LoadFrameInfos(std::string path);
FrameInfo ComputeFrame1(cv::Mat img, ColorRegion* cr = NULL);
FrameInfo ComputeFrame1FG(cv::Mat img, cv::Mat fg, ColorRegion* cr);
FrameInfo ComputeFrame2(cv::Mat img, ColorRegion* cr = NULL);
FrameInfo ComputeFrame2FG(cv::Mat img, cv::Mat fg, ColorRegion* cr);
FrameInfo ComputeFrame3(cv::Mat img, ColorRegion* cr = NULL);
FrameInfo ComputeCurve(cv::Mat img);
void RemoveBGs_FG_Part(FrameInfo& fi, cv::Mat fg);
void RemoveFGs_BG_Part(FrameInfo& fi, cv::Mat fg);
FrameInfo ComputeFrame09(cv::Mat img, ColorRegion* cr = NULL);
FrameInfo ComputeFrameFG09(cv::Mat img, cv::Mat fg, ColorRegion* cr);
bool drawmask5x5(cv::Mat img, int x, int y, cv::Vec3b color, cv::Mat dst);
bool drawmask3x3(cv::Mat img, int x, int y, cv::Vec3b color, cv::Mat dst);
bool checkmask3x3(cv::Mat img, int x, int y);
FrameInfo ComputeFrameFGIsoline(cv::Mat img);
