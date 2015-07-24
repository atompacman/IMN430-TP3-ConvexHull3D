#ifndef __ConflictGraph__
#define __ConflictGraph__

#include <list>
#include <vector>

#include "Point.h"

typedef unsigned int uint;

struct FacetNode {
    Point*          m_Pts;
    uint            m_NbPoints;
    std::list<uint> m_Conflicts;
};

class ConflictGraph
{
public:

    ConflictGraph(const std::vector<Point>& i_Pts,
                  const int*                i_Index,
                  uint                      i_P1,
                  uint                      i_P2,
                  uint                      i_P3,
                  uint                      i_P4);

private:

    std::list<FacetNode*>* m_Conflicts;
};

#endif