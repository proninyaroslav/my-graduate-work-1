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
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import QtPositioning 5.6
import AppController 1.0
import "sprintf.js" as Sprintf

ApplicationWindow {
	id: appWindow
	width: 1024
	height: 768
	visible: true
	Component.onCompleted: {
		setX(Screen.width / 2 - width / 2);
		setY(Screen.height / 2 - height / 2);
	}

	header: ToolBar {
		height: 50
		background: Rectangle {
			anchors.fill: parent
			gradient: Gradient {
				GradientStop {position: 0; color: "#FFFFFF"}
				GradientStop {position: 1; color: "#EEEEEE"}
			}

			Rectangle {
				height: 1
				width: parent.width
				anchors.bottom: parent.bottom
				color: "#999999"
			}
		}

		ComboBox {
			id: algorithms
			width: 125
			anchors.left: parent.left
			anchors.leftMargin: 10
			anchors.verticalCenter: parent.verticalCenter
			background: Rectangle {
				anchors.fill: parent
				border.color: "#999999"
				gradient: Gradient {
					GradientStop {position: 0; color: "#FFFFFF"}
					GradientStop {position: 1; color: "#E0E0E0"}
				}
			}
			indicator: Image {
				height: 20
				width: 20
				x: algorithms.width - width - algorithms.padding
				y: algorithms.topPadding + (algorithms.availableHeight - height) / 2
				source: "qrc:/qt-project.org/imports/QtQuick/Controls.2/images/double-arrow.png"
			}
			model: ["NBA*", "A*", "Дейкстра"]
			onCurrentIndexChanged: {
				switch (algorithms.currentIndex) {
				case 0:
					map.algorithm = AppController.NBA_STAR
					break
				case 1:
					map.algorithm = AppController.A_STAR
					break
				case 2:
					map.algorithm = AppController.DIJKSTRA
					break
				}
				map.draw_path()
			}
		}

		ToolbarButton {
			id: clear
			text: "Очистить"
			anchors.left: algorithms.right
			anchors.leftMargin: 25
			anchors.verticalCenter: parent.verticalCenter
			onClicked: map.clear_markers()
		}

		CheckBox {
			id: traffic_jams
			anchors.left: clear.right
			anchors.verticalCenter: parent.verticalCenter
			anchors.leftMargin: 25
			indicator: Rectangle {
				x: traffic_jams.leftPadding
				y: (parent.height - height) / 2
				implicitWidth: 20
				implicitHeight: 20
				radius: 3
				border.width: 1
				border.color: "#999999"

				Image {
					visible: traffic_jams.checkState === Qt.Checked
					height: parent.height
					width: parent.width
					x: (parent.width - width) / 2
					y: (parent.height - height) / 2
					source: "qrc:/qt-project.org/imports/QtQuick/Controls.2/images/check.png"
				}
			}
			text: "Пробки"
			checked: false
			onCheckedChanged: {
				if (traffic_jams.checked) {
					map.enable_jams()
				} else {
					map.disable_jams()
					controller.reset_graph()
					map.draw_path()
				}

				generate_jams.visible = traffic_jams.checked
			}
		}

		RowLayout {
			id: markers
			anchors.left: traffic_jams.right
			anchors.verticalCenter: parent.verticalCenter
			anchors.leftMargin: 25
			visible: false
			Image {
				id: from_image
				source: "res/marker_from.svg"
				sourceSize.width: clear.height
				sourceSize.height: clear.height
			}

			SelectableLabel {id: from_coords}

			Image {
				id: to_image
				source: "res/marker_to.svg"
				sourceSize.width: clear.height
				sourceSize.height: clear.height
			}

			SelectableLabel {id: to_coords}
		}

		Label {
			id: distance
			anchors.left: markers.right
			anchors.verticalCenter: parent.verticalCenter
			anchors.leftMargin: 25
			horizontalAlignment: Qt.AlignHCenter
			verticalAlignment: Qt.AlignVCenter
		}
	}

	MapComponent {
		id: map
		onPathAdded: distance.text = Sprintf.sprintf("Расстояние: %.3f км; Найдено за: %d мс",
							     controller.calc_distance(map.path_polyline.path), map.find_time)
		onPathDestroyed: distance.text = "";
		onMarkerFromChanged: from_coords.text = Sprintf.sprintf("%.3f, %.3f", coords.longitude, coords.latitude)
		onMarkerToChanged: to_coords.text = Sprintf.sprintf("%.3f, %.3f", coords.longitude, coords.latitude)
		onMarkersAdded: markers.visible = true
		onMarkersRemoved: markers.visible = false
		onJamsBoundsChanged: generate_jams.visible = true
	}

	Button {
		id: generate_jams
		text: "Генерация"
		anchors.top: map.top
		anchors.right: map.right
		anchors.topMargin: 10
		anchors.rightMargin: 10
		visible: false
		onClicked: {
			generate_jams.visible = false
			controller.gen_traffic_jams(map.jams_bounds.path);
			map.draw_path()
		}
		contentItem: Text {
			text: generate_jams.text
			font: generate_jams.font
			color: "white"
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			elide: Text.ElideRight
		}
		background: Rectangle {
			anchors.fill: parent
			radius: 5
			border.color: "#999999"
			color: "dodgerblue"
		}
	}
}
