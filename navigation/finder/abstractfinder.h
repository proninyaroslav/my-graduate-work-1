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

#ifndef ABSTRACTFINDER_H
#define ABSTRACTFINDER_H

#include <map>
#include <vector>
#include <cctype>
#include "graph/graph.h"

class AbstractFinder
{
public:
	typedef double (*heuristic_func)(Node a, Node b);

	AbstractFinder(Graph *g, heuristic_func heuristic);
	virtual std::vector<Node> find(GraphId::id_t from_id, GraphId::id_t to_id,
				       size_t *num_visited) = 0;
	static double default_heuristic(Node a, Node b);

protected:
	Graph *g;
	heuristic_func heuristic;
	Node from_node, to_node;
	size_t num_visited;
	virtual void free() = 0;
};

#endif // ABSTRACTFINDER_H
