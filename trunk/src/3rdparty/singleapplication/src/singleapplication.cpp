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
#include <QtCore/QHash>
#include <QtCore/QRegExp>
#include <QtCore/QString>

#include <QtNetwork/QLocalSocket>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#else
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#endif

#include "systemsemaphore.h"

// timeouts are in ms
static const int i_timeout_sem		= 10;
static const int i_timeout_connect	= 200;
static const int i_timeout_read		= 5000;
static const int i_timeout_write	= 5000;

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

	socket->write(block);
	bool ok = socket->waitForBytesWritten(timeout);

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
			return message;
	}
	in >> message;

	if(ok)
		*ok = true;

	return message;
}


LocalThread::LocalThread(quintptr socketDescriptor, const QString& id, QObject* parent) : QThread(parent)
{
	this->socketDescriptor = socketDescriptor;
	this->id = id;
}

LocalThread::~LocalThread()
{
	if(isRunning())
	{
		stopped = true;
		wait();
	}
}

void LocalThread::run()
{
	stopped = false;

	QLocalSocket socket;
	if(!socket.setSocketDescriptor(socketDescriptor) || socket.state() != QLocalSocket::ConnectedState)
		return;

	// send id to client
	if(!writeMessage(&socket, id))
		return;

	const QString magic = id + QLatin1Char(':');
	// wait to messages from client
	while(socket.state() == QLocalSocket::ConnectedState && !stopped)
	{
		if(socket.waitForReadyRead(1000))
		{
			while(socket.bytesAvailable() > 0)
			{
				bool ok;
				QString message = readMessage(&socket, &ok);
				if(ok && message.startsWith(magic))
				{
					emit messageReceived(message.remove(0, magic.size()));
					writeMessage(&socket, id, i_timeout_write);
				}
			}
		}
	}
}


typedef QHash<QString, SystemSemaphore*> SemaphoreDict;
typedef QHash<QString, QLocalServer*> ServerDict;

class SingleApplicationPrivate
{
public:
	SingleApplicationPrivate(SingleApplication* parent)
		: q(parent), server(0), socket(0)
	{}
	~SingleApplicationPrivate()
	{ delete socket; }

	void init();
	bool isRunning() const;
	bool connectToServer();
	bool sendMessage(const QString& message, int timeout);

	SingleApplication* q;

	QString id;
	QString serverName;

	QString allowedId;
	QString allowedServerName;

	QLocalServer* server;
	QLocalSocket* socket;

	static SemaphoreDict& semaphoreDict();
	static ServerDict& serverDict();
};

static bool qSemaphoreDictInit = false;
static void cleanSemaphoreDict()
{
	qDeleteAll(SingleApplicationPrivate::semaphoreDict());
	SingleApplicationPrivate::semaphoreDict().clear();
	qSemaphoreDictInit = false;
}

SemaphoreDict& SingleApplicationPrivate::semaphoreDict()
{
	static SemaphoreDict dict;
	if(!qSemaphoreDictInit)
	{
		qSemaphoreDictInit = true;
		qAddPostRoutine(cleanSemaphoreDict);
	}

	return dict;
}

static bool qServerDictInit = false;
static void cleanServerDict()
{
	qDeleteAll(SingleApplicationPrivate::serverDict());
	SingleApplicationPrivate::serverDict().clear();
	qServerDictInit = false;
}

ServerDict& SingleApplicationPrivate::serverDict()
{
	static ServerDict dict;
	if(!qServerDictInit)
	{
		qServerDictInit = true;
		qAddPostRoutine(cleanServerDict);
	}

	return dict;
}

void SingleApplicationPrivate::init()
{
	if(id.isEmpty())
		id = QLatin1String("SA");
	if(serverName.isEmpty())
		serverName = QLatin1String("SingleApplication");

	static QString login;
	if(login.isEmpty())
	{
#ifdef Q_OS_WIN

		QT_WA({
			wchar_t buffer[256];
			DWORD bufferSize = sizeof(buffer) / sizeof(wchar_t) - 1;
			GetUserNameW(buffer, &bufferSize);
			login = QString::fromUtf16((ushort*)buffer);
		},
		{
			char buffer[256];
			DWORD bufferSize = sizeof(buffer) / sizeof(char) - 1;
			GetUserNameA(buffer, &bufferSize);
			login = QString::fromLocal8Bit(buffer);
		});
#else
		struct passwd* pwd = getpwuid(getuid());
		if(pwd)
			login = QString(pwd->pw_name);
#endif
	}
	allowedId = id;
	allowedServerName = serverName;
	allowedId.replace(QRegExp(QLatin1String("[^A-Za-z0-9]")), QString());
	allowedServerName.replace(QRegExp(QLatin1String("[^A-Za-z0-9]")), QString());
	allowedServerName.prepend(login).append(allowedId);

	if(!isRunning())
	{
		server = serverDict().value(allowedServerName, 0);
		if(!server)
		{
			server = new ThreadedLocalServer(allowedId);
			serverDict().insert(allowedServerName, server);
		}
		if(!server->isListening() && !server->listen(allowedServerName))
		{
			delete serverDict().take(allowedServerName);
			server = 0;
			return;
		}
		QObject::connect(server, SIGNAL(messageReceived(const QString&)),
						 q, SIGNAL(messageReceived(const QString&)),
						 Qt::QueuedConnection);
	}
}

