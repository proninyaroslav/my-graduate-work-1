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

#include "node.h"
#include "edge.h"

Edge::Edge(GraphId::id_t id, GraphId::id_t from_id, GraphId::id_t to_id, double weight)
{
	this->id = id;
	this->from_id = from_id;
	this->to_id = to_id;
	this->weight = weight;
}

GraphId::id_t Edge::make_edge_id()
{
	static GraphId::id_t i = 0;
	return i++;
}

bool Edge::operator==(const Edge &other) const
{
	return id == other.id;
}
