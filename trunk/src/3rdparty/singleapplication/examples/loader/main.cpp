/*******************************************************************************
SingleApplication provides a way to detect a running instance.
Copyright (C) 2008  Ritt K.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
********************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QString>

#include <QtGui/QApplication>

#include "mainwindow.h"
#include "singleapplication.h"

#ifdef Q_OS_WIN
#define _WIN32_WINNT 0x0501
#include <qt_windows.h>
#endif

int main(int argc, char* argv[])
{
#ifdef Q_OS_WIN
	AllowSetForegroundWindow(ASFW_ANY);
#endif

	QApplication app(argc, argv);
	app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

#ifdef Q_OS_WIN
	// This is needed so that relative paths will work on Windows regardless of where the app is launched from.
	QDir::setCurrent(app.applicationDirPath());
#endif

	SingleApplication instance("LoaderExample");
	if(instance.isRunning())
	{
		for(int i = 1; i < argc; ++i)
			instance.sendMessage(argv[i]);
		return 0;
	}

	MainWindow mw;
	mw.show();
	for(int i = 1; i < argc; ++i)
		mw.handleMessage(argv[i]);

	QObject::connect(&instance, SIGNAL(messageReceived(const QString&)),
						 &mw, SLOT(handleMessage(const QString&)));

	return app.exec();
}