bool SingleApplicationPrivate::isRunning() const
{
	SystemSemaphore* semaphore = semaphoreDict().value(allowedServerName, 0);
	if(semaphore)
		return false;

	semaphore = new SystemSemaphore(allowedServerName, 1);
	if(semaphore->tryAcquire(i_timeout_sem))
	{
		semaphoreDict().insert(allowedServerName, semaphore);
		return false;
	}
	delete semaphore;

	return true;
}

bool SingleApplicationPrivate::connectToServer()
{
	if(socket && socket->state() == QLocalSocket::ConnectedState)
		return true;

	if(!socket)
		socket = new QLocalSocket(q);
	socket->connectToServer(allowedServerName);
	if(socket->waitForConnected(i_timeout_connect))
	{
		bool ok;
		const QString message = readMessage(socket, &ok);
		// now compare received bytes with id
		if(ok && message.compare(allowedId) == 0)
			return true;
	}
	socket->abort();
	delete socket;
	socket = 0;

	return false;
}

bool SingleApplicationPrivate::sendMessage(const QString& message, int timeout)
{
	if(!isRunning() || !connectToServer())
		return false;

	const QString magic = allowedId + QLatin1Char(':');
	if(!writeMessage(socket, magic + message, timeout))
		return false;

	bool ok;
	QString response = readMessage(socket, &ok);

	return ok && response.compare(allowedId) == 0;
}


/*!
  \class SingleApplication
  \brief The SingleApplication class provides an crossplatform interface to detect a running
  instance, and to send command strings to that instance.

  The SingleApplication component is basically imitating QtSingleApplication commercial class.
  Unlike QtSingleApplication the SingleApplication implementation uses System Semaphore
  to detect a running instance and so-called "Local Sockets" to communicate with it.
*/

/*!
  Creates a SingleApplication object with the parent \a parent and the identifier \a id.
*/

SingleApplication::SingleApplication(const QString& id, QObject* parent) :
	QObject(parent), d(new SingleApplicationPrivate(this))
{
	d->id = id;
	d->init();
}

/*!
  Creates a SingleApplication object with the parent \a parent and the identifier \a id.
  \a serverName specifies name for internal local socket.
*/

SingleApplication::SingleApplication(const QString& id, const QString& serverName, QObject* parent) :
	QObject(parent), d(new SingleApplicationPrivate(this))
{
	d->id = id;
	d->serverName = serverName;
	d->init();
}

/*!
  The destructor destroys the SingleApplication object, but the
  underlying system semaphore and local server are not removed from the system
  unless application exit.

  Note: If the same application is started again it WILL find this instance.
*/

SingleApplication::~SingleApplication()
{
	delete d;
}

/*!
  Returns the identifier of this singleton object.
*/

QString SingleApplication::id() const
{
	return d->id;
}

/*!
  Returns the name of internal local server or socket.
*/

QString SingleApplication::serverName() const
{
	return d->serverName;
}

/*!
  Returns true if another instance of this application has started;
  otherwise returns false.

  This function does not find instances of this application that are
  being run by a different user.
*/

bool SingleApplication::isRunning() const
{
	return d->isRunning();
}

/*!
  This is an overloaded static member function, provided for convenience.

  Returns true if another instance of this application has started;
  otherwise returns false.
*/

bool SingleApplication::isRunning(const QString& id)
{
	return SingleApplication(id).isRunning();
}

/*!
  Tries to send the text \a message to the currently running instance.
  The SingleApplication object in the running instance
  will emit the messageReceived() signal when it receives the message.

  This function returns true if the message has been sent to, and
  processed by, the current instance. If there is no instance
  currently running, or if the running instance fails to process the
  message within \a timeout milliseconds this function return false.

  \sa messageReceived()
*/

bool SingleApplication::sendMessage(const QString& message, int timeout)
{
	return d->sendMessage(message, timeout);
}

/*!
  This is an overloaded static member function, provided for convenience.

  Tries to send the text \a message to the currently running instance.
*/

bool SingleApplication::sendMessage(const QString& id, const QString& message, int timeout)
{
	return SingleApplication(id).sendMessage(message, timeout);
}

/*!
  \fn void SingleApplication::messageReceived(const QString& message)

  This signal is emitted when the current instance receives a \a
  message from another instance of this application.

  \sa sendMessage()
*/

#include "moc_singleapplication.cpp"
