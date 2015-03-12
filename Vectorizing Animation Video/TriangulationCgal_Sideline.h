#pragma once
#include "TriangulationBase.h"
#pragma warning( push, 0 )
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/Color.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Triangulation_conformer_2.h>
#include <CGAL/Polygon_2.h>
#pragma warning( pop )

#include "Patch.h"
#include "ColorConstraint.h"
#include "LineDef.h"
#include "vavImage.h"
#include "RegionDiffusion.h"


class TriangulationCgal_Sideline : public TriangulationBase
{
public:
    struct FaceInfo2
    {
        FaceInfo2(): nesting_level(-1), color_label(0) {}
        int nesting_level;
        int color_label;
        bool in_domain()
        {
            return nesting_level >= 0;
        }
        bool in_domain2()
        {
            return nesting_level % 2 == 1;
        }
    };
    
    struct VertexInfo2
    {
        VertexInfo2(): nesting_level(-1) {}
		int nesting_level;
		int idx;
    };
    typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
    //typedef CGAL::Triangulation_vertex_base_2<K>                    Vb;
    typedef CGAL::Triangulation_vertex_base_with_info_2<VertexInfo2, K> Vb;
    typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2, K> Fbbb;
    typedef CGAL::Constrained_triangulation_face_base_2<K, Fbbb>    Fbb;
    typedef CGAL::Delaunay_mesh_face_base_2<K, Fbb>         Fb;
    typedef CGAL::Triangulation_data_structure_2<Vb, Fb>            TDS;
    typedef CGAL::Exact_predicates_tag                              Itag;
    typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag> CTriangulation;
	typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag> Triangulation;
    typedef CGAL::Delaunay_mesh_size_criteria_2<CTriangulation>  Criteria;
    typedef CGAL::Delaunay_mesher_2<CTriangulation, Criteria>    Mesher;
    typedef CGAL::Polygon_2<K>                  Polygon;
    
    typedef CTriangulation::Face_handle          Face_handle;
    typedef CTriangulation::Finite_faces_iterator        Finite_faces_iterator;
	typedef CTriangulation::Finite_edges_iterator        Finite_edges_iterator;
    typedef CTriangulation::Finite_vertices_iterator     Finite_vertices_iterator;
    typedef CTriangulation::Vertex_handle            Vertex_handle;
    typedef CTriangulation::Point                Point;
    
    typedef std::vector<Point>      Points;
    typedef std::vector<Vertex_handle>  Vertex_handles;
    
    void    AddPatch(Patch& cvps);
    void    AddLines(Lines& is);
    TriangulationCgal_Sideline(): m_Criteria(0.125, 5) {}
    virtual ~TriangulationCgal_Sideline() {}
    virtual void AddPoint(double x, double y);
    virtual int Compute();
	void	MakeColorByModel();
    virtual void Clear();
    void    MakeColorSequential();
    void    MakeColorRandom();
    void    SetSize(int w, int h);
    void    SetCriteria(float shapebound, float length);
    void    insert_polygon(CTriangulation& cdt, const Polygon& polygon, int domain);
    
    void mark_domains(CTriangulation& ct, CTriangulation::Face_handle start,
                      int index, std::list<CTriangulation::Edge>& border);
    void mark_domains(CTriangulation& cdt);
    void mark_domains2(CTriangulation& ct, CTriangulation::Face_handle start,
                       int index, std::list<CTriangulation::Edge>& border, int color_label);
    int mark_domains2(CTriangulation& cdt);
    void SetColor(ColorConstraints& colors);
    void RemoveShadingLine(vavImage& img);
    void Insert_lines(CTriangulation& cdt);
    Triangles2d GetAllRegion();
    static const int TRIANGLE_NOT_INIT = -1;
    static const int TRIANGLE_TRANSPARENT = -2;
    
    int     m_RegionCount;
    CTriangulation   m_Triangulation;
    Criteria    m_Criteria;
    Points      m_SeedPoints;
    Patchs      m_Patch;
    Polygon     m_Polygon;
    int     m_w, m_h;
    ColorConstraints   m_ColorConstraint;
    Lines       m_Lines;
    double_vector2d m_LinesWidth;
	Index2Side	m_i2s;
};


