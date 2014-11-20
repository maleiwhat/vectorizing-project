#pragma once
#undef max
#undef min
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Utils/getopt.h>
#include "LineDef.h"
#include "ColorConstraint.h"
#include "TriangulationBase.h"


struct BasicTraits : public OpenMesh::DefaultTraits
{
	typedef OpenMesh::Vec3d Point;
	FaceTraits
	{
	public:
		// for mapping id
		int mapid;
		// colorid
		int cid;
		// region id
		int rid;
		FaceT() : cid(-1), rid(-1) { }
	};
	EdgeTraits
	{
	public:
		int idx;
		EdgeT(): idx(-1) { }
	};
	VertexTraits
	{
	public:
		// for constraint line index
		int lineidx;
		// for constraint line id
		int lineid;
		// for check is constraint point
		char constraint;
		// for check end point
		bool end;
		// for interpolation color
		Vector3 c;
		// for mapping id
		int mapid;
		VertexT(): end(false), constraint(0), mapid(0), lineid(-1) { }
	};
};

typedef OpenMesh::TriMesh_ArrayKernelT<BasicTraits> BasicMesh;

typedef std::vector<BasicMesh::FHandle> FHandles;
typedef std::vector<FHandles> FHandles2d;
typedef std::vector<BasicMesh::EHandle> EHandles;
typedef std::vector<EHandles> EHandles2d;
typedef std::vector<BasicMesh::VHandle> VHandles;
typedef std::vector<VHandles> VHandles2d;
typedef std::vector<BasicMesh::HHandle> HHandles;
typedef std::vector<HHandles> HHandles2d;
typedef std::vector<BasicMesh::HHandle*> HHandlePs;
typedef std::vector<BasicMesh::VHandle*> VHandlePs;
typedef std::vector<BasicMesh::FHandle*> FHandlePs;

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
	void MakeRegionLine(double lmax);
	// mapping next mesh
	void MappingMesh(PicMesh& pm, double x, double y);
	// out1 is this mesh color, out2 is input mapping color
	void GetMappingCT(PicMesh& pm, ColorTriangles& out1, ColorTriangles& out2);
	// interpolation c1 to c2
	ColorTriangles Interpolation(ColorTriangles& c1, ColorTriangles& c2, double alpha);
	// Connect one ring edge, return true if success
	bool ConnectOneRingConstraint(VHandle vh, VHandle lastvh, Vector2& out, Point dir, double lmax);
	// check vertex connection
	bool isConnection(VHandle vh1, VHandle vh2);
	// set w h
	void SetSize(int w, int h);
	// save w h
	int m_w, m_h;
	Lines m_Lines;
	VHandles2d	m_LinesVH;
	HHandles2d	m_LinesHH;
	FHandles2d	m_Regions;
	Index2Side	m_i2s;
	ColorTriangles	m_Trangles;
	ColorConstraints m_ColorConstraint;
};

