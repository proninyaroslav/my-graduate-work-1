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

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QMessageBox>
#include <QUnhandledException>
#include <QQmlContext>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include "geopoint.h"
#include "exception.h"
#include "appcontroller.h"

Q_DECLARE_METATYPE(GeoPoint)

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
	QApplication app(argc, argv);
	QCoreApplication::setApplicationName("Навигация");

	QCommandLineParser parser;
	parser.addHelpOption();
	QCommandLineOption city_option(QStringList() << "c" << "city",
				       "Загружаемая карта", "название", "Тамбов");
	QCommandLineOption test_option(QStringList() << "t" << "test",
				       "Выводит текст результата тестирования алгоритмов", "кол-во испытаний");
	parser.addOption(city_option);
	parser.addOption(test_option);
	parser.process(app);

	AppController::city_t city;
	QString city_name = parser.value(city_option);
	if (!QString::compare(city_name, "Тамбов", Qt::CaseInsensitive)) {
		city = AppController::city_t::TAMBOV;
	} else if (!QString::compare(city_name, "Москва", Qt::CaseInsensitive)) {
		city = AppController::city_t::MOSCOW;
	} else {
		QMessageBox::critical(NULL, "Ошибка", "Нет данных для города: " + city_name);

		return -1;
	}

	AppController *controller = NULL;
	try {
		controller = new AppController(city);
	} catch (const Exception e) {
		QMessageBox::critical(NULL, "Ошибка", e.what());

		return -1;
	}

	if (parser.isSet(test_option)) {
		controller->test_algorithms(parser.value(test_option).toInt());

		return 0;
	}

	qRegisterMetaType<GeoPoint>();
	qmlRegisterType<AppController>("AppController", 1, 0, "AppController");
	QQmlApplicationEngine engine;

	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;
	engine.rootContext()->setContextProperty("controller", controller);

	return app.exec();
}
