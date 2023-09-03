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

#include <cstdlib>
#include "utils.h"

void Utils::deg_to_weigth(const double lon, const double lat, double *x, double *y)
{
	*x = lon * weigth_scale;
	*y = lat * weigth_scale;
}

void Utils::weigth_to_deg(const double x, const double y, double *lon, double *lat)
{
	*lon = x / weigth_scale;
	*lat = y / weigth_scale;
}

double Utils::euclid_distance(Node a, Node b)
{
	double dx = a.x - b.x;
	double dy = a.y - b.y;

	return sqrt(dx * dx + dy * dy);
}

double Utils::haversine_distance(double lon1, double lat1, double lon2, double lat2)
{
	lon1 = RADIAN(lon1);
	lat1 = RADIAN(lat1);
	lon2 = RADIAN(lon2);
	lat2 = RADIAN(lat2);

	double dlon = lon2 - lon1;
	double dlat = lat2 - lat1;
	double sin_dlat = sin(dlat / 2);
	double sin_dlon = sin(dlon / 2);
	double a = (sin_dlat * sin_dlat) + cos(lat1) * cos(lat2) * (sin_dlon * sin_dlon);
	double c = 2 * asin(sqrt(a));

	return c * EARTH_RADIUS_KM;
}

int Utils::get_rand_speed()
{
	float r = (float)rand() / (RAND_MAX + 1.0) * MAX_SPEED;

	return round(log(1 + r) * 14.71);
}

double Utils::calc_weight(int speed, double distance)
{
	if (speed > TRAFFIC_CONGESTION_THRESHOLD || speed <= 0)
		return distance;

	return (double)MAX_SPEED / (double)speed * distance;
}
