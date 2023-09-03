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

import QtQuick 2.9
import QtLocation 5.6
import QtPositioning 5.6
import AppController 1.0

Map {
	id: map
	anchors.fill: parent
	plugin: plugin
	center: controller.city === AppController.TAMBOV ? QtPositioning.coordinate(52.7299, 41.4504) :
							   QtPositioning.coordinate(55.7750, 37.6170)
	zoomLevel: 15

	Plugin {
		id: plugin
		name: "osm"
	}

	property var path_polyline
	property var marker_from
	property var marker_to
	property var bounds_nw
	property var bounds_se
	property var jams_bounds
	property var find_time
	property var algorithm

	signal pathAdded()
	signal pathDestroyed()
	signal markerFromAdded();
	signal markersAdded();
	signal markersRemoved();
	signal markerFromChanged(var coords)
	signal markerToChanged(var coords)
	signal jamsBoundsChanged()

	MouseArea {
		anchors.fill: parent
		onClicked: {
			if (marker_to && marker_from)
				return

			var node = controller.find_nearest_node(map.toCoordinate(Qt.point(mouse.x, mouse.y)))
			if (marker_from) {
				marker_to = Qt.createQmlObject('Marker {id: marker; source: "res/marker_to.svg";' +
							       'onCoordinateChanged: {on_coords_changed(marker); markerToChanged(marker.coordinate)}}', map)
				marker_to.id = node.id;
				marker_to.coordinate = node.coords
				map.addMapItem(marker_to)
				markersAdded()
			} else {
				marker_from = Qt.createQmlObject('Marker {id: marker; source: "res/marker_from.svg"; ' +
								 'onCoordinateChanged: {on_coords_changed(marker); markerFromChanged(marker.coordinate)}}', map)
				marker_from.id = node.id
				marker_from.coordinate = node.coords
				map.addMapItem(marker_from)
			}
		}
	}

	function clear_markers() {
		if (marker_to) {
			map.removeMapItem(marker_to)
			marker_to.destroy()
		}

		if (marker_from) {
			map.removeMapItem(marker_from)
			marker_from.destroy()
		}

		if (path_polyline) {
			map.removeMapItem(path_polyline)
			path_polyline.destroy()
			find_time = 0
			pathDestroyed()
		}
		markersRemoved();
	}

	function on_coords_changed(marker) {
		var node = controller.find_nearest_node(marker.coordinate)
		marker.id = node.id
		marker.coordinate = node.coords
		draw_path()
	}

	function on_bounds_coords_changed() {
		if (!jams_bounds || !bounds_nw || !bounds_se)
			return;
		jams_bounds.path = [bounds_nw.coordinate,
				    {latitude: bounds_nw.coordinate.latitude, longitude: bounds_se.coordinate.longitude},
				    bounds_se.coordinate,
				    {latitude: bounds_se.coordinate.latitude, longitude: bounds_nw.coordinate.longitude}]
	}

	function draw_path() {
		if (marker_to && marker_from) {
			var points = controller.find_path(marker_from.id, marker_to.id, algorithm)
			find_time = points.pop()

			if (points.length > 0) {
				if (!path_polyline) {
					path_polyline = Qt.createQmlObject('Path {}', map)
					map.addMapItem(path_polyline)
				}
				path_polyline.path = points;
				pathAdded()
			} else if (path_polyline) {
				map.removeMapItem(path_polyline)
				path_polyline.destroy()
				find_time = 0
				pathDestroyed()
			}
		}
	}

	function enable_jams() {
		if (bounds_nw && bounds_se)
			return;

		var center = map.center
		var half = 1 / map.zoomLevel * 0.1
		bounds_nw = Qt.createQmlObject('BoundsMarker {onCoordinateChanged: on_bounds_coords_changed()}', map)
		bounds_nw.coordinate = QtPositioning.coordinate(center.latitude + half, center.longitude - half)
		map.addMapItem(bounds_nw)

		bounds_se = Qt.createQmlObject('BoundsMarker {onCoordinateChanged: on_bounds_coords_changed()}', map)
		bounds_se.coordinate = QtPositioning.coordinate(center.latitude - half, center.longitude + half)
		map.addMapItem(bounds_se)

		jams_bounds = Qt.createQmlObject('Bounds {onPathChanged: jamsBoundsChanged()}', map)
		jams_bounds.path = [bounds_nw.coordinate,
				    {latitude: bounds_nw.coordinate.latitude, longitude: bounds_se.coordinate.longitude},
				    bounds_se.coordinate,
				    {latitude: bounds_se.coordinate.latitude, longitude: bounds_nw.coordinate.longitude}]
		map.addMapItem(jams_bounds)
	}

	function disable_jams() {
		if (jams_bounds) {
			map.removeMapItem(jams_bounds)
			jams_bounds.destroy()
		}

		if (bounds_nw) {
			map.removeMapItem(bounds_nw)
			bounds_nw.destroy()
		}

		if (bounds_se) {
			map.removeMapItem(bounds_se)
			bounds_se.destroy()
		}
	}
}
