#pragma once

#include <vector>
#include <list>
#include <memory>

#include "BeachLine.h"

using namespace std;

/*
LIMITES:
	- Si un sommet est créé sur une arête, cette dernière n'est pas fractionnée en deux.
	- Si deux arêtes se rejoignant en un sommet forment un angle trop petit, il peut y avoir de mauvais résultats.
	- Si on ajoute une arête qui part ou arrive sur une arête existante, cette dernière n'est pas fractionnée.
	- Lorsqu'une region est fractionnée en deux, deux nouvelles sont créées et l'ancienne est supprimée. Un pointeur sur l'ancienne devient invalide.
	- On considère que le contour extérieur du graphe délimite une region.
*/

class Region;

class HalfEdge
{
public:
	Coord origin;
	shared_ptr<Region> region;
	shared_ptr<HalfEdge> twin;
	shared_ptr<HalfEdge> next;
	shared_ptr<HalfEdge> prev;
	bool line_seen; // Used by get_lines()

	HalfEdge(Coord a_origin)
	: origin(a_origin)
	, line_seen(false)
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
	Coord to_vector()
	{
		return Coord(twin->origin.x - origin.x, twin->origin.y - origin.y);
	}
};

class Vertex
{
public:
	Coord point;
	shared_ptr<HalfEdge> an_edge;

	Vertex(Coord a_point)
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
	shared_ptr<HalfEdge> begin_edge(Coord point);
	void complete_edge(shared_ptr<HalfEdge> half_edge_start, Coord point);
	shared_ptr<HalfEdge> add_edge(const Coord start, const Coord end);
	vector<pair<Coord, Coord>> get_lines();
	void close_boundaries(double left, double bottom, double right, double top);
	list<shared_ptr<HalfEdge>> get_half_edge_unliked();

	DoublyConnectedEdgeList() : no_line_seen(true)
	{}

private:
	list<shared_ptr<Region>> regions;
	vector<shared_ptr<HalfEdge>> edges;
	vector<Vertex> vertices;
	list<shared_ptr<HalfEdge>> unlinked_edges;
	bool no_line_seen; // used by get_lines()

	vector<shared_ptr<HalfEdge>> get_edges_touching_point(Coord point);
	shared_ptr<HalfEdge> find_adjac_half_edge(bool is_next, shared_ptr<HalfEdge> half_edge);
	shared_ptr<HalfEdge> find_next_half_edge(shared_ptr<HalfEdge> half_edge);
	shared_ptr<HalfEdge> find_prev_half_edge(shared_ptr<HalfEdge> half_edge);
	void connect_all_edges();
	void assign_region_to_loop(shared_ptr<Region>, shared_ptr<HalfEdge>);
};