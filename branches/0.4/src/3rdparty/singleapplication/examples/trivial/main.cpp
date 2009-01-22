#include <QtCore/QObject>
#include <QtCore/QString>

#include <QtGui/QApplication>

#include "singleapplication.h"
#include "widget.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

	SingleApplication instance("TrivialExample", &app);
	if(instance.isRunning())
	{
		QString message = Widget::tr("Hello! Did You hear other Trivial Example instance? :)\nPID: %1")
							.arg(app.applicationPid());
		if(instance.sendMessage(message))
			return 0;
	}

	Widget w;
	w.show();

	QObject::connect(&instance, SIGNAL(messageReceived(const QString&)),
						 &w, SLOT(handleMessage(const QString&)));

	return app.exec();
}
