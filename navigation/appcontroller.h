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

#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QGeoCoordinate>
#include <QVariantList>
#include <QList>
#include <QPair>
#include "graph/graph.h"
#include "quadtree/quadtree.h"
#include "geopoint.h"
#include "finder/astarfinder.h"
#include "finder/nbastarfinder.h"

class AppController : public QObject
{
	Q_OBJECT

private:
	Graph g;
	Quadtree *node_cache;
	NBAStarFinder *nba_finder;
	AStarFinder *astar_finder;
	AStarFinder *dijkstra_finder;
	QList<QPair<double, double>> jams_bounds;
	QList<QPair<double, double>> congestion_bounds;

	bool node_in_jams_bounds(Node node);
	bool node_in_congestion_bounds(Node node);
	static void load_graph(AppController *p_this);
	static void gen_traffic_jams(AppController *p_this);
	static void traffic_jams_callback(void* p_this, Edge edge);
	static void reset_graph(AppController *p_this);
	static void reset_graph_callback(void* p_this, Edge edge);
public:
	enum city_t {TAMBOV, MOSCOW};
	Q_ENUM(city_t)
	enum algorithm_t {NBA_STAR, A_STAR, DIJKSTRA};
	Q_ENUM(algorithm_t)

	city_t city;
	Q_PROPERTY(city_t city MEMBER city CONSTANT)

	explicit AppController(QObject *parent = nullptr);
	explicit AppController(city_t city, QObject *parent = nullptr);
	~AppController();
	Q_INVOKABLE GeoPoint find_nearest_node(QGeoCoordinate coords, double distance_to_find = 1);
	Q_INVOKABLE QVariantList find_path(long from_id, long to_id, algorithm_t algorithm);
	Q_INVOKABLE double calc_distance(QVariantList points);
	Q_INVOKABLE void gen_traffic_jams(QVariantList coords);
	Q_INVOKABLE void reset_graph();
	void test_algorithms(int n);
};

#endif // APPCONTROLLER_H
