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

#ifndef SINGLE_APPLICATION_P_H
#define SINGLE_APPLICATION_P_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QThread>

#include <QtNetwork/QLocalServer>

class LocalThread : public QThread
{
	Q_OBJECT

public:
	LocalThread(quintptr socketDescriptor, const QString& id, QObject* parent = 0);
	~LocalThread();

signals:
	void messageReceived(const QString& message);

protected:
	void run();

private:
	quintptr socketDescriptor;
	QString id;

	bool stopped;
};


class ThreadedLocalServer : public QLocalServer
{
	Q_OBJECT

public:
	ThreadedLocalServer(const QString& id, QObject* parent = 0) : QLocalServer(parent)
	{ this->id = id; }

signals:
	void messageReceived(const QString& message);

protected:
	void incomingConnection(quintptr socketDescriptor)
	{
		LocalThread* thread = new LocalThread(socketDescriptor, id, this);
		connect(thread, SIGNAL(messageReceived(const QString&)),
				 this, SIGNAL(messageReceived(const QString&)));
		connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
		thread->start();
	}

private:
	QString id;
};

#endif // SINGLE_APPLICATION_P_H
