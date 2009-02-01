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

#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QtCore/QObject>

#ifdef SINGLEAPPLICATION_DLL
#	ifdef SINGLEAPPLICATION_BUILD
#		define SINGLEAPPLICATION_EXPORT Q_DECL_EXPORT
#	else
#		define SINGLEAPPLICATION_EXPORT Q_DECL_IMPORT
#	endif
#endif
#ifndef SINGLEAPPLICATION_EXPORT
#	define SINGLEAPPLICATION_EXPORT
#endif

class QString;

class SingleApplicationPrivate;
class SINGLEAPPLICATION_EXPORT SingleApplication : public QObject
{
	Q_OBJECT

public:
	explicit SingleApplication(QObject* parent = 0);
	explicit SingleApplication(const QString& key, QObject* parent = 0);
	virtual ~SingleApplication();

	QString key() const;
	bool isRunning() const;

public slots:
	bool sendMessage(const QString& message, int timeout = 500);

signals:
	void messageReceived(const QString& message);

private:
	Q_DISABLE_COPY(SingleApplication);
	SingleApplicationPrivate* const d;
};

#endif // SINGLEAPPLICATION_H
