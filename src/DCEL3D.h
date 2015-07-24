#ifndef __DCEL__
#define __DCEL__

#include <list>
#include <memory>

#include "Point.h"

#define sptr std::shared_ptr

struct HalfEdge : std::enable_shared_from_this<HalfEdge>
{
    sptr<const Point> m_Origin;
    sptr<HalfEdge>    m_Twin;
    sptr<HalfEdge>    m_Next;
    sptr<HalfEdge>    m_Prev;

    HalfEdge(const Point& i_Origin) :
        m_Origin(sptr<const Point>(&i_Origin)){}

    inline sptr<HalfEdge> connectTo(sptr<HalfEdge> i_Edge)
    {
        m_Next = i_Edge;
        i_Edge->m_Prev = shared_from_this();
        return i_Edge;
    }

    inline sptr<HalfEdge> connectTo(const Point& i_Pt)
    {
        return connectTo(sptr<HalfEdge>(new HalfEdge(i_Pt)));
    }
};

struct Facet
{
    sptr<HalfEdge> m_AnEdge;
    Vector         m_Normal;

    Facet(const Point& i_PtA,
          const Point& i_PtB,
          const Point& i_PtC):

          m_Normal(cross(i_PtB - i_PtA, i_PtC - i_PtA)),
          m_AnEdge(new HalfEdge(i_PtA))
    {
        m_AnEdge->connectTo(i_PtB)->connectTo(i_PtC)->connectTo(m_AnEdge);
    }
};

struct DCEL3D
{
    Point                  m_PtInside;
    std::list<sptr<Facet>> m_Facets;

    DCEL3D(const Point& i_PtA,
           const Point& i_PtB,
           const Point& i_PtC, 
           const Point& i_PtD):

           m_PtInside((i_PtA + i_PtB + i_PtC + i_PtD) * 0.25),
           m_Facets(4)
    {
        
    }

    void addFacet(const Point& i_P1,
                  const Point& i_P2,
                  const Point& i_P3)
    {
        HalfEdge a(i_P1);
    }
};

static bool isVisible(const Point& i_Pt, const FacetNode& i_Facet)
{
    Point& a(i_Facet.m_Pts[0]);
    Point& b(i_Facet.m_Pts[1]);
    Point& c(i_Facet.m_Pts[2]);

    Vector normal(cross(b - a, c - a));
    return normal.m_x * i_Pt.m_x +
           normal.m_y * i_Pt.m_y +
           normal.m_z * i_Pt.m_z  >
                                    normal.m_x * a.m_x +
                                    normal.m_y * a.m_y +
                                    normal.m_z * a.m_z;
}

#endif