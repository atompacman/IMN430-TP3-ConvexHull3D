#ifndef __DCEL__
#define __DCEL__

#include <list>
#include <memory>
#include <vector>

#include "Point.h"

typedef unsigned int uint;

#define sptr std::shared_ptr

struct HalfEdge : std::enable_shared_from_this<HalfEdge>
{
    sptr<const Point> m_Origin;
    sptr<HalfEdge>    m_Twin;
    sptr<HalfEdge>    m_Next;
    sptr<HalfEdge>    m_Prev;
    sptr<Facet>       m_Facet;

    HalfEdge(sptr<const Point> i_Origin);

    sptr<HalfEdge> connectTo(sptr<HalfEdge> i_HalfEdge);
    sptr<HalfEdge> connectTo(sptr<const Point> i_Pt);

    void twinTo(sptr<HalfEdge> i_HalfEdge);

    bool hasExtremities(sptr<const Point> i_PtA, sptr<const Point> i_PtB);
};

struct Facet : std::enable_shared_from_this<HalfEdge>
{
    sptr<HalfEdge>         m_AnEdge;
    Vector                 m_Normal;
    uint                   m_ID;
    std::list<sptr<Facet>> m_AdjFacets;

    static uint    s_LastID;

    Facet(sptr<const Point> i_PtA,
          sptr<const Point> i_PtB,
          sptr<const Point> i_PtC,
          sptr<const Point> i_InsideDCEL3D);

    void connectTo(sptr<Facet> i_Facet, sptr<const Point> i_PtA, sptr<const Point> i_PtB);

    sptr<HalfEdge> findHalfEdge(sptr<const Point> i_PtA, sptr<const Point> i_PtB);

    bool isVisibleFor(sptr<const Point> i_Pt);
};

struct DCEL3D
{
    sptr<const Point>        m_PtInside;
    std::vector<sptr<Facet>> m_Facets;

    DCEL3D(sptr<const Point> i_PtA,
           sptr<const Point> i_PtB,
           sptr<const Point> i_PtC,
           sptr<const Point> i_PtD);

    sptr<Facet> addFacet(sptr<const Point> i_P1,
                         sptr<const Point> i_P2,
                         sptr<const Point> i_P3);
};

#endif