/*
 * Copyright (C) 2018 Yaroslav Pronin <proninyaroslav@mail.ru>
 *
 * This file is part of Navigation.
 *
 * Navigation is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
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

#include <vector>
#include <ctime>
#include <cstdlib>
#include <ctime>
#include <QVariant>
#include <QProgressDialog>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include <QList>
#include <QPair>
#include <QDebug>
#include "osmparser.h"
#include "utils.h"
#include "exception.h"
#include "appcontroller.h"

AppController::AppController(QObject *parent) : QObject(parent) {}

AppController::AppController(city_t city, QObject *parent) : QObject(parent)
{
	srand(time(NULL));

	this->city = city;

	QProgressDialog dialog;
	dialog.setLabelText("Загрузка графа");
	dialog.setRange(0, 0);

	QFutureWatcher<void> futureWatcher;
	QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dialog, &QProgressDialog::reset);
	futureWatcher.setFuture(QtConcurrent::run(load_graph, this));
	dialog.exec();
	futureWatcher.waitForFinished();
}

void AppController::load_graph(AppController *p_this)
{
	if (!p_this)
		return;

	try {
		OsmParser::create_graph((p_this->city == TAMBOV ? ":res/tambov.json" :
								  ":res/moscov.json"), p_this->g);
	} catch (const runtime_error e) {
		throw Exception(e.what());
	}
	p_this->node_cache = new Quadtree(p_this->g.get_nodes());
	p_this->nba_finder = new NBAStarFinder(&p_this->g, Utils::euclid_distance);
	p_this->astar_finder = new AStarFinder(&p_this->g, Utils::euclid_distance);
	p_this->dijkstra_finder = new AStarFinder(&p_this->g);
}

AppController::~AppController()
{
	delete node_cache;
	delete nba_finder;
	delete astar_finder;
	delete dijkstra_finder;
}

GeoPoint AppController::find_nearest_node(QGeoCoordinate coords, double distance_to_find)
{
	double x, y;
	Utils::deg_to_weigth(coords.longitude(), coords.latitude(), &x, &y);
	vector<GraphId::id_t> nodes_id = node_cache->get_nodes_around(g.get_nodes(), x, y, distance_to_find);
	QGeoCoordinate node_coords;
	Node node;
	if (!nodes_id.empty() && g.get_node(*nodes_id.begin(), node)) {
		double lon, lat;
		Utils::weigth_to_deg(node.x, node.y, &lon, &lat);
		node_coords.setLongitude(lon);
		node_coords.setLatitude(lat);
	} else {
		return find_nearest_node(coords, distance_to_find * 2);
	}

	return GeoPoint(node.id, node_coords);
}

QVariantList AppController::find_path(long from_id, long to_id, algorithm_t algorithm)
{
	vector<Node> nodes;
	QVariantList path;

	if (from_id == to_id)
		return path;

	AbstractFinder *finder;
	switch (algorithm) {
	case NBA_STAR:
		finder = nba_finder;
		break;
	case A_STAR:
		finder = astar_finder;
		break;
	case DIJKSTRA:
		finder = dijkstra_finder;
		break;
	}
	int find_time_msec = 0;
	try {
		clock_t start = clock(), diff;

		nodes = finder->find(from_id, to_id, NULL);

		diff = clock() - start;
		find_time_msec = diff * 1000 / CLOCKS_PER_SEC;
	} catch (const runtime_error e) {
		QMessageBox::critical(NULL, "Ошибка", e.what());
		path.append(find_time_msec);

		return path;
	}
	for (vector<Node>::iterator it = nodes.begin(); it != nodes.end(); it++) {
		double lon, lat;
		Utils::weigth_to_deg(it->x, it->y, &lon, &lat);
		path.append(QVariant::fromValue(QGeoCoordinate(lat, lon)));
	}
	path.append(find_time_msec);

	return path;
}

double AppController::calc_distance(QVariantList points)
{
	double distance = 0;
	if (points.length() < 2)
		return distance;

	for (int i = 1; i < points.length(); i++) {
		QGeoCoordinate coord1 = qvariant_cast<QGeoCoordinate>(points[i - 1]);
		QGeoCoordinate coord2 = qvariant_cast<QGeoCoordinate>(points[i]);
		distance += Utils::haversine_distance(coord1.longitude(), coord1.latitude(),
						      coord2.longitude(), coord2.latitude());
	}

	return distance;
}

void AppController::gen_traffic_jams(QVariantList coords)
{
	reset_graph();
	jams_bounds.clear();
	congestion_bounds.clear();
	foreach (QVariant v, coords) {
		QGeoCoordinate c = qvariant_cast<QGeoCoordinate>(v);
		double x, y;
		Utils::deg_to_weigth(c.longitude(), c.latitude(), &x, &y);
		jams_bounds.append(QPair<double, double>(x, y));
	}
	double half = fabs(jams_bounds[1].first - jams_bounds[0].first);
	congestion_bounds.append(QPair<double, double>(jams_bounds[0].first - half,jams_bounds[0].second + half));
	congestion_bounds.append(QPair<double, double>(jams_bounds[1].first + half, jams_bounds[1].second + half));
	congestion_bounds.append(QPair<double, double>(jams_bounds[2].first + half, jams_bounds[2].second - half));
	congestion_bounds.append(QPair<double, double>(jams_bounds[3].first - half, jams_bounds[3].second - half));

	QProgressDialog dialog;
	dialog.setLabelText("Генерация пробок");
	dialog.setRange(0, 0);

	QFutureWatcher<void> futureWatcher;
	QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dialog, &QProgressDialog::reset);
	futureWatcher.setFuture(QtConcurrent::run(gen_traffic_jams, this));
	dialog.exec();
	futureWatcher.waitForFinished();
}

bool AppController::node_in_jams_bounds(Node node)
{
	if (jams_bounds.isEmpty())
		return false;

	return (node.x >= jams_bounds[0].first &&
		node.y <= jams_bounds[0].second &&
		node.x <= jams_bounds[2].first &&
		node.y >= jams_bounds[2].second) ||
		(node.x <= jams_bounds[0].first &&
		node.y >= jams_bounds[0].second &&
		node.x >= jams_bounds[2].first &&
		node.y <= jams_bounds[2].second);
}

bool AppController::node_in_congestion_bounds(Node node)
{
	if (congestion_bounds.isEmpty())
		return false;

	return (node.x >= congestion_bounds[0].first &&
		node.y <= congestion_bounds[0].second &&
		node.x <= congestion_bounds[2].first &&
		node.y >= congestion_bounds[2].second) ||
		(node.x <= congestion_bounds[0].first &&
		node.y >= congestion_bounds[0].second &&
		node.x >= congestion_bounds[2].first &&
		node.y <= congestion_bounds[2].second);
}

void AppController::gen_traffic_jams(AppController *p_this)
{
	if (!p_this)
		return;

	p_this->g.foreach_edges(p_this, traffic_jams_callback);
}

void AppController::traffic_jams_callback(void *p_this, Edge edge)
{
	if (!p_this)
		return;

	AppController *c = (AppController*)p_this;
	QVariantList coords;
	Node from_node, to_node;
	double lon, lat;

	if (!c->g.get_node(edge.from_id, from_node) || !c->g.get_node(edge.to_id, to_node))
		return;

	int speed;
	if (c->node_in_jams_bounds(to_node))
		speed = Utils::MIN_SPEED + (rand() % (int)(Utils::TRAFFIC_JAM_THRESHOLD - Utils::MIN_SPEED + 1));
	else if (c->node_in_congestion_bounds(to_node))
		speed = Utils::TRAFFIC_JAM_THRESHOLD + 1 +
			(rand() % (int)(Utils::TRAFFIC_CONGESTION_THRESHOLD - Utils::TRAFFIC_JAM_THRESHOLD + 2));
	else
		return;

	Utils::weigth_to_deg(from_node.x, from_node.y, &lon, &lat);
	coords.append(QVariant::fromValue(QGeoCoordinate(lat, lon)));
	Utils::weigth_to_deg(to_node.x, to_node.y, &lon, &lat);
	coords.append(QVariant::fromValue(QGeoCoordinate(lat, lon)));

	bool oneway = true;
	for (vector<Edge>::iterator it = to_node.edges.begin(); it != to_node.edges.end(); it++)
		if (edge.from_id == it->to_id)
			oneway = false;

	edge.weight = Utils::calc_weight(speed, edge.weight);
	c->g.update_edge(edge);
}

void AppController::reset_graph()
{
	QProgressDialog dialog;
	dialog.setLabelText("Генерация графа");
	dialog.setRange(0, 0);

	QFutureWatcher<void> futureWatcher;
	QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dialog, &QProgressDialog::reset);
	futureWatcher.setFuture(QtConcurrent::run(reset_graph, this));
	dialog.exec();
	futureWatcher.waitForFinished();
}

void AppController::reset_graph(AppController *p_this)
{
	if (!p_this)
		return;

	p_this->g.foreach_edges(p_this, reset_graph_callback);
}

void AppController::reset_graph_callback(void *p_this, Edge edge)
{
	if (!p_this)
		return;

	AppController *c = (AppController*)p_this;
	Node from_node, to_node;

	if (!c->g.get_node(edge.from_id, from_node) || !c->g.get_node(edge.to_id, to_node))
		return;

	edge.weight = Utils::euclid_distance(from_node, to_node);
	c->g.update_edge(edge);
}

void AppController::test_algorithms(int n)
{
	map<GraphId::id_t, Node> nodes = g.get_nodes();
	QList<GraphId::id_t> ids;
	AbstractFinder* finders[3] = {nba_finder, astar_finder, dijkstra_finder};

	for (map<GraphId::id_t, Node>::iterator it = nodes.begin(); it != nodes.end(); it++)
		ids.append(it->first);

	printf("Узлов: %d; Рёбер: %d\n", g.get_num_nodes(), g.get_num_edges());
	printf("Дистанция | NBA*, мс | A*, мс | Дейкстра, мс | NBA*, узл. | A*, узл. | Дейкстра, узл.\n");
	for (int i = 0; i < n; i++) {
		int from, to;
		size_t num_visited[3] = {0, 0, 0};
		while (true) {
			from = rand() % (int)(ids.size() + 1);
			to = rand() % (int)(ids.size() + 1);
			if (from != to)
				break;
		}
		GraphId::id_t from_id = ids[from];
		GraphId::id_t to_id = ids[to];

		for (int j = 0; j < 3; j++) {
			vector<Node> path_nodes;
			int find_time_msec = 0;
			try {
				clock_t start = clock(), diff;

				path_nodes = finders[j]->find(from_id, to_id, &num_visited[j]);

				diff = clock() - start;
				find_time_msec = diff * 1000 / CLOCKS_PER_SEC;
			} catch (const runtime_error e) {
				qDebug() << "Ошибка:" << e.what();
				continue;
			}

			if (j == 0) {
				double distance = 0;
				for (int k = 1; k < path_nodes.size(); k++) {
					double lon1, lat1, lon2, lat2;
					Utils::weigth_to_deg(path_nodes[k - 1].x, path_nodes[k - 1].y, &lon1, &lat1);
					Utils::weigth_to_deg(path_nodes[k].x, path_nodes[k].y, &lon2, &lat2);
					distance += Utils::haversine_distance(lon1, lat1, lon2, lat2);
				}
				printf("%.2f\t%d", distance, find_time_msec);
			} else {
				printf("\t%d", find_time_msec);
			}
		}
		printf("\t%zu\t%zu\t%zu\n", num_visited[0], num_visited[1], num_visited[2]);
		fflush(stdout);
	}
}
