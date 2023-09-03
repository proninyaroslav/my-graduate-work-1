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

#include <cmath>
#include <algorithm>
#include "quadtree.h"

Quadtree::Quadtree(map<GraphId::id_t, Node> &nodes)
{
	if (nodes.empty()) {
		root = new TreeNode(Bounds());
	} else {
		double min_x = INFINITY;
		double min_y = INFINITY;
		double max_x = -INFINITY;
		double max_y = -INFINITY;
		for (map<GraphId::id_t, Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			Node node = it->second;
			if (node.x < min_x)
				min_x = node.x;
			if (node.x > max_x)
				max_x = node.x;
			if (node.y < min_y)
				min_y = node.y;
			if (node.y > max_y)
				max_y = node.y;
		}
		double side = max(max_x - min_x, max_y - min_y);
		side += 2;
		--min_x;
		--min_y;
		double half = side / 2;
		root = new TreeNode(Bounds(min_x + half, min_y + half, half));
	}
	for (map<GraphId::id_t, Node>::iterator it = nodes.begin(); it != nodes.end(); it++)
		root->insert(it->first, nodes);
}

vector<GraphId::id_t> Quadtree::get_nodes_around(map<GraphId::id_t, Node> nodes, double x,
						 double y, double half)
{
	vector<GraphId::id_t> ids;
	root->query(Bounds(x, y, half), ids, nodes);

	return ids;
}

void Quadtree::traversal(TreeNode::treenode_callback callback)
{
	root->traversal(callback);
}

Bounds Quadtree::get_bounds()
{
	return root->bounds;
}
