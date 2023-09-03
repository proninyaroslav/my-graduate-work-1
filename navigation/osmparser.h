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

#ifndef OSMPARSER_H
#define OSMPARSER_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include "graph/graph.h"

using namespace std;

class OsmParser
{
private:
	static QJsonDocument get_json_doc(QString path);
	static bool is_oneway(QJsonObject elem);
public:
	static void create_graph(QString path, Graph &g);
};

#endif // OSMPARSER_H
