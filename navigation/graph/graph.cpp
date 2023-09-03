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

#include "utils.h"
#include "graph.h"

Graph::Graph() {}

Node Graph::put_node(GraphId::id_t node_id, double x, double y)
{
	return nodes[node_id] = Node(node_id, x, y);
}

bool Graph::get_node(GraphId::id_t node_id, Node &node)
{
	map<GraphId::id_t, Node>::iterator it = nodes.find(node_id);
	if (it == nodes.end())
		return false;
	node = it->second;

	return true;
}

bool Graph::remove_node(GraphId::id_t node_id)
{
	Node node;
	if (!get_node(node_id, node))
		return false;

	for (vector<Edge>::iterator it = node.edges.begin(); it != node.edges.end(); it++)
		remove_edge(*it);
	nodes.erase(node_id);

	return true;
}

Edge Graph::add_edge(Node from_node, Node to_node, double weight)
{
	GraphId::id_t edge_id = Edge::make_edge_id();
	Edge edge(edge_id, from_node.id, to_node.id, weight);
	edges[edge.id] = edge;

	from_node.edges.push_back(edge);
	nodes[from_node.id] = from_node;
	if (from_node.id != to_node.id) {
		to_node.edges.push_back(edge);
		nodes[to_node.id] = to_node;
	}

	return edge;
}

Edge Graph::add_edge(GraphId::id_t from_id, GraphId::id_t to_id, double weight)
{
	Node from_node;
	Node to_node;
	if (!get_node(from_id, from_node))
		from_node = put_node(from_id);
	if (!get_node(to_id, to_node))
		to_node = put_node(to_id);

	return add_edge(from_node, to_node, weight);
}

bool Graph::get_edge(GraphId::id_t from_id, GraphId::id_t to_id, Edge &edge)
{
	Node node;
	if (!get_node(from_id, node) || node.edges.empty())
		return false;

	for (vector<Edge>::iterator it = node.edges.begin(); it != node.edges.end(); it++) {
		edge = *it;
		if (edge.from_id == from_id && edge.to_id == to_id)
			return true;
	}

	return false;
}

vector<Edge> Graph::get_edges(GraphId::id_t node_id)
{
	Node node;

	return (get_node(node_id, node) ? node.edges : vector<Edge>());
}

bool Graph::remove_edge(Edge edge)
{
	Node from_node;
	Node to_node;

	edges.erase(edge.id);
	if (get_node(edge.from_id, from_node)) {
		int pos = from_node.get_edge_pos(edge);
		if (pos >= 0) {
			from_node.edges.erase(from_node.edges.begin() + pos);
			nodes[edge.from_id] = from_node;
		}
	}
	if (get_node(edge.to_id, to_node)) {
		int pos = to_node.get_edge_pos(edge);
		if (pos >= 0) {
			to_node.edges.erase(to_node.edges.begin() + pos);
			nodes[edge.to_id] = to_node;
		}
	}

	return true;
}

bool Graph::update_edge(Edge edge)
{
	if (edges.find(edge.id) == edges.end())
		return false;

	Node from_node;
	Node to_node;

	edges[edge.id] = edge;
	if (!get_node(edge.from_id, from_node) || !get_node(edge.to_id, to_node))
		return false;
	int pos_from = from_node.get_edge_pos(edge);
	int pos_to = to_node.get_edge_pos(edge);
	if (pos_from < 0 || pos_to < 0)
		return false;
	from_node.edges[pos_from] = edge;
	nodes[edge.from_id] = from_node;
	to_node.edges[pos_to] = edge;
	nodes[edge.to_id] = to_node;


	return true;
}

void Graph::clear()
{
	nodes.clear();
	edges.clear();
}

GraphId::id_t Graph::get_num_nodes()
{
	return nodes.size();
}

GraphId::id_t Graph::get_num_edges()
{
	return edges.size();
}

void Graph::foreach_nodes(void *p_this, nodes_callback callback)
{
	if (!callback)
		return;

	for (map<GraphId::id_t, Node>::iterator it = nodes.begin(); it != nodes.end(); it++)
		callback(p_this, it->second);
}

void Graph::foreach_edges(void *p_this, edges_callback callback)
{
	if (!callback)
		return;

	for (map<GraphId::id_t, Edge>::iterator it = edges.begin(); it != edges.end(); it++)
		callback(p_this, it->second);
}

void Graph::foreach_linked_nodes(void *p_this, linked_nodes_callback callback,
				 GraphId::id_t node_id, bool oriented)
{
	if (!callback || node_id == GraphId::UNDEF_ID)
		return;

	Node node;
	if (!get_node(node_id, node) || node.edges.empty())
		return;
	if (oriented)
		foreach_oriented_edges(p_this, callback, node_id, node.edges);
	else
		foreach_non_oriented_edges(p_this, callback, node_id, node.edges);
}

map<GraphId::id_t, Node> &Graph::get_nodes()
{
	return nodes;
}

map<GraphId::id_t, Edge> &Graph::get_edges()
{
	return edges;
}

void Graph::foreach_non_oriented_edges(void *p_this, linked_nodes_callback callback,
				       GraphId::id_t node_id, vector<Edge> &edges)
{
	for (vector<Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
		Edge edge = *it;
		GraphId::id_t linked_node_id = (edge.from_id == node_id ? edge.to_id : edge.from_id);
		callback(p_this, nodes[linked_node_id], edge);
	}
}

void Graph::foreach_oriented_edges(void *p_this, linked_nodes_callback callback,
				   GraphId::id_t node_id, vector<Edge> &edges)
{
	for (vector<Edge>::iterator it = edges.begin(); it != edges.end(); it++) {
		Edge edge = *it;
		if (edge.from_id == node_id)
			callback(p_this, nodes[edge.to_id], edge);
	}
}
