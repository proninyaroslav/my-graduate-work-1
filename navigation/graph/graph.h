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

#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <vector>
#include "node.h"
#include "edge.h"

using namespace std;

class Graph
{
public:
	typedef void (*nodes_callback)(void*, Node);
	typedef void (*edges_callback)(void*, Edge);
	typedef void (*linked_nodes_callback)(void*, Node, Edge);

	Graph();
	Node put_node(GraphId::id_t node_id, double x = Node::UNDEF_DATA, double y = Node::UNDEF_DATA);
	bool get_node(GraphId::id_t node_id, Node &node);
	bool remove_node(GraphId::id_t node_id);
	Edge add_edge(Node from_node, Node to_node, double weight = Edge::UNDEF_WEIGHT);
	Edge add_edge(GraphId::id_t from_id, GraphId::id_t to_id, double weight = Edge::UNDEF_WEIGHT);
	bool get_edge(GraphId::id_t from_id, GraphId::id_t to_id, Edge &edge);
	vector<Edge> get_edges(GraphId::id_t node_id);
	bool remove_edge(Edge edge);
	bool update_edge(Edge edge);
	void clear();
	GraphId::id_t get_num_nodes();
	GraphId::id_t get_num_edges();
	void foreach_nodes(void *p_this, nodes_callback callback);
	void foreach_edges(void *p_this, edges_callback callback);
	void foreach_linked_nodes(void *p_this, linked_nodes_callback callback,
				  GraphId::id_t node_id, bool oriented = false);
	map<GraphId::id_t, Node> &get_nodes();
	map<GraphId::id_t, Edge> &get_edges();

private:
	map<GraphId::id_t, Node> nodes;
	map<GraphId::id_t, Edge> edges;

	ptrdiff_t get_edges_pos(vector<Edge> &edges, Edge edge);
	void foreach_non_oriented_edges(void *p_this, linked_nodes_callback callback,
					GraphId::id_t node_id, vector<Edge> &edges);
	void foreach_oriented_edges(void *p_this, linked_nodes_callback callback,
				    GraphId::id_t node_id, vector<Edge> &edges);

};

#endif // GRAPH_H
