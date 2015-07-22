#define _USE_MATH_DEFINES

#include "DoublyConnectedEdgeList.h"
#include <cassert>

/*
Returns all half-edges having 'point' as their origin.
*/
vector<shared_ptr<HalfEdge>> DoublyConnectedEdgeList::get_edges_touching_point(Coord point)
{
	vector<shared_ptr<HalfEdge>> edges_touching_point;

	for (shared_ptr<HalfEdge> e : edges)
	{
		if (e->origin == point)
		{
			edges_touching_point.push_back(e);
		}
	}

	return edges_touching_point;
}

shared_ptr<HalfEdge> DoublyConnectedEdgeList::add_edge(const Coord start, const Coord end)
{
	shared_ptr<HalfEdge> e = begin_edge(start);
	complete_edge(e, end);
	return e;
}

/*
Returns a "dangling" half-edge waiting to be completed with its twin using complete_edge().
*/
shared_ptr<HalfEdge> DoublyConnectedEdgeList::begin_edge(Coord point)
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

void DoublyConnectedEdgeList::complete_edge(shared_ptr<HalfEdge> half_edge_start, Coord point)
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

	// Set next and prev pointers
	unlinked_edges.push_back(half_edge_start);
	unlinked_edges.push_back(half_edge_end);
	connect_all_edges();
}

/*
This method signature requires explanation.
If the argument 'is_next' is set to true, this method finds the half-edge following the one given in the argument 'half_edge'.
If the argument 'is_next' is set to false, this method finds the previous half-edge.
*/
shared_ptr<HalfEdge> DoublyConnectedEdgeList::find_adjac_half_edge(bool is_next, shared_ptr<HalfEdge> half_edge)
{
	shared_ptr<HalfEdge> half_edge_a = (is_next) ? half_edge->twin : half_edge;
	
	double lowest_angle = numeric_limits<double>::infinity();
	shared_ptr<HalfEdge> next_prev_half_edge; // null by default

	// Angle of half_edge_a (counterclockwise from x axis)
	double half_edge_a_angle = atan2(half_edge_a->twin->origin.y - half_edge_a->origin.y, half_edge_a->twin->origin.x - half_edge_a->origin.x);
	if (half_edge_a_angle < 0) half_edge_a_angle = 2 * M_PI + half_edge_a_angle;

	for (shared_ptr<HalfEdge> half_edge_b : get_edges_touching_point(half_edge_a->origin)) // half-edges qui touchent la fin de a
	{
		if (half_edge_a == half_edge_b || half_edge_a == half_edge_b->twin || half_edge_b == half_edge_a->twin) continue;

		// Angle of half_edge_b (counterclockwise from x axis)
		double half_edge_b_angle = atan2(half_edge_b->twin->origin.y - half_edge_b->origin.y, half_edge_b->twin->origin.x - half_edge_b->origin.x);
		if (half_edge_b_angle < 0) half_edge_b_angle = 2 * M_PI + half_edge_b_angle;

		double angle_from_a_to_b = half_edge_b_angle - half_edge_a_angle;
		if (angle_from_a_to_b < 0) angle_from_a_to_b = 2 * M_PI + angle_from_a_to_b;
		assert(angle_from_a_to_b >= 0);
		assert(angle_from_a_to_b < 2 * M_PI);

		if (is_next)
			angle_from_a_to_b = 2 * M_PI - angle_from_a_to_b;

		if (angle_from_a_to_b < lowest_angle)
		{
			lowest_angle = angle_from_a_to_b;
			next_prev_half_edge = half_edge_b;
		}
	}

	if (!next_prev_half_edge || is_next) // If next_prev_half_edge is null, return null
		return next_prev_half_edge;
	else
		return next_prev_half_edge->twin;
}

shared_ptr<HalfEdge> DoublyConnectedEdgeList::find_next_half_edge(shared_ptr<HalfEdge> half_edge)
{
	return find_adjac_half_edge(true, half_edge);
}
shared_ptr<HalfEdge> DoublyConnectedEdgeList::find_prev_half_edge(shared_ptr<HalfEdge> half_edge)
{
	return find_adjac_half_edge(false, half_edge);
}

