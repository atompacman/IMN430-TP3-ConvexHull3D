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

    HalfEdge(sptr<const Point> i_Origin) :
        m_Origin(i_Origin),
        m_Twin(NULL),
        m_Next(NULL),
        m_Prev(NULL){}

    sptr<HalfEdge> connectTo(sptr<HalfEdge> i_HalfEdge)
    {
        m_Next = i_HalfEdge;
        i_HalfEdge->m_Prev = shared_from_this();
        return i_HalfEdge;
    }

    sptr<HalfEdge> connectTo(sptr<const Point> i_Pt)
    {
        return connectTo(sptr<HalfEdge>(new HalfEdge(i_Pt)));
    }

    void twinTo(sptr<HalfEdge> i_HalfEdge)
    {
        assert(m_Next->m_Origin == i_HalfEdge->m_Origin &&
               i_HalfEdge->m_Next->m_Origin == m_Origin);
        m_Twin = i_HalfEdge;
        i_HalfEdge->m_Twin = shared_from_this();
    }

    bool hasExtremities(sptr<const Point> i_PtA, sptr<const Point> i_PtB)
    {
        return m_Origin == i_PtA && m_Next->m_Origin == i_PtB ||
               m_Origin == i_PtB && m_Next->m_Origin == i_PtA;
    }
};

struct Facet
{
    sptr<HalfEdge> m_AnEdge;
    Vector         m_Normal;

    Facet(sptr<const Point> i_PtA,
          sptr<const Point> i_PtB,
          sptr<const Point> i_PtC,
          sptr<const Point> i_InsideDCEL3D) :

        m_Normal(cross(*i_PtB - *i_PtA, *i_PtC - *i_PtA)),
        m_AnEdge(new HalfEdge(i_PtA))
    {
        // Check if its normal points in the right direction 
        // (away from the center of the DCEL)
        if (isVisibleFrom(i_InsideDCEL3D)) {
            m_Normal *= -1;
            m_AnEdge->connectTo(i_PtC)->connectTo(i_PtB)->connectTo(m_AnEdge);
        } else {
            m_AnEdge->connectTo(i_PtB)->connectTo(i_PtC)->connectTo(m_AnEdge);
        }
    }

    void connectTo(sptr<Facet> i_Facet, sptr<const Point> i_PtA, sptr<const Point> i_PtB)
    {
        findHalfEdge(i_PtA, i_PtB)->twinTo(i_Facet->findHalfEdge(i_PtA, i_PtB));
    }

    /************************************************************************/
    /* Find the edge that as two specific extremities                       */
    /************************************************************************/
    sptr<HalfEdge> findHalfEdge(sptr<const Point> i_PtA, sptr<const Point> i_PtB)
    {
        sptr<HalfEdge> currEdge(m_AnEdge->m_Next);
        while (currEdge != m_AnEdge && !currEdge->hasExtremities(i_PtA, i_PtB)) {
            currEdge = currEdge->m_Next;
        }
        assert(currEdge->hasExtremities(i_PtA, i_PtB));
        return currEdge;
    }

    bool isVisibleFrom(sptr<const Point> i_Pt)
    {
        return dot(m_Normal, *i_Pt) > dot(m_Normal, *(m_AnEdge->m_Origin));
    }
};

struct DCEL3D
{
    sptr<const Point>      m_PtInside;
    std::list<sptr<Facet>> m_Facets;

    DCEL3D(const Point& i_PtA,
           const Point& i_PtB,
           const Point& i_PtC,
           const Point& i_PtD) :

        m_PtInside(new Point((i_PtA + i_PtB + i_PtC + i_PtD) * 0.25)),
        m_Facets()
    {
        // Create points pointers
        sptr<const Point> a(&i_PtA);
        sptr<const Point> b(&i_PtB);
        sptr<const Point> c(&i_PtC);
        sptr<const Point> d(&i_PtD);

        // create first four facets
        sptr<Facet> abc(addFacet(a, b, c));
        sptr<Facet> bcd(addFacet(b, c, d));
        sptr<Facet> acd(addFacet(a, c, d));
        sptr<Facet> abd(addFacet(a, b, d));

        // connect them via twins
        // TOdO: make this part suck less
        abc->connectTo(bcd, b, c);
        abc->connectTo(acd, a, c);
        abc->connectTo(abd, a, b);
        bcd->connectTo(abc, b, c);
        bcd->connectTo(acd, c, d);
        bcd->connectTo(abd, b, d);
        acd->connectTo(abc, a, c);
        acd->connectTo(bcd, c, d);
        acd->connectTo(abd, a, d);
        abd->connectTo(abc, a, b);
        abd->connectTo(bcd, b, d);
        abd->connectTo(acd, a, d);
    }

    sptr<Facet> addFacet(sptr<const Point> i_P1,
                         sptr<const Point> i_P2,
                         sptr<const Point> i_P3)
    {
        m_Facets.emplace_back(new Facet(i_P1, i_P2, i_P3, m_PtInside));
        return m_Facets.back();
    }
};

#endif