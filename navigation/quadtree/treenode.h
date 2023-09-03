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

#ifndef TREENODE_H
#define TREENODE_H

#include <vector>
#include <map>
#include "graph/node.h"
#include "bounds.h"

using namespace std;

class TreeNode
{
public:
	enum ChildId {NW, NE, SW, SE};
	typedef void (*treenode_callback)(TreeNode *);

	Bounds bounds;

	TreeNode();
	~TreeNode();
	TreeNode(Bounds bounds);
	void insert(GraphId::id_t node_id, map<GraphId::id_t, Node> &nodes, int depth = 0);
	void subdivide();
	void query(Bounds bounds, vector<GraphId::id_t> &result_ids, map<GraphId::id_t, Node> &nodes);
	bool intersects(Bounds a, Bounds b);
	void traversal(treenode_callback callback);
	static TreeNode *get_child(TreeNode *node, ChildId id);

private:
	static const int MAX_NODES = 4;

	vector<GraphId::id_t> ids;
	TreeNode *nw, *ne, *sw, *se;

	void free();
};

#endif // TREENODE_H
