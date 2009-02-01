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

#include <QtCore/QCoreApplication>
#include <QtCore/QString>

#include "singleapplication.h"
#include "worker.h"

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);

	SingleApplication instance("ConsoleExample");
	if(instance.isRunning())
	{
		QString message = Worker::tr("Hello! Did You hear other Console Example instance? :)\nPID: %1")
							.arg(app.applicationPid());
		if(instance.sendMessage(message))
			return 0;
	}

	Worker o;
	QObject::connect(&instance, SIGNAL(messageReceived(const QString&)),
						 &o, SLOT(showMessage(const QString&)));

	return app.exec();
}
