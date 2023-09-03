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

#include "astarfinder.h"
#include <sstream>

AStarFinder::AStarFinder(Graph *g, AbstractFinder::heuristic_func heuristic) :
	AbstractFinder(g, heuristic)
{
	came_from = NULL;
}

vector<Node> AStarFinder::find(GraphId::id_t from_id, GraphId::id_t to_id, size_t *num_visited)
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

	vector<Node> path;
	AStarState *start_state = new AStarState(from_node);
	start_state->g = 0;
	start_state->f = heuristic(from_node, to_node);
	start_state->open = true;
	states[from_node.id] = start_state;
	open_set.push(start_state);

	while (open_set.size()) {
		came_from = (AStarState *)open_set.pop();
		if (came_from->node.id == to_node.id) {
			path = make_path(came_from);
			break;
		}

		came_from->visited = true;
		this->num_visited++;
		g->foreach_linked_nodes(this, visit, came_from->node.id, true);
	}
	if (num_visited)
		*num_visited = this->num_visited;
	free();

	return path;
}

double AStarFinder::cmp_f(AbstractState *a, AbstractState *b)
{
	return ((AStarState *)a)->f - ((AStarState *)b)->f;
}

void AStarFinder::visit(void *p_this, Node node, Edge edge)
{
	if (!p_this)
		return;

	 AStarFinder *finder = (AStarFinder *)p_this;

	 map<GraphId::id_t, AStarState *>::iterator it = finder->states.find(node.id);
	 AStarState *state = NULL;
	 if (it == finder->states.end()) {
		 state = new AStarState(node);
		 finder->states[node.id] = state;
	 } else {
		 state = it->second;
	 }

	 if (state->visited)
		 return;
	 if (!state->open) {
		 state->open = true;
		 finder->open_set.push(state);
	 }

	 double tentative_distance = finder->came_from->g + edge.weight;
	 if (tentative_distance < state->g) {
		 state->g = tentative_distance;
		 state->f = tentative_distance + finder->heuristic(state->node, finder->to_node);
		 state->parent = finder->came_from;
		 finder->open_set.push(state);
	 }
}

vector<Node> AStarFinder::make_path(AStarState *state)
{
	vector<Node> path;
	if (!state)
		return path;

	path.push_back(state->node);

	AStarState *parent = state->parent;
	while (parent) {
		path.push_back(parent->node);
		parent = parent->parent;
	}

	return path;
}

void AStarFinder::free()
{
	num_visited = 0;
	open_set.clear();
	came_from = NULL;
	for (map<GraphId::id_t, AStarState *>::iterator it = states.begin(); it != states.end(); it++)
		delete it->second;
	states.clear();
}
