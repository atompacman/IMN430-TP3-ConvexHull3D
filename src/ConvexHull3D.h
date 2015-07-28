#ifndef __ConvexHull3D__
#define __ConvexHull3D__

#include <algorithm>
#include <ctime>
#include <iostream>
#include <iterator>
#include <list>
#include <random>
#include <set>
#include <vector>

#include "DCEL3D.h"
#include "Point.h"

std::vector<sPoint> g_Pts;
int*                g_Index;
sptr<DCEL3D>        g_ConvexHull;
std::set<uint>*     g_FacetsVisibleByPt;

bool areCollinear(sPoint i_A, sPoint i_B, sPoint i_C)
{
    return fabs(i_A->m_x * (i_B->m_y - i_C->m_y) +
                i_B->m_x * (i_C->m_y - i_A->m_y) +
                i_C->m_x * (i_A->m_y - i_B->m_y)) < EPSILON;
}

bool areCoplanar(sPoint i_A, sPoint i_B, sPoint i_C, sPoint i_D)
{
    return fabs(dot(*i_C - *i_A, cross(*i_B - *i_A, *i_D - *i_C))) < EPSILON;
}

void selectInitialTetrahedronVertices(uint& o_P1, uint& o_P2, uint& o_P3, uint& o_P4)
{
    // Random integer distribution
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<uint> uni(0, g_Pts.size() - 1);

    // Randomly pick the first point
    o_P1 = uni(rng);

    // Pick a different point
    do {
        o_P2 = uni(rng);
    } while (o_P2 == o_P1);

    // Pick a point that is not collinear with the 2 others
    do {
        o_P3 = uni(rng);
    } while (areCollinear(g_Pts[o_P1], g_Pts[o_P2], g_Pts[o_P3]));

    // Pick a point that is not coplanar with the 3 others
    do {
        o_P4 = uni(rng);
    } while (areCoplanar(g_Pts[o_P1], g_Pts[o_P2], g_Pts[o_P3], g_Pts[o_P4]));
}

void createRandomPermutationOfIndices(uint i_P1, uint i_P2, uint i_P3, uint i_P4)
{
    // Build an array of point indices without indices of points chosen to
    // build the initial tetrahedron
    g_Index = new int[g_Pts.size() - 4];

    for (uint i = 0, j = 0; i < g_Pts.size(); ++i) {
        if (i == i_P1 || i == i_P2 || i == i_P3 || i == i_P4) {
            continue;
        }
        g_Index[j++] = i;
    }

    // Generate a random permutation with the remaining points
    std::random_shuffle(g_Index, g_Index + g_Pts.size() - 4);
}

void createConflictGraph()
{
    // For each point to insert, there is a list of facets with which they are in conflict
    g_FacetsVisibleByPt = new std::set<uint>[g_Pts.size()];

    // For each facet
    for (const sptr<Facet>& facet : g_ConvexHull->m_Facets) {
        // For each point
        for (uint i = 0; i < g_Pts.size() - 4; ++i) {
            // If facet is visible from the point
            uint index(g_Index[i]);
            if (facet->isVisibleBy(g_Pts[index])) {
                facet->m_Conflicts.emplace(index);
                g_FacetsVisibleByPt[index].emplace(facet->m_ID);
            }
        }
    }
}

sptr<HalfEdge> findAnHalfEdgeOfFacetOnHorizon(uint i_FacetID, uint i_PtIdx)
{
    sptr<HalfEdge> halfEdge(g_ConvexHull->m_Facets[i_FacetID]->m_AnEdge);
    sptr<HalfEdge> firstEdge(halfEdge);
    std::set<uint>& visibleFacetIDs(g_FacetsVisibleByPt[i_PtIdx]);

    // For every half-edge forming the facet
    do {
        // If facet next to that half-edge is not visible, then edge is on the horizon
        if (visibleFacetIDs.find(halfEdge->m_Twin->m_Facet->m_ID) == visibleFacetIDs.end()) {
            return halfEdge;
        }
        halfEdge = halfEdge->m_Next;
    } while (halfEdge != firstEdge);

    // This facet do not border the horizon
    return NULL;
}

sptr<HalfEdge> findNextHalfEdgeOnHorizon(sptr<HalfEdge> i_HalfEdge, uint i_PtIdx)
{
    sptr<HalfEdge> nextHalfEdge = i_HalfEdge->m_Next;
    std::set<uint>& visibleFacetIDs(g_FacetsVisibleByPt[i_PtIdx]);

    // For every half-edge sharing the same origin
    do {
        // If facet next to that half-edge is not visible, then edge is on the horizon
        if (visibleFacetIDs.find(nextHalfEdge->m_Twin->m_Facet->m_ID) == visibleFacetIDs.end()) {
            return nextHalfEdge;
        }
        nextHalfEdge = nextHalfEdge->m_Twin->m_Next;
    } while (nextHalfEdge != i_HalfEdge);

    // We're dead, Jim
    assert(false);
    return NULL;
}

