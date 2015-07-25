#include "DCEL3D.h"

/************************************************************************/
/*                             HalfEdge                                 */
/************************************************************************/

HalfEdge::HalfEdge(sptr<const Point> i_Origin) :
    m_Origin(i_Origin),
    m_Twin(NULL),
    m_Next(NULL),
    m_Prev(NULL),
    m_Facet(NULL){}

sptr<HalfEdge> HalfEdge::connectTo(sptr<HalfEdge> i_HalfEdge)
{
    m_Next = i_HalfEdge;
    i_HalfEdge->m_Prev = shared_from_this();
    return i_HalfEdge;
}

sptr<HalfEdge> HalfEdge::connectTo(sptr<const Point> i_Pt)
{
    return connectTo(sptr<HalfEdge>(new HalfEdge(i_Pt)));
}

void HalfEdge::twinTo(sptr<HalfEdge> i_HalfEdge)
{
    assert(m_Next->m_Origin == i_HalfEdge->m_Origin &&
           i_HalfEdge->m_Next->m_Origin == m_Origin);
    m_Twin = i_HalfEdge;
    i_HalfEdge->m_Twin = shared_from_this();
}

bool HalfEdge::hasExtremities(sptr<const Point> i_PtA, sptr<const Point> i_PtB)
{
    return m_Origin == i_PtA && m_Next->m_Origin == i_PtB ||
           m_Origin == i_PtB && m_Next->m_Origin == i_PtA;
}


/************************************************************************/
/*                               Facet                                  */
/************************************************************************/

uint Facet::s_LastID = 0;

Facet::Facet(sptr<const Point> i_PtA,
             sptr<const Point> i_PtB,
             sptr<const Point> i_PtC,
             sptr<const Point> i_InsideDCEL3D) :

    m_Normal(cross(*i_PtB - *i_PtA, *i_PtC - *i_PtA)),
    m_AnEdge(new HalfEdge(i_PtA)),
    m_ID(s_LastID++),
    m_AdjFacets()
{
    // Check if its normal points in the right direction 
    // (away from the center of the DCEL)
    if (isVisibleFor(i_InsideDCEL3D)) {
        m_Normal *= -1;
        m_AnEdge->connectTo(i_PtC)->connectTo(i_PtB)->connectTo(m_AnEdge);
    }
    else {
        m_AnEdge->connectTo(i_PtB)->connectTo(i_PtC)->connectTo(m_AnEdge);
    }
}

void Facet::connectTo(sptr<Facet> i_Facet, sptr<const Point> i_Pti_PtA, sptr<const Point> i_PtB)
{
    findHalfEdge(i_Pti_PtA, i_PtB)->twinTo(i_Facet->findHalfEdge(i_Pti_PtA, i_PtB));
    m_AdjFacets.emplace_back(i_Facet);
    i_Facet->m_AdjFacets.emplace_back(shared_from_this());
}

sptr<HalfEdge> Facet::findHalfEdge(sptr<const Point> i_Pti_PtA, sptr<const Point> i_PtB)
{
    sptr<HalfEdge> currEdge(m_AnEdge->m_Next);
    while (currEdge != m_AnEdge && !currEdge->hasExtremities(i_Pti_PtA, i_PtB)) {
        currEdge = currEdge->m_Next;
    }
    assert(currEdge->hasExtremities(i_Pti_PtA, i_PtB));
    return currEdge;
}

bool Facet::isVisibleFor(sptr<const Point> i_Pt)
{
    return dot(m_Normal, *i_Pt) > dot(m_Normal, *(m_AnEdge->m_Origin));
}


/************************************************************************/
/*                                DCEL                                  */
/************************************************************************/

DCEL3D::DCEL3D(sptr<const Point> i_PtA,
               sptr<const Point> i_PtB,
               sptr<const Point> i_PtC,
               sptr<const Point> i_PtD) :

    m_PtInside(new Point((*i_PtA + *i_PtB + *i_PtC + *i_PtD) * 0.25)),
    m_Facets()
{
    // Create first four facets
    sptr<Facet> abc(addFacet(i_PtA, i_PtB, i_PtC));
    sptr<Facet> bcd(addFacet(i_PtB, i_PtC, i_PtD));
    sptr<Facet> acd(addFacet(i_PtA, i_PtC, i_PtD));
    sptr<Facet> abd(addFacet(i_PtA, i_PtB, i_PtD));

    // Connect them via twins
    // TODO: make this part suck less
    abc->connectTo(bcd, i_PtB, i_PtC);
    abc->connectTo(acd, i_PtA, i_PtC);
    abc->connectTo(abd, i_PtA, i_PtB);
    bcd->connectTo(abc, i_PtB, i_PtC);
    bcd->connectTo(acd, i_PtC, i_PtD);
    bcd->connectTo(abd, i_PtB, i_PtD);
    acd->connectTo(abc, i_PtA, i_PtC);
    acd->connectTo(bcd, i_PtC, i_PtD);
    acd->connectTo(abd, i_PtA, i_PtD);
    abd->connectTo(abc, i_PtA, i_PtB);
    abd->connectTo(bcd, i_PtB, i_PtD);
    abd->connectTo(acd, i_PtA, i_PtD);
}

sptr<Facet> DCEL3D::addFacet(sptr<const Point> i_P1,
                             sptr<const Point> i_P2,
                             sptr<const Point> i_P3)
{
    // Create facet
    sptr<Facet> facet(new Facet(i_P1, i_P2, i_P3, m_PtInside));

    // Add it to the list
    m_Facets.push_back(facet);

    // Link edges to facet
    facet->m_AnEdge->m_Facet = facet;
    facet->m_AnEdge->m_Next->m_Facet = facet;
    facet->m_AnEdge->m_Next->m_Next->m_Facet = facet;

    return facet;
}