void DoublyConnectedEdgeList::connect_all_edges()
{
	list<shared_ptr<HalfEdge>> linkedWereRemoved;
	
	for (shared_ptr<HalfEdge> half_edge : unlinked_edges)
	{
		if (half_edge->is_linked()) continue;

		half_edge->next = find_next_half_edge(half_edge);
		half_edge->prev = find_prev_half_edge(half_edge);
		
		shared_ptr<Region> old_region;
		if (half_edge->next)
		{
			// If the next in already in a region, it means we are fractionning this region
			old_region = half_edge->next->region;
		}

		if (half_edge->next) half_edge->next->prev = half_edge; // Link back the next to the current (if next exists)
		if (half_edge->prev) half_edge->prev->next = half_edge; // Link back the prev to the current (if prev exists)

		if (half_edge->is_part_of_loop())
		{
			// If were are fractionning an existing region, we delete it. New regions will be created.
			if (old_region)
			{
				old_region->an_edge = nullptr;
				regions.remove(old_region);
			}

			// If the half-edge was not in a loop, but now it does, a new region is created.
			shared_ptr<Region> new_region(new Region(half_edge));
			regions.push_back(new_region);
			assign_region_to_loop(new_region, half_edge);
		}

		// Build new list without linked half-edges, to have fewer iterations next time
		if (!half_edge->is_linked())
			linkedWereRemoved.push_back(half_edge);
	}

	unlinked_edges = linkedWereRemoved;
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

vector<pair<Coord, Coord>> DoublyConnectedEdgeList::get_lines()
{
	vector<pair<Coord, Coord>> lines;
	
	// no_line_seen is only true before the first call to this method
	if (!no_line_seen)
	{
		for (shared_ptr<HalfEdge> e : edges)
			e->line_seen = false;
	}

	for (shared_ptr<HalfEdge> e : edges)
	{
		if (e->line_seen) continue;

		lines.push_back(make_pair(Coord(e->origin), Coord(e->twin->origin)));
		e->line_seen = true;
		e->twin->line_seen = true;
	}
	no_line_seen = false;

	return lines;
}

void DoublyConnectedEdgeList::close_boundaries(double left, double bottom, double right, double top)
{
	// Left
	{
		set<double> left_bound_y_positions;
		for (Vertex v : vertices)
		{
			if (v.point.x == left) left_bound_y_positions.insert(v.point.y);
		}
		shared_ptr<HalfEdge> cur_edge = begin_edge(Coord(left, bottom));
		for (double y : left_bound_y_positions)
		{
			Coord pt(left, y);
			complete_edge(cur_edge, pt);
			cur_edge = begin_edge(pt);
		}
		complete_edge(cur_edge, Coord(left, top));
	}

	// Top
	{
		set<double> top_bound_x_positions;
		for (Vertex v : vertices)
		{
			if (v.point.y == top) top_bound_x_positions.insert(v.point.x);
		}
		shared_ptr<HalfEdge> cur_edge = begin_edge(Coord(left, top));
		for (double x : top_bound_x_positions)
		{
			Coord pt(x, top);
			complete_edge(cur_edge, pt);
			cur_edge = begin_edge(pt);
		}
		complete_edge(cur_edge, Coord(right, top));
	}

	// Bottom
	{
		set<double> bottom_bound_x_positions;
		for (Vertex v : vertices)
		{
			if (v.point.y == bottom) bottom_bound_x_positions.insert(v.point.x);
		}
		shared_ptr<HalfEdge> cur_edge = begin_edge(Coord(left, bottom));
		for (double x : bottom_bound_x_positions)
		{
			Coord pt(x, bottom);
			complete_edge(cur_edge, pt);
			cur_edge = begin_edge(pt);
		}
		complete_edge(cur_edge, Coord(right, bottom));
	}

	// Right
	{
		set<double> right_bound_y_positions;
		for (Vertex v : vertices)
		{
			if (v.point.x == right) right_bound_y_positions.insert(v.point.y);
		}
		shared_ptr<HalfEdge> cur_edge = begin_edge(Coord(right, bottom));
		for (double y : right_bound_y_positions)
		{
			Coord pt(right, y);
			complete_edge(cur_edge, pt);
			cur_edge = begin_edge(pt);
		}
		complete_edge(cur_edge, Coord(right, top));
	}
}

list<shared_ptr<HalfEdge>>  DoublyConnectedEdgeList::get_half_edge_unliked()
{
	return unlinked_edges;
}