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

#include <cfloat>
#include <algorithm>
#include "node.h"

Node::Node(GraphId::id_t id, double x, double y)
{
	this->id = id;
	this->x = x;
	this->y = y;
}

ptrdiff_t Node::get_edge_pos(Edge edge)
{
	ptrdiff_t pos = find(edges.begin(), edges.end(), edge) - edges.begin();

	return (pos < edges.size() ? pos : -1);
}
