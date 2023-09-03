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

#ifndef ASTARFINDER_H
#define ASTARFINDER_H

#include "abstractfinder.h"
#include "astarstate.h"
#include "nodequeue.h"

using namespace std;

class AStarFinder : public AbstractFinder
{
public:
	AStarFinder(Graph *g, AbstractFinder::heuristic_func heuristic = default_heuristic);
	vector<Node> find(GraphId::id_t from_id, GraphId::id_t to_id,
			  size_t *num_visited);
	static double cmp_f(AbstractState *a, AbstractState *b);

private:
	map<GraphId::id_t, AStarState *> states;
	NodeQueue open_set = NodeQueue(cmp_f);
	AStarState *came_from;

	static void visit(void *p_this, Node node, Edge edge);
	vector<Node> make_path(AStarState *state);
	void free();
};

#endif // ASTARFINDER_H
