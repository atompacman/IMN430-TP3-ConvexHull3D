#pragma once

#include <vector>
#include <list>
#include <memory>
#include "Point.h"

using namespace std;

class Region;

class HalfEdge
{
public:
	Point origin;
	shared_ptr<Region> region;
	shared_ptr<HalfEdge> twin;
	shared_ptr<HalfEdge> next;
	shared_ptr<HalfEdge> prev;

	HalfEdge(Point a_origin)
	: origin(a_origin)
	{}
	bool is_linked()
	{
		return next != nullptr && prev != nullptr;
	}
	bool is_part_of_loop()
	{
		int nb_iter(0);
		shared_ptr<HalfEdge> cur_half_edge = next;
		while (cur_half_edge.get() != this)
		{
			if (!cur_half_edge || nb_iter > 200) return false; // Not in a closed loop
			cur_half_edge = cur_half_edge->next;
			
			nb_iter++;
		}
		return true; // We have looped around a region
	}
};

class Vertex
{
public:
	Point point;
	shared_ptr<HalfEdge> an_edge;

	Vertex(Point a_point)
	{
		point = a_point;
	}
};

class Region
{
public:
	shared_ptr<HalfEdge> an_edge;

	Region(shared_ptr<HalfEdge> a_edge)
	{
		an_edge = a_edge;
	}
};

class DoublyConnectedEdgeList
{
public:
	shared_ptr<HalfEdge> begin_edge(Point point);
	void complete_edge(shared_ptr<HalfEdge> half_edge_start, Point point);
	shared_ptr<HalfEdge> add_edge(const Point start, const Point end);

	DoublyConnectedEdgeList()
	{}

	list<shared_ptr<Region>> regions;
	vector<shared_ptr<HalfEdge>> edges;
	vector<Vertex> vertices;

	void assign_region_to_loop(shared_ptr<Region>, shared_ptr<HalfEdge>);
};