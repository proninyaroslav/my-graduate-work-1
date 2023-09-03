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

#include <stdexcept>
#include <QFile>
#include <QJsonArray>
#include <QDebug>
#include "utils.h"
#include "osmparser.h"

void OsmParser::create_graph(QString path, Graph &g)
{
	if (path.isNull() || path.isEmpty())
		throw runtime_error("Неверный путь: пустая строка");

	QJsonDocument json_doc = get_json_doc(path);
	QJsonObject root = json_doc.object();
	QJsonArray elements = root["elements"].toArray();

	foreach (const QJsonValue &val, elements) {
		QJsonObject elem = val.toObject();
		QString type = elem["type"].toString();
		if (type == "node") {
			double lon = elem["lon"].toDouble();
			double lat = elem["lat"].toDouble();
			double x, y;
			Utils::deg_to_weigth(lon, lat, &x, &y);
			g.put_node(elem["id"].toDouble(), x, y);
		} else if (type == "way") {
			QJsonArray nodes = elem["nodes"].toArray();
			for (int i = 1; i < nodes.size(); i++) {
				GraphId::id_t from_id = nodes.at(i - 1).toDouble();
				GraphId::id_t to_id = nodes.at(i).toDouble();
				Node from_node;
				Node to_node;
				if (!g.get_node(from_id, from_node))
					from_node = g.put_node(from_id);
				if (!g.get_node(to_id, to_node))
					to_node = g.put_node(to_id);

				Edge e = g.add_edge(from_node, to_node, Utils::euclid_distance(from_node, to_node));
				if (!is_oneway(elem))
					g.add_edge(to_node.id, from_node.id, e.weight);
			}
		}
	}
}

QJsonDocument OsmParser::get_json_doc(QString path)
{
	QFile file(path);
	if (!file.exists())
		throw runtime_error("Невозможно открыть файл " + path.toStdString() + ": файл не существует");

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		throw runtime_error("Невозможно открыть файл " + path.toStdString());

	QString data = file.readAll();
	file.close();
	if (data.isEmpty())
		throw runtime_error("Ошибка чтения файла " + path.toStdString());
	QJsonParseError err;
	QJsonDocument json_doc = QJsonDocument::fromJson(data.toUtf8(), &err);
	if (json_doc.isNull() || err.error != QJsonParseError::NoError)
		throw runtime_error("Ошибка декодирования файла " + path.toStdString() + ": " + err.errorString().toStdString());

	return json_doc;
}

bool OsmParser::is_oneway(QJsonObject elem)
{
	if (!elem.contains("tags"))
		return false;

	QJsonObject tags = elem["tags"].toObject();
	if (!tags.contains("oneway"))
		return false;

	QJsonValue oneway = tags["oneway"];
	switch (oneway.type()) {
	case QJsonValue::String:
		return oneway.toString() == "yes" || oneway.toString() == "*";
	case QJsonValue::Double:
		return oneway.toDouble() == 1;
	case QJsonValue::Bool:
		return oneway.toBool();
	}

	return false;
}
