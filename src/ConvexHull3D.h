#ifndef __ConvexHull3D__
#define __ConvexHull3D__

#include <algorithm>
#include <ctime>
#include <list>
#include <random>
#include <set>
#include <vector>

#include "ConflictGraph.h"
#include "DCEL3D.h"
#include "Point.h"

#define EPSILON 1e-6

inline bool areCollinear(sptr<const Point> i_A,
                         sptr<const Point> i_B,
                         sptr<const Point> i_C)
{
    return fabs(i_A->m_x * (i_B->m_y - i_C->m_y) +
                i_B->m_x * (i_C->m_y - i_A->m_y) +
                i_C->m_x * (i_A->m_y - i_B->m_y)) < EPSILON;
}

inline bool areCoplanar(sptr<const Point> i_A,
                        sptr<const Point> i_B,
                        sptr<const Point> i_C,
                        sptr<const Point> i_D)
{
    return fabs(dot(*i_C - *i_A, cross(*i_B - *i_A, *i_D - *i_C))) < EPSILON;
}

void selectInitialTetrahedronVertices(const std::vector<sptr<const Point>>& i_Pts,
                                      uint& o_P1,
                                      uint& o_P2,
                                      uint& o_P3,
                                      uint& o_P4)
{
    // Random integer distribution
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<uint> uni(0, i_Pts.size() - 1);

    // Randomly pick the first point
    o_P1 = uni(rng);

    // Pick a different point
    do {
        o_P2 = uni(rng);
    } while (o_P2 == o_P1);

    // Pick a point that is not collinear with the 2 others
    do {
        o_P3 = uni(rng);
    } while (areCollinear(i_Pts[o_P1], i_Pts[o_P2], i_Pts[o_P3]));

    // Pick a point that is not coplanar with the 3 others
    do {
        o_P4 = uni(rng);
    } while (areCoplanar(i_Pts[o_P1], i_Pts[o_P2], i_Pts[o_P3], i_Pts[o_P4]));
}

int* createRandomPermutationOfIndices(uint i_N,
                                      uint i_P1,
                                      uint i_P2,
                                      uint i_P3,
                                      uint i_P4)
{
    // Build an array of point indices without indices of points chosen to
    // build the initial tetrahedron
    int* index = new int[i_N - 4];
    for (uint i = 0, j = 0; i < i_N; ++i) {
        if (i == i_P1 || i == i_P2 || i == i_P3 || i == i_P4) {
            continue;
        }
        index[j++] = i;
    }

    // Generate a random permutation with the remaining points
    std::random_shuffle(index, index + i_N - 4);

    return index;
}

void insertPointInConvexHull(uint              i_PtIndex,
                             sptr<const Point> i_Pts,
                             ConflictGraph&    io_graph,
                             sptr<DCEL3D>      io_DCEL)
{
    // Remove visible facets of DCEL and record their ids
    std::set<uint> visibleFacets;

    for (sptr<FacetNode>& toDelete : io_graph.m_Conflicts[i_PtIndex]) {
        io_DCEL->m_Facets[toDelete->m_Facet->m_ID] = NULL;
        visibleFacets.emplace(toDelete->m_Facet->m_ID);
    }

    // v Find an edge that is on the horizon
    sptr<HalfEdge> startEdge;

    // For each visible facet "F"
    for (sptr<FacetNode>& facet : io_graph.m_Conflicts[i_PtIndex]) {
        // For each adjacent facet "A"
        for (sptr<Facet>& adjFacet : facet->m_Facet->m_AdjFacets) {
            // If "F" has an adjacent facet "A" that is not visible from the
            // point, "F" has an edge that is on the horizon
            if (visibleFacets.find(adjFacet->m_ID) == visibleFacets.end()) {
                // Find the half-edge that is on the horizon
                startEdge = facet->m_Facet->m_AnEdge;
                while (startEdge->m_Twin->m_Facet->m_ID != adjFacet->m_ID) {
                    startEdge = startEdge->m_Next;
                }
                break;
            }
        }
        if (!startEdge) {
            break;
        }
    }
    assert(startEdge);

    // Walk along the horizon
    sptr<HalfEdge> edge;
    do {

        edge = edge->m_Next->m_Twin->m_Next;
    } while (edge != startEdge);
}

sptr<DCEL3D> compute3DConvexHull(const std::vector<sptr<const Point>>& i_Pts, sptr<ConflictGraph>& YOLOOOOOO)
{
    // Select points that forms the initial tetrahedron
    uint p1, p2, p3, p4;
    selectInitialTetrahedronVertices(i_Pts, p1, p2, p3, p4);

    // Build initial tetrahedric convex hull
    sptr<DCEL3D> convexHull(new DCEL3D(i_Pts[p1], i_Pts[p2], i_Pts[p3], i_Pts[p4]));

    // Create random permutation of indices
    int* index(createRandomPermutationOfIndices(i_Pts.size(), p1, p2, p3, p4));

    // Initialize conflict graph
    YOLOOOOOO = sptr<ConflictGraph>(new ConflictGraph(i_Pts, index, convexHull));
    //// Add each remaining point to the convex hull
    //for (uint i = 0; i < i_Pts.size() - 4; ++i) {
    //    sptr<const Point> pt(i_Pts[index[i]]);
    //    if (!graph.m_Conflicts[index[i]].empty()) {
    //        insertPointInConvexHull(index[i], i_Pts[index[i]], graph, convexHull);
    //    }
    //}

    // Get rid of those monstrous integers !
    delete[] index;

    return convexHull;
}

#endif