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

#ifndef QUADTREE_H
#define QUADTREE_H

#include <map>
#include <vector>
#include "treenode.h"
#include "bounds.h"
#include "graph/node.h"

using namespace std;

class Quadtree
{
private:
	TreeNode *root;
public:
	Quadtree(map<GraphId::id_t, Node> &nodes);
	vector<GraphId::id_t> get_nodes_around(map<GraphId::id_t, Node> nodes, double x,
				      double y, double half);
	void traversal(TreeNode::treenode_callback callback);
	Bounds get_bounds();
};

#endif // QUADTREE_H
