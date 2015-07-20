#ifndef __ConflictGraph__
#define __ConflictGraph__

#include <list>
#include <vector>
#include "Point.h"

typedef unsigned int uint;

struct FacetNode {
	Point*			m_Pts;
	uint			m_NbPoints;
	std::list<uint> m_Conflicts;
};

class ConflictGraph
{
public:
	
	ConflictGraph(const std::vector<Point>& i_Pts, 
				  const int*				i_Index, 
				  uint						i_P1,
				  uint						i_P2,
				  uint						i_P3,
				  uint						i_P4);

private:

	std::list<FacetNode*>* m_Conflicts;
};

bool isVisible(const Point& i_Pt, const FacetNode& i_Facet)
{
	Vector normal(cross(i_Facet.m_Pts[1] - i_Facet.m_Pts[0], 
						i_Facet.m_Pts[2] - i_Facet.m_Pts[0]));
	return normal.m_x * i_Pt.m_x + 
		   normal.m_y * i_Pt.m_y + 
		   normal.m_z * i_Pt.m_z   >
									  normal.m_x * i_Facet.m_Pts[0].m_x + 
									  normal.m_y * i_Facet.m_Pts[0].m_y + 
									  normal.m_z * i_Facet.m_Pts[0].m_z;
}

#endif