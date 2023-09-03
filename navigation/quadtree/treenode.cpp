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

#include <cstddef>
#include "treenode.h"

TreeNode::TreeNode()
{
	nw = NULL;
	ne = NULL;
	sw = NULL;
	se = NULL;
}

TreeNode::~TreeNode()
{
	free();
}

TreeNode::TreeNode(Bounds bounds) : TreeNode()
{
	this->bounds = bounds;
}

void TreeNode::insert(GraphId::id_t node_id, map<GraphId::id_t, Node> &nodes, int depth)
{
	bool is_leaf = !nw;
	if (is_leaf) {
		ids.push_back(node_id);
		if (bounds.half > 0.01) {
			if (ids.size() >= MAX_NODES) {
				subdivide();
				for (vector<GraphId::id_t>::iterator it = ids.begin(); it != ids.end(); it++)
					insert(*it, nodes, depth + 1);
				ids.clear();
			}
		}
	} else {
		ChildId child_id = NW;
		if (nodes[node_id].x > bounds.x)
			child_id = NE;
		if (nodes[node_id].y > bounds.y) {
			if (child_id == NW)
				child_id = SW;
			else
				child_id = SE;
		}
		TreeNode *child = get_child(this, child_id);
		if (child)
			child->insert(node_id, nodes, depth + 1);
	}
}

void TreeNode::subdivide()
{
	double quarter = bounds.half / 2;
	free();
	nw = new TreeNode(Bounds(bounds.x - quarter, bounds.y - quarter, quarter));
	ne = new TreeNode(Bounds(bounds.x + quarter, bounds.y - quarter, quarter));
	sw = new TreeNode(Bounds(bounds.x - quarter, bounds.y + quarter, quarter));
	se = new TreeNode(Bounds(bounds.x + quarter, bounds.y + quarter, quarter));
}

void TreeNode::query(Bounds bounds, vector<GraphId::id_t> &result_ids, map<GraphId::id_t, Node> &nodes)
{
	if (!intersects(this->bounds, bounds))
		return;

	if (!ids.empty()) {
		for (vector<GraphId::id_t>::iterator it = ids.begin(); it != ids.end(); it++) {
			GraphId::id_t id = *it;
			if (bounds.contains(nodes[id].x, nodes[id].y))
				result_ids.push_back(id);
		}
	}

	if (!nw)
		return;

	nw->query(bounds, result_ids, nodes);
	ne->query(bounds, result_ids, nodes);
	sw->query(bounds, result_ids, nodes);
	se->query(bounds, result_ids, nodes);
}

bool TreeNode::intersects(Bounds a, Bounds b)
{
	return a.x - a.half < b.x + b.half &&
	       a.x + a.half > b.x - b.half &&
	       a.y - a.half < b.y + b.half &&
	       a.y + a.half > b.y - b.half;
}

void TreeNode::traversal(TreeNode::treenode_callback callback)
{
	if (!callback)
		return;

	callback(this);
	if (nw) {
		nw->traversal(callback);
		ne->traversal(callback);
		sw->traversal(callback);
		se->traversal(callback);
	}
}

TreeNode *TreeNode::get_child(TreeNode *node, TreeNode::ChildId id)
{
	switch (id) {
	case NW:
		return node->nw;
	case NE:
		return node->ne;
	case SW:
		return node->sw;
	case SE:
		return node->se;
	}

	return NULL;
}

void TreeNode::free()
{
	delete nw;
	delete ne;
	delete sw;
	delete se;
}
