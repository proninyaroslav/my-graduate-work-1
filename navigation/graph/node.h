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

#ifndef NODE_H
#define NODE_H

#include <vector>
#include <cfloat>
#include <cmath>
#include <cstddef>
#include "edge.h"
#include "id.h"

using namespace std;

class Node
{
public:
	static constexpr double UNDEF_DATA = INFINITY;

	GraphId::id_t id;
	vector<Edge> edges;
	double x;
	double y;

	Node(GraphId::id_t id = -1, double x = UNDEF_DATA, double y = UNDEF_DATA);

	ptrdiff_t get_edge_pos(Edge edge);

};

#endif // NODE_H
