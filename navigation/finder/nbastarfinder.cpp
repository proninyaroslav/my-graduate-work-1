/*
 * Copyright (C) 2018 Yaroslav Pronin <proninyaroslav@mail.ru>
 *
 * This file is part of libnavigation.
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LibreTorrent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LibreTorrent.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdexcept>
#include <sstream>
#include <cfloat>
#include <QDebug>
#include "nbastarfinder.h"

NBAStarFinder::NBAStarFinder(Graph *g, AbstractFinder::heuristic_func heuristic) :
	AbstractFinder(g, heuristic)
{
	came_from = NULL;
	min_node_state = NULL;
	min_len = INFINITY;
	f_forward = INFINITY;
	f_backward = INFINITY;
}

vector<Node> NBAStarFinder::find(GraphId::id_t from_id, GraphId::id_t to_id, size_t *num_visited)
{
	if (!g->get_node(from_id, from_node)) {
		stringstream ss;
		ss << "Начальный узел пути id=" << from_id << " не определён в графе";
		throw runtime_error(ss.str());
	}
	if (!g->get_node(to_id, to_node)) {
		stringstream ss;
		ss << "Конечный узел пути " << to_id << " не определён в графе";
		throw runtime_error(ss.str());
	}

	NBAStarState *start_state, *end_state;

	start_state = new NBAStarState(from_node);
	start_state->g_forward = 0;
	f_forward = heuristic(from_node, to_node);
	start_state->f_forward = f_forward;
	states[from_node.id] = start_state;
	open_forward_set.push(start_state);

	end_state = new NBAStarState(to_node);
	end_state->g_backward = 0;
	f_backward = f_forward;
	end_state->f_backward = f_backward;
	states[to_node.id] = end_state;
	open_backward_set.push(end_state);

	while (open_forward_set.size() && open_backward_set.size()) {
		if (open_forward_set.size() < open_backward_set.size())
			forward_search();
		else
			backward_search();
	}
	vector<Node> path = make_path(min_node_state);
	if (num_visited)
		*num_visited = this->num_visited;
	free();

	return path;
}

double NBAStarFinder::cmp_f_forward(AbstractState *a, AbstractState *b)
{
	return ((NBAStarState *)a)->f_forward - ((NBAStarState *)b)->f_forward;
}

double NBAStarFinder::cmp_f_backward(AbstractState *a, AbstractState *b)
{
	return ((NBAStarState *)a)->f_backward - ((NBAStarState *)b)->f_backward;
}

void NBAStarFinder::forward_search()
{
	came_from = (NBAStarState *)open_forward_set.pop();
	if (came_from->visited)
		return;
	came_from->visited = true;
	num_visited++;

	double heuristic_dist = heuristic(from_node, came_from->node);
	if (came_from->f_forward < min_len && (came_from->g_forward + f_backward - heuristic_dist) < min_len)
		g->foreach_linked_nodes(this, visit_forward_oriented, came_from->node.id);

	if (open_forward_set.size() > 0)
		f_forward = ((NBAStarState *)open_forward_set.peek())->f_forward;
}

void NBAStarFinder::backward_search()
{
	came_from = (NBAStarState *)open_backward_set.pop();
	if (came_from->visited)
		return;
	came_from->visited = true;
	num_visited++;

	double heuristic_dist = heuristic(came_from->node, to_node);
	if (came_from->f_backward < min_len && (came_from->g_backward + f_forward - heuristic_dist) < min_len) {
		g->foreach_linked_nodes(this, visit_backward_oriented, came_from->node.id);
	}

	if (open_backward_set.size() > 0)
		f_backward = ((NBAStarState *)open_backward_set.peek())->f_backward;
}

void NBAStarFinder::visit_forward_oriented(void *p_this, Node node, Edge edge)
{
	if (!p_this)
		return;

	NBAStarFinder *finder = (NBAStarFinder *)p_this;
	if (edge.from_id == finder->came_from->node.id)
		finder->visit_forward(node, edge);
}

void NBAStarFinder::visit_backward_oriented(void *p_this, Node node, Edge edge)
{
	if (!p_this)
		return;

	NBAStarFinder *finder = (NBAStarFinder *)p_this;
	if (edge.to_id == finder->came_from->node.id)
		finder->visit_backward(node, edge);
}

void NBAStarFinder::visit_forward(Node node, Edge edge)
{
	map<GraphId::id_t, NBAStarState *>::iterator it = states.find(node.id);
	NBAStarState *state = NULL;
	if (it == states.end()) {
		state = new NBAStarState(node);
		states[node.id] = state;
	} else {
		state = it->second;
	}
	if (state->visited)
		return;

	double tentative_distance = came_from->g_forward + edge.weight;
	if (tentative_distance < state->g_forward) {
		state->g_forward = tentative_distance;
		state->f_forward = tentative_distance + heuristic(state->node, to_node);
		state->parent_forward = came_from;
		open_forward_set.push(state);
	}
	double potential_min = state->g_forward + state->g_backward;
	if (potential_min < min_len) {
		min_len = potential_min;
		min_node_state = state;
	}
}

void NBAStarFinder::visit_backward(Node node, Edge edge)
{
	map<GraphId::id_t, NBAStarState *>::iterator it = states.find(node.id);
	NBAStarState *state = NULL;
	if (it == states.end()) {
		state = new NBAStarState(node);
		states[node.id] = state;
	} else {
		state = it->second;
	}
	if (state->visited)
		return;

	double tentative_distance = came_from->g_backward + edge.weight;
	if (tentative_distance < state->g_backward) {
		state->g_backward = tentative_distance;
		state->f_backward = tentative_distance + heuristic(from_node, state->node);
		state->parent_backward = came_from;
		open_backward_set.push(state);
	}
	double potential_min = state->g_forward + state->g_backward;
	if (potential_min < min_len) {
		min_len = potential_min;
		min_node_state = state;
	}
}

vector<Node> NBAStarFinder::make_path(NBAStarState *state)
{
	vector<Node> path;
	if (!state)
		return path;

	path.push_back(state->node);

	NBAStarState *parent = state->parent_forward;
	while (parent) {
		path.push_back(parent->node);
		parent = parent->parent_forward;
	}

	NBAStarState *child = state->parent_backward;
	while (child) {
		path.insert(path.begin(), child->node);
		child = child->parent_backward;
	}

	return path;
}

void NBAStarFinder::free()
{
	num_visited = 0;
	open_forward_set.clear();
	open_backward_set.clear();
	min_node_state = NULL;
	came_from = NULL;
	min_len = INFINITY;
	f_forward = INFINITY;
	f_backward = INFINITY;
	for (map<GraphId::id_t, NBAStarState *>::iterator it = states.begin(); it != states.end(); it++)
		delete it->second;
	states.clear();
}
