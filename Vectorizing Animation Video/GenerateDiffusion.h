#pragma once
#include "LineDef.h"
#include "LineWidthConstraint.h"
#include <opencv2\core\core.hpp>
#include "DX11\d3dApp.h"

struct FrameInfo
{
	// decorative
	Lines curves1;
	ColorConstraints color1;
	LineWidthConstraints lineWidth;
	// diffusion
	Color2SideConstraint color2;
	Lines curves2;
	Lines GetLineWidth();
	Vector3s2d GetLineColor();
	Color2Side GetBoundaryColor();
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& curves1;
		ar& color1;
		ar& lineWidth;
		ar& color2;
		ar& curves2;
	}
};
typedef std::vector<FrameInfo> FrameInfos;

typedef std::vector<cv::Vec2f> Vec2fs;
struct BackGround
{
	FrameInfo m_FI;
	Vec2fs  m_Moves;
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

void SaveBGInfos(std::string path, BackGround& frms);
FrameInfos LoadBGInfos(std::string path);
void SaveFrameInfos(std::string path, BackGround& frms);
FrameInfos LoadFrameInfos(std::string path);
FrameInfo ComputeFrame(cv::Mat img);
void SetDrawFrame(D3DApp& d3dApp, FrameInfo& fi);

