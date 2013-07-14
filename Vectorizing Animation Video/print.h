#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>

template<class CgalInexactKernel>
void print_point(CGAL::Point_2<CgalInexactKernel> const& p)
{
	std::cout << "(" << p.x() << "," << p.y() << ")" ;
}

template<class CgalInexactKernel>
void print_polygon(CGAL::Polygon_2<CgalInexactKernel> const& poly)
{
	typedef CGAL::Polygon_2<CgalInexactKernel> Polygon ;
	std::cout << "Polygon with " << poly.size() << " vertices" << std::endl ;

	for (typename Polygon::Vertex_const_iterator vi = poly.vertices_begin() ;
	        vi != poly.vertices_end() ; ++ vi)
	{
		print_point(*vi); std::cout << std::endl ;
	}
}

template<class CgalInexactKernel>
void print_polygons(std::vector< boost::shared_ptr< CGAL::Polygon_2<CgalInexactKernel> > >
                    const& polies)
{
	typedef std::vector< boost::shared_ptr< CGAL::Polygon_2<CgalInexactKernel> > > PolygonVector ;
	std::cout << "Polygon list with " << polies.size() << " polygons" << std::endl ;

	for (typename PolygonVector::const_iterator pi = polies.begin() ;
	        pi != polies.end() ; ++ pi)
	{
		print_polygon(**pi);
	}
}

template<class CgalInexactKernel>
void print_polygon_with_holes(CGAL::Polygon_with_holes_2<CgalInexactKernel> const& polywh)
{
	typedef CGAL::Polygon_with_holes_2<CgalInexactKernel> PolygonWithHoles ;
	std::cout << "Polygon_with_holes having " << polywh.number_of_holes() <<
	          " holes" << std::endl ;
	print_polygon(polywh.outer_boundary());

	for (typename PolygonWithHoles::Hole_const_iterator hi = polywh.holes_begin() ;
	        hi != polywh.holes_end() ; ++ hi)
	{
		print_polygon(*hi);
	}
}

template<class CgalInexactKernel>
void print_polygons_with_holes(
    std::vector< boost::shared_ptr< CGAL::Polygon_with_holes_2<CgalInexactKernel> > > const& polies)
{
	typedef std::vector< boost::shared_ptr< CGAL::Polygon_with_holes_2<CgalInexactKernel> > >
	PolygonWithHolesVector ;
	std::cout << "Polygon_with_holes list with " << polies.size() << " element" <<
	          std::endl ;

	for (typename PolygonWithHolesVector::const_iterator pi = polies.begin() ;
	        pi != polies.end() ; ++ pi)
	{
		print_polygon_with_holes(**pi);
	}
}

template<class CgalInexactKernel>
void print_straight_skeleton(CGAL::Straight_skeleton_2<CgalInexactKernel> const& ss)
{
	typedef CGAL::Straight_skeleton_2<CgalInexactKernel> Ss ;
	typedef typename Ss::Vertex_const_handle     Vertex_const_handle ;
	typedef typename Ss::Halfedge_const_handle   Halfedge_const_handle ;
	typedef typename Ss::Halfedge_const_iterator Halfedge_const_iterator ;
	Halfedge_const_handle null_halfedge ;
	Vertex_const_handle   null_vertex ;
	std::cout << "Straight skeleton with " << ss.size_of_vertices()
	          << " vertices, " << ss.size_of_halfedges()
	          << " halfedges and " << ss.size_of_faces()
	          << " faces" << std::endl ;

	for (Halfedge_const_iterator i = ss.halfedges_begin(); i != ss.halfedges_end();
	        ++i)
	{
		print_point(i->opposite()->vertex()->point()) ;
		std::cout << "->" ;
		print_point(i->vertex()->point());
		std::cout << " " << (i->is_bisector() ? "bisector" : "contour") << std::endl;
	}
}
