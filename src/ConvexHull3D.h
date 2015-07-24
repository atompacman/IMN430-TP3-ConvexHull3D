#ifndef __ConvexHull3D__
#define __ConvexHull3D__

#include <algorithm>
#include <ctime>
#include <list>
#include <random>
#include <vector>

#include "ConflictGraph.h"
#include "DCEL3D.h"
#include "Point.h"

#define EPSILON 1e-6

inline bool areCollinear(const Point& i_A,
                         const Point& i_B,
                         const Point& i_C)
{
    return fabs(i_A.m_x * (i_B.m_y - i_C.m_y) +
                i_B.m_x * (i_C.m_y - i_A.m_y) +
                i_C.m_x * (i_A.m_y - i_B.m_y)) < EPSILON;
}

inline bool areCoplanar(const Point& i_A,
                        const Point& i_B,
                        const Point& i_C,
                        const Point& i_D)
{
    return fabs(dot(i_C - i_A, cross(i_B - i_A, i_D - i_C))) < EPSILON;
}

void selectInitialTetrahedronVertices(const std::vector<Point>& i_Pts,
                                      uint&                     o_P1,
                                      uint&                     o_P2,
                                      uint&                     o_P3,
                                      uint&                     o_P4)
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

sptr<DCEL3D> compute3DConvexHull(const std::vector<Point>& i_Pts)
{
    // Select points that forms the initial tetrahedron
    uint p1, p2, p3, p4;
    selectInitialTetrahedronVertices(i_Pts, p1, p2, p3, p4);

    // Build initial tetrahedric convex hull
    sptr<DCEL3D> convexHull(new DCEL3D(i_Pts[p1], i_Pts[p2], i_Pts[p3], i_Pts[p4]));

    // Create random permutation of indices
    int* index(createRandomPermutationOfIndices(i_Pts.size(), p1, p2, p3, p4));

    return convexHull;
}

#endif