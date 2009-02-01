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

#ifndef SINGLEAPPLICATION_P_H
#define SINGLEAPPLICATION_P_H

#include <QtCore/QThread>

#include <QtNetwork/QLocalServer>

class QString;

class ServerThread : public QThread
{
	Q_OBJECT

public:
	ServerThread(const QString& key, QObject* parent = 0);
	~ServerThread();

signals:
	void messageReceived(const QString& message);

protected:
	void run();

private slots:
	void newConnection();
	void readyRead();

private:
	QString key;
	QLocalServer localServer;
};

#endif // SINGLEAPPLICATION_P_H
