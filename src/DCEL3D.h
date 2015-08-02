#ifndef __DCEL__
#define __DCEL__

#include <list>
#include <memory>
#include <set>
#include <vector>

#include "Point.h"

#define EPSILON 1e-8
#define sptr std::shared_ptr

typedef std::shared_ptr<const Point> sPoint;
typedef unsigned int uint;

struct Facet;

struct HalfEdge : std::enable_shared_from_this<HalfEdge>
{
    sPoint         m_Origin;
    sptr<HalfEdge> m_Twin;
    sptr<HalfEdge> m_Next;
    sptr<HalfEdge> m_Prev;
    sptr<Facet>    m_Facet;

    HalfEdge(sPoint i_Origin);

    sptr<HalfEdge> connectTo(sptr<HalfEdge> i_HalfEdge);
    sptr<HalfEdge> connectTo(sPoint i_Pt);

    void twinTo(sptr<HalfEdge> i_HalfEdge);

    bool hasExtremities(sPoint i_PtA, sPoint i_PtB);
};

struct Facet : std::enable_shared_from_this<Facet>
{
    sptr<HalfEdge> m_AnEdge;
    Vector         m_Normal;
    uint           m_ID;
    std::set<uint> m_Conflicts;

    static uint    s_LastID;

    Facet(sPoint i_PtA, sPoint i_PtB, sPoint i_PtC, sPoint i_InsideDCEL3D);

    void connectTo(sptr<Facet> i_Facet, sPoint i_PtA, sPoint i_PtB);

    sptr<HalfEdge> findHalfEdge(sPoint i_PtA, sPoint i_PtB);

    bool isVisibleBy(sPoint i_Pt);

    bool isCoplanarWith(sPoint i_Pt);
};

struct DCEL3D
{
    sPoint        m_PtInside;
    std::vector<sptr<Facet>> m_Facets;

    DCEL3D(sPoint i_PtA, sPoint i_PtB, sPoint i_PtC, sPoint i_PtD);

    sptr<Facet> addFacet(sPoint i_P1, sPoint i_P2, sPoint i_P3);
};

#endif