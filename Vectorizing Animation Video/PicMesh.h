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
	void ReadFromSideline(TriangulationCgal_Sideline* data);
	Lines m_Lines;
	VHandles2d	m_LinesVH;
	HHandles2d	m_LinesHH;
	FHandles2d	m_Regions;
	Index2Side	m_i2s;
	ColorTriangles	m_Trangles;
	ColorConstraints m_ColorConstraint;
};

