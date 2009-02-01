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

#include "singleapplication.h"
#include "singleapplication_p.h"

#include <QtCore/QByteArray>
#include <QtCore/QCoreApplication>
#include <QtCore/QDataStream>
#include <QtCore/QDir>
#include <QtCore/QRegExp>
#include <QtCore/QSharedMemory>
#include <QtCore/QString>

#include <QtNetwork/QLocalSocket>

#ifndef Q_OS_WIN
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#endif

// timeouts all are in ms
static const int i_timeout_connect	= 200;
static const int i_timeout_read		= 250;
static const int i_timeout_write	= 500;

static QString login()
{
	static QString login;
	if(login.isEmpty())
	{
#ifdef Q_WS_QWS
		login = QLatin1String("qws");
#endif
#ifdef Q_OS_WIN
		login = QDir::home().dirName();
#else
		struct passwd* pwd = getpwuid(getuid());
		if(pwd)
			login = QLatin1String(pwd->pw_name);
#endif
		Q_ASSERT(!login.isEmpty());
	}

	return login;
}

static QString makeUniqueKey(const QString& key)
{
	QString uniqueKey = QString("_sa_");
	uniqueKey.append(key);
	uniqueKey.append(QLatin1Char('_'));
	uniqueKey.append(login().toUtf8().toHex());

	return uniqueKey;
}

static bool writeMessage(QLocalSocket* socket, const QString& message, int timeout = i_timeout_write)
{
	if(!socket || socket->state() != QLocalSocket::ConnectedState)
		return false;

	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out << (quint32)0;
	out << message;
	out.device()->seek(0);
	out << (quint32)(block.size() - sizeof(quint32));

	bool ok = (socket->write(block) != -1);
	if(ok)
		ok &= socket->waitForBytesWritten(timeout);

	return ok;
}

static QString readMessage(QLocalSocket* socket, bool* ok = 0)
{
	QString message;
	if(ok)
		*ok = false;

	if(!socket || socket->state() != QLocalSocket::ConnectedState)
		return message;

	while(socket->bytesAvailable() < (int)sizeof(quint32))
	{
		if(!socket->waitForReadyRead(i_timeout_read))
			return message;
	}

	quint32 blockSize;
	QDataStream in(socket);
	in >> blockSize;
	while(socket->bytesAvailable() < blockSize)
	{
		if(!socket->waitForReadyRead(i_timeout_read))
		{
			in >> message;
			return message;
		}
	}
	in >> message;

	if(ok)
		*ok = true;

	return message;
}


