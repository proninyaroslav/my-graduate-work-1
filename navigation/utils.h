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

#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include "graph/node.h"

class Utils
{
public:
	#define RADIAN(angle) ((angle) * M_PI / 180.0)

	static constexpr double EARTH_RADIUS = 20037508.34;
	static constexpr double EARTH_RADIUS_KM = 6371;
	static constexpr int weigth_scale = 10000;
	static constexpr int MAX_SPEED = 45;
	static constexpr int MIN_SPEED = 1;
	static constexpr int AVERAGE_SPEED = MAX_SPEED;
	/* Верхнее значение скорости потока для затора*/
	static constexpr int TRAFFIC_CONGESTION_THRESHOLD = 20;
	/* Верхнее значение скорости потока для пробки*/
	static constexpr int TRAFFIC_JAM_THRESHOLD = 10;
	/* Верхнее значение скорости потока для перекрытого движения*/
	static constexpr int BLOCKED_TRAFFIC_THRESHOLD = 5;

	static void deg_to_weigth(const double lon, const double lat,
				  double *x, double *y);
	static void weigth_to_deg(const double x, const double y, double *lon, double *lat);
	static double euclid_distance(Node a, Node b);
	static double haversine_distance(double lon1, double lat1, double lon2, double lat2);
	static int get_rand_speed();
	static double calc_weight(int speed, double distance);
};

#endif // UTILS_H
