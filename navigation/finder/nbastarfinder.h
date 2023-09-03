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

#ifndef NBASTARFINDER_H
#define NBASTARFINDER_H

#include "abstractfinder.h"
#include "nbastarstate.h"
#include "nodequeue.h"

using namespace std;

class NBAStarFinder : public AbstractFinder
{
public:
	NBAStarFinder(Graph *g, AbstractFinder::heuristic_func heuristic);
	vector<Node> find(GraphId::id_t from_id, GraphId::id_t to_id,
			  size_t *num_visited);
	static double cmp_f_forward(AbstractState *a, AbstractState *b);
	static double cmp_f_backward(AbstractState *a, AbstractState *b);

private:
	map<GraphId::id_t, NBAStarState *> states;
	NodeQueue open_forward_set = NodeQueue(cmp_f_forward);
	NodeQueue open_backward_set = NodeQueue(cmp_f_backward);
	NBAStarState *min_node_state;
	NBAStarState *came_from;
	double f_forward, f_backward, min_len;

	void forward_search();
	void backward_search();
	static void visit_forward_oriented(void *p_this, Node node, Edge edge);
	static void visit_backward_oriented(void *p_this, Node node, Edge edge);
	void visit_forward(Node node, Edge edge);
	void visit_backward(Node node, Edge edge);
	std::vector<Node> make_path(NBAStarState *state);
	void free();
};

#endif // NBASTARFINDER_H
