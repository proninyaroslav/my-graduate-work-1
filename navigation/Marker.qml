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

MapQuickItem {
	property alias source: image.source
	property var id
	signal mousePressed()
	signal mouseReleased()

	id: marker
	z: 1
	anchorPoint.x: image.width / 2
	anchorPoint.y: image.height
	sourceItem: Image {
		id: image
		sourceSize.width: 36
		sourceSize.height: 36
	}
	MouseArea  {
		anchors.fill: parent
		hoverEnabled: false
		drag.target: marker
		preventStealing: true
		onPressed: mousePressed()
		onReleased: mouseReleased()
	}
}
