#pragma once
#include "BasicMesh.h"

#include "ColorConstraint.h"
#include "TriangulationBase.h"
#include "vavImage.h"
#include "btree.h"
#include "math\AABB2D.h"


struct sortu
{
	int oid;
	float size;
	bool operator<(const sortu& l)
	{
		return size < l.size;
	}
};

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
	void MakeSeedPointMap(cv::Mat& out1, cv::Mat& out2);
	// mapping next mesh
	void MappingMesh(PicMesh& pm, double x, double y);
	// mapping next mesh and if color distance so big dont mapping
	void MappingMeshByColor(PicMesh& pm, double x, double y, cv::Mat& img1, cv::Mat& bg, cv::Mat& fg);
	// mapping next mesh by midpoint
	void MappingMeshByMidPoint(PicMesh& pm, double x, double y);
	// mark different region
	void MarkDifferentRegion2(PicMesh& pm, double v, double x, double y);
	// mark different regionfv 
	void MarkDifferentRegion1(vavImage& img, double v, double x, double y);
	// out1 is this mesh color, out2 is input mapping color
	void GetMappingCT(PicMesh& pm, ColorTriangles& out1, ColorTriangles& out2, double x, double y);
	// interpolation c1 to c2
	ColorTriangles Interpolation(ColorTriangles& c1, ColorTriangles& c2, double alpha);
	// Connect one ring edge, return true if success
	bool ConnectOneRingConstraint1(VHandle vh, VHandle lastvh, Vector2& out, Point dir, double lmax);
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

	void MakeColor6x(cv::Mat& img);

	void MakeFGLine(float x, float y, cv::Mat fg);

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
	// each region triangles amount to m_RegionAreas
	void ComputeRegion();
	// each region region's Neighbor
	void ComputeNeighbor();

	void BuildBtree();

	void DrawTree();

	int GetLeaveSize(int idx);

	void SetRegionColor(cv::Mat& img);

	void makeRank(std::vector<sortu> &tary, chars &used);

	char bindnode(chars& used, int idx);
	// draw color use tree
	void MakeColorX1(Vector3s& colors);

	void MakeColorX2(ints& mappings, float x, float y);

	void MakeColorX9(int id);

	void MakeTreeLeaveColor(int idx, Vector3 colors);
	void MarkTreeLeave(int idx);

	void DrawLeave(int idx, cv::Mat& img, int x, int y);

	int GetRegionId(float x, float y);
	// save w h
	int m_w, m_h;


	Lines m_Lines;
	VHandles2d	m_LinesVH;
	HHandles2d	m_LinesHH;
	ints2d		m_LinesHH_Rrid;
	ints2d		m_LinesHH_Lrid;
	FHandles2d	m_Regions;
	ints		m_DifferentRegionIDs;
	ints		m_FGids;
	ints		m_RegionSize;
	// for next region
	ints		m_MapingRegionIDs;
	ints		m_MapingCount;
	ints2d		m_RegionNeighbor;
	Vector3s	m_RegionColor;
	AABB2Ds		m_RegionAABB;
	btree		m_btree;
	doubles		m_RegionAreas;
	ints		m_RegionRank;
	chars		m_used;
	Index2Side	m_i2s;
	ColorTriangles	m_Trangles;
	ColorConstraints m_ColorConstraint;

	Lines fglines;
	Lines fglinesW;
	Vector3s2d fglineColors;
};

