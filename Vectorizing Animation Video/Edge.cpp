#include "Edge.h"

Line CEdge::GetLine() const
{
	Line res;

	for (CvLine::const_iterator it = pnts.begin(); it != pnts.end(); ++it)
	{
		res.push_back(Vector2(it->x, it->y));
	}

	return res;
}

Lines GetLines(const CEdges& edges)
{
	Lines res;

	for (CEdges::const_iterator it = edges.begin(); it != edges.end(); ++it)
	{
		res.push_back(it->GetLine());
	}

	return res;
}
