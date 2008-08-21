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

	SingleApplication instance("LoaderExample", &app);
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
