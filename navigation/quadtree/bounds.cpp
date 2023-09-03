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

#include "bounds.h"

Bounds::Bounds(double x, double y, double half)
{
	this->x = x;
	this->y = y;
	this->half = half;
}

double Bounds::left()
{
	return x - half;
}

double Bounds::top()
{
	return y - half;
}

double Bounds::center_x()
{
	return x;
}

double Bounds::center_y()
{
	return y;
}

bool Bounds::contains(double x, double y)
{
	return this->x - half <= x && x < this->x + half &&
	       this->y - half <= y && y < this->y + half;
}

double Bounds::height()
{
	return half * 2;
}

double Bounds::width()
{
	return half * 2;
}
