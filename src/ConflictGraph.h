#ifndef __ConflictGraph__
#define __ConflictGraph__

#include <list>
#include <vector>

#include "DCEL3D.h"
#include "Point.h"

struct FacetNode {

    sptr<Facet>     m_Facet;
    std::list<uint> m_Conflicts;

    FacetNode(sptr<Facet> i_Facet) :
        m_Facet(i_Facet),
        m_Conflicts(){}
};

struct ConflictGraph
{
    std::list<sptr<FacetNode>>* m_Conflicts;

    ConflictGraph(const std::vector<sptr<const Point>>& i_Pts, 
                  const int*                            i_Index, 
                  sptr<DCEL3D>                          i_DCEL) :

        m_Conflicts(new std::list<sptr<FacetNode>>[i_Pts.size()])
    {
    // For each facet
    for (const sptr<Facet>& facet : i_DCEL->m_Facets) {
        // Create a node in the conflict graph
        sptr<FacetNode> facetNode(new FacetNode(facet));

        // For each point
        for (uint i = 0; i < i_Pts.size() - 4; ++i) {
            // If facet is visible from the point
            if (facet->isVisibleFor(i_Pts[i])) {
                facetNode->m_Conflicts.push_back(i);
                m_Conflicts[i].push_back(facetNode);
            }
        }
    }
};

#endif