void addNewConflicts(sptr<Facet> m_FromFacet, sptr<Facet> m_ToFacet, uint i_ProcessedPt)
{
    // For each point in conflict with the "From" facet
    for (uint index : m_FromFacet->m_Conflicts) {
        // Do not add new faces to visible faces of processed point, because
        // when need to keep clean the list of old faces in order to delete them
        if (index == i_ProcessedPt) {
            continue;
        }
        // If "To" facet is visible from the point
        if (m_ToFacet->isVisibleBy(g_Pts[index])) {
            m_ToFacet->m_Conflicts.emplace(index);
            g_FacetsVisibleByPt[index].emplace(m_ToFacet->m_ID);
        }
    }
}

sptr<HalfEdge> addNewFace(uint i_PtIdx, sptr<HalfEdge> i_HalfEdge)
{
    sptr<HalfEdge> twin(i_HalfEdge->m_Twin);

    // Check if the face that must be created is coplanar with its adjacent face
    if (twin->m_Facet->isCoplanarWith(g_Pts[i_PtIdx])) {
        // Twin is now connected to the new point, which is connected to the 
        // end of the half-edge on horizon
        twin->connectTo(g_Pts[i_PtIdx])->connectTo(twin->m_Next);
        // Twin's twin is now unkown
        twin->m_Twin = NULL;

        return twin;
    } 
    else {
        // Create new facet
        sptr<Facet> newFacet(g_ConvexHull->addFacet(i_HalfEdge->m_Origin, 
                                                    i_HalfEdge->m_Next->m_Origin, 
                                                    g_Pts[i_PtIdx]));
        // Set twins for new facet
        twin->twinTo(newFacet->m_AnEdge);

        // For each point in conflict with the first facet
        addNewConflicts(i_HalfEdge->m_Facet, newFacet, i_PtIdx);

        // For each point in conflict with the second facet
        addNewConflicts(twin->m_Facet, newFacet, i_PtIdx);

        return newFacet->m_AnEdge->m_Next;
    }
}

void insertPointInConvexHull(uint i_PtIdx)
{
    // Find an arbitrary edge that is on the horizon
    sptr<HalfEdge> startEdge;

    // For each visible facet
    for (uint facetID : g_FacetsVisibleByPt[i_PtIdx]) {
        // Find an half-edge that is on the horizon
        startEdge = findAnHalfEdgeOfFacetOnHorizon(facetID, i_PtIdx);
        // If we found one
        if (startEdge) {
            break;
        }
    }

    // There should be at least three edges on the horizon so...
    assert(startEdge);

    // Walk along the horizon
    sptr<HalfEdge> halfEdge(startEdge);
    sptr<HalfEdge> waiting4ATwin;
    sptr<HalfEdge> lastToTwin;
    do {
        // Add new face to convex hull
        sptr<HalfEdge> twinMe(addNewFace(i_PtIdx, halfEdge));

        // Connect it to previous new facet
        if (waiting4ATwin) {
            twinMe->m_Next->twinTo(waiting4ATwin);
        } else {
            lastToTwin = twinMe;
        }
        waiting4ATwin = twinMe;

        // Find next edge on horizon
        halfEdge = findNextHalfEdgeOnHorizon(halfEdge, i_PtIdx);
    } while (halfEdge != startEdge);

    // Connect last facet with first one
    lastToTwin->m_Next->twinTo(waiting4ATwin);

    // Delete arcs incident to deleted facets
    for (uint facetID : g_FacetsVisibleByPt[i_PtIdx]) {
        for (uint ptIndex : g_ConvexHull->m_Facets[facetID]->m_Conflicts) {
            if (i_PtIdx != ptIndex) {
                g_FacetsVisibleByPt[ptIndex].erase(facetID);
            }
        }
        // Remove the facet from the DCEL (it is still referenced by twins)
        g_ConvexHull->m_Facets[facetID] = NULL;
    }
}

sptr<DCEL3D> compute3DConvexHull()
{
    std::cout << "Building initial tetrahedron" << std::endl;

    // Select points that forms the initial tetrahedron
    uint p1, p2, p3, p4;
    selectInitialTetrahedronVertices(p1, p2, p3, p4);

    // Create random permutation of indices
    createRandomPermutationOfIndices(p1, p2, p3, p4);

    // Build initial tetrahedric convex hull
    g_ConvexHull = sptr<DCEL3D>(new DCEL3D(g_Pts[p1], g_Pts[p2], g_Pts[p3], g_Pts[p4]));

    // Create conflict graph
    std::cout << "Creating initial conflict graph" << std::endl;
    createConflictGraph();

    // Add each remaining point to the convex hull
    for (uint i = 0; i < g_Pts.size() - 4; ++i) {
        std::cout << "\rAdding point " << i << "/" << g_Pts.size() - 4;
        if (!g_FacetsVisibleByPt[g_Index[i]].empty()) {
            insertPointInConvexHull(g_Index[i]);
        }
    }

    // Get rid of those monstrous integers !
    delete[] g_Index;

    return g_ConvexHull;
}

#endif