ServerThread::ServerThread(const QString& key, QObject* parent) : QThread(parent),
	key(key)
{
	connect(&localServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

ServerThread::~ServerThread()
{
	localServer.close();

	quit();
	wait();
}

void ServerThread::run()
{
	QString uniqueKey = makeUniqueKey(key);

	if(!localServer.listen(uniqueKey))
		return;

	exec();
}

void ServerThread::newConnection()
{
	QLocalSocket* socket = localServer.nextPendingConnection();
	if(socket)
	{
		connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
		connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

		if(!writeMessage(socket, key))
			socket->close();
	}
}

void ServerThread::readyRead()
{
	QLocalSocket* socket = qobject_cast<QLocalSocket*>(sender());
	if(socket)
	{
		bool ok = false;
		QString message;
		while(socket->bytesAvailable() > 0)
		{
			// read message from client
			message = readMessage(socket, &ok);
			if(ok)
			{
				writeMessage(socket, key, i_timeout_write); // confirm
				emit messageReceived(message);
			}
		}
	}
}


class SingleApplicationPrivate
{
public:
	inline SingleApplicationPrivate(SingleApplication* parent) : q(parent),
		socket(0),
		isRunning(false)
	{}

	void init();
	bool connectToServer();
	bool sendMessage(const QString& message, int timeout);

	SingleApplication* q;

	QString key;
	QString uniqueKey;

	QLocalSocket* socket;

	bool isRunning;
};

void SingleApplicationPrivate::init()
{
	key.replace(QRegExp("[^A-Za-z0-9]"), QString());
	if(key.isEmpty())
		key = QLatin1String("SADEFAULTKEY");
	uniqueKey = makeUniqueKey(key);

	QSharedMemory* shMem = new QSharedMemory(uniqueKey, q);
	if(shMem->create(1))
	{
		ServerThread* server = new ServerThread(key, q);
		QObject::connect(server, SIGNAL(finished()), server, SLOT(deleteLater()));
		QObject::connect(server, SIGNAL(messageReceived(const QString&)),
						 q, SIGNAL(messageReceived(const QString&)),
						 Qt::QueuedConnection);
		server->start(QThread::LowPriority);
	}
	else
	{
		isRunning = (shMem->error() == QSharedMemory::AlreadyExists);
		delete shMem;
	}
}

bool SingleApplicationPrivate::connectToServer()
{
	if(socket && socket->state() == QLocalSocket::ConnectedState)
		return true;

	if(!socket)
		socket = new QLocalSocket(q);
	socket->connectToServer(uniqueKey);
	if(socket->waitForConnected(i_timeout_connect))
	{
		bool ok;
		const QString message = readMessage(socket, &ok);
		// now compare received bytes with key
		if(ok && message == key)
			return true;

		socket->disconnectFromServer();
		socket->waitForDisconnected(i_timeout_connect);
	}

	socket->abort();

	return false;
}

bool SingleApplicationPrivate::sendMessage(const QString& message, int timeout)
{
	if(!isRunning || !connectToServer())
		return false;

	if(!writeMessage(socket, message, timeout))
		return false;

	bool ok;
	QString response = readMessage(socket, &ok);

	return (ok && response == key);
}


/*!
  \class SingleApplication
  \brief The SingleApplication class provides an crossplatform interface to detect a running
  instance, and to send command strings to that instance.

  The SingleApplication component is basically imitating QtSingleApplication commercial class.
  Unlike QtSingleApplication the SingleApplication implementation uses Shared Memory
  to detect a running instance and so-called "Local Sockets" to communicate with it.
*/

/*!
  Creates a SingleApplication object with the parent \a parent and the default key.

  \sa key(), QCoreApplication::applicationName()
*/

SingleApplication::SingleApplication(QObject* parent) : QObject(parent),
	d(new SingleApplicationPrivate(this))
{
	d->key = QCoreApplication::applicationName();
	d->init();
}

/*!
  Creates a SingleApplication object with the parent \a parent and the key \a key.

  \sa key()
*/

SingleApplication::SingleApplication(const QString& key, QObject* parent) : QObject(parent),
	d(new SingleApplicationPrivate(this))
{
	d->key = key;
	d->init();
}

/*!
  Destroys the SingleApplication object.

  Note: If another instance of application with the same key is started again it WILL NOT find this instance.
*/

SingleApplication::~SingleApplication()
{
	delete d;
}

/*!
  Returns the key of this single application.
*/

QString SingleApplication::key() const
{
	return d->key;
}

/*!
  Returns true if another instance of this application has started;
  otherwise returns false.

  This function does not find instances of this application that are
  being run by a different user.
*/

bool SingleApplication::isRunning() const
{
	return d->isRunning;
}

/*!
  Tries to send the text \a message to the currently running instance.
  The SingleApplication object in the running instance
  will emit the messageReceived() signal when it receives the message.

  This function returns true if the message has been sent to another
  currently running instance. If there is no instance currently running,
  or if the running instance fails to process the
  message within \a timeout milliseconds this function return false.

  \sa messageReceived()
*/

bool SingleApplication::sendMessage(const QString& message, int timeout)
{
	return d->sendMessage(message, timeout);
}

/*!
  \fn void SingleApplication::messageReceived(const QString& message)

  This signal is emitted when the current instance receives a \a
  message from another instance of this application.

  \sa sendMessage()
*/

#include "moc_singleapplication.cpp"
