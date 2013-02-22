#include "Edge.h"

Line CEdge::GetLine() const 
{
	Line res;

	for ( CvPoints::const_iterator it = pnts.begin(); it != pnts.end(); ++it )
	{
		res.push_back( Vector2( it->x, it->y ) );
	}
	return res;
}

Lines GetLines( const CEdges& edges )
{
	Lines res;

	for ( CEdges::const_iterator it = edges.begin(); it != edges.end(); ++it )
	{
		res.push_back( it->GetLine() );
	}
	return res;
}

Lines GetLine( CvPoints2d& cvline )
{
	Lines res;
	for (auto it = cvline.begin();it!= cvline.end();++it)
	{
		Line li;
		for (auto it2 = it->begin();it2 != it->end();++it2)
		{
			li.push_back(Vector2(it2->x, it2->y));
		}
		res.push_back(li);
	}
	return res;
}
