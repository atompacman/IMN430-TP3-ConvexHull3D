#define _USE_MATH_DEFINES

#include "DoublyConnectedEdgeList.h"
#include <cassert>

shared_ptr<HalfEdge> DoublyConnectedEdgeList::add_edge(const Point start, const Point end)
{
	shared_ptr<HalfEdge> e = begin_edge(start);
	complete_edge(e, end);
	return e;
}

/*
Returns a "dangling" half-edge waiting to be completed with its twin using complete_edge().
*/
shared_ptr<HalfEdge> DoublyConnectedEdgeList::begin_edge(Point point)
{
	// Create vertex
	vertices.emplace_back(point);
	Vertex* v = &vertices.back();

	// Create half edge
	edges.emplace_back(shared_ptr<HalfEdge>(new HalfEdge(point)));
	shared_ptr<HalfEdge> e = edges.back();

	// Link an edge to dat vertex
	v->an_edge = e;

	return e;
}

void DoublyConnectedEdgeList::complete_edge(shared_ptr<HalfEdge> half_edge_start, Point point)
{
	// Create second half edge -----------
	// Create vertex
	vertices.emplace_back(point);
	Vertex* v = &vertices.back();

	// Create half edge
	edges.emplace_back(shared_ptr<HalfEdge>(new HalfEdge(point)));
	shared_ptr<HalfEdge> half_edge_end = edges.back();

	// Link an edge to dat vertex
	v->an_edge = half_edge_end;

	// Twin-up those half edges ----------
	half_edge_start->twin = half_edge_end;
	half_edge_end->twin = half_edge_start;
}

void DoublyConnectedEdgeList::assign_region_to_loop(shared_ptr<Region> region, shared_ptr<HalfEdge> half_edge)
{
	half_edge->region = region;
	shared_ptr<HalfEdge> cur_half_edge = half_edge->next;
	while (cur_half_edge != half_edge)
	{
		assert(cur_half_edge);
		cur_half_edge->region = region;
		cur_half_edge = cur_half_edge->next;
	}
}