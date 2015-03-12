#pragma once
#include "BasicMesh.h"

#include "ColorConstraint.h"
#include "TriangulationBase.h"
#include "vavImage.h"

class TriangulationCgal_Sideline;
class PicMesh : public BasicMesh
{
public:
	PicMesh(void);
	~PicMesh(void);
	// get middle point of face
	BasicMesh::Point MidPoint(FHandle fh);

	void ReadFromSideline(TriangulationCgal_Sideline* data);
	// draw black color for no color
	void MakeColor1();
	// don't draw for no color
	void MakeColor2();
	// add region line by triangle information
	// lmax is max distance of connection line
	void MakeRegionLine(cv::Mat& img, double lmax);
	// mapping next mesh
	void MappingMesh(PicMesh& pm, double x, double y);
	// mapping next mesh and if color distance so big dont mapping
	void MappingMeshByColor(PicMesh& pm, double x, double y);
	// mapping next mesh by midpoint
	void MappingMeshByMidPoint(PicMesh& pm, double x, double y);
	// mark different region
	void MarkDifferentRegion2(PicMesh& pm, double v, double x, double y);
	// mark different region
	void MarkDifferentRegion1(vavImage& img, double v, double x, double y);
	// out1 is this mesh color, out2 is input mapping color
	void GetMappingCT(PicMesh& pm, ColorTriangles& out1, ColorTriangles& out2, double x, double y);
	// interpolation c1 to c2
	ColorTriangles Interpolation(ColorTriangles& c1, ColorTriangles& c2, double alpha);
	// Connect one ring edge, return true if success
	bool ConnectOneRingConstraint1(vavImage& img, VHandle vh, VHandle lastvh, Vector2& out, Point dir, double lmax);
	// Connect one ring edge, return true if success
	bool ConnectOneRingConstraint2(vavImage& img, VHandle vh, VHandle lastvh, Vector2& out, Point dir, double lmax);
	// check vertex connection
	bool isConnection(VHandle vh1, VHandle vh2);
	// set w h
	void SetSize(int w, int h);
	// draw color for color model
	void MakeColor3();
	// draw color form image color
	void MakeColor4(cv::Mat& img);
	// draw mark region form image color
	void MakeColor5(cv::Mat& img);
	// modify MakeColor4
	void MakeColor6(cv::Mat& img);

	// make same region same color
	void MakeColor7(Vector3s& colors, ints& lastmap);

	void MakeColor72(ColorConstraints& colors, ints& lastmap, double x, double y);

	// make same face same color
	void MakeColor8(PicMesh& pm);

	// draw color use c2
	void MakeColor9();

	void FillConstraintRegion();

	void MarkColoredFace();

	void GetConstraintFaceColor(cv::Mat& img);

	// Dilation face color
	void Dilation();

	// use picture build color model
	void BuildColorModels(cv::Mat& img);
	// use mesh color build color model
	void BuildColorModels();
	// get lighter color
	bool GetLighterColor(cv::Mat& img, VHandle vh1, VHandle vh2, Vector3& c);
	// get gradient of line 
	double GetLineGradient(vavImage& img, VHandle vh1, VHandle vh2, double len);
	// internel function
	void blurC2();
	// light blur
	void lightblurC2();
	// remove black
	void removeblackC2();
	// blurfacec2
	void blurFaceC2();
	void blurFaceC2x();
	// save w h
	int m_w, m_h;
	Lines m_Lines;
	VHandles2d	m_LinesVH;
	HHandles2d	m_LinesHH;
	FHandles2d	m_Regions;
	ints		m_DifferentRegionIDs;
	// for next region
	ints		m_MapingRegionIDs;
	Index2Side	m_i2s;
	ColorTriangles	m_Trangles;
	ColorConstraints m_ColorConstraint;
};

