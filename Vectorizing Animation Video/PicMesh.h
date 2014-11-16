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
		// for interpolation color
		Vector3 c;
		// for mapping id
		int mapid;
		int left;
		int right;
		VertexT(): left(-1), right(-1) { }
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
	void MappingMesh(PicMesh& pm, double x, double y);
	// out1 is this mesh color, out2 is input mapping color
	void GetMappingCT(PicMesh& pm, ColorTriangles& out1, ColorTriangles& out2);
	ColorTriangles Interpolation(ColorTriangles& c1, ColorTriangles& c2, double alpha);
	Lines m_Lines;
	VHandles2d	m_LinesVH;
	HHandles2d	m_LinesHH;
	FHandles2d	m_Regions;
	Index2Side	m_i2s;
	ColorTriangles	m_Trangles;
	ColorConstraints m_ColorConstraint;
};

