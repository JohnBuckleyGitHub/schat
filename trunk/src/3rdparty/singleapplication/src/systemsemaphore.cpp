#include "systemsemaphore.h"
#include "systemsemaphore_p.h"

#include <QtCore/QDir>

QString SystemSemaphorePrivate::makeKeyFileName()
{
	const QString prefix = QLatin1String("qipc_systemsem_");
	if(key.isEmpty())
		return QString();

	QString allowableKey;
	for(int i = 0; i < key.count(); ++i)
		allowableKey += QString::number(key.at(i).unicode());
	QString strippedKey = key;
	strippedKey.replace(QRegExp(QLatin1String("[^A-Za-z]")), QString());

#ifdef Q_OS_WIN
	return prefix + strippedKey + allowableKey;
#else
	return QDir::tempPath() + QLatin1Char('/') + prefix + strippedKey + allowableKey;
#endif
}


SystemSemaphore::SystemSemaphore(const QString& key, int initialValue, AccessMode mode)
{
	d = new SystemSemaphorePrivate;
	setKey(key, initialValue, mode);
}

SystemSemaphore::~SystemSemaphore()
{
	d->cleanHandle();
	delete d;
}

void SystemSemaphore::setKey(const QString& key, int initialValue, AccessMode mode)
{
	if(key == d->key && mode == Open)
		return;

	d->error = NoError;
	d->errorString = QString();
#ifndef Q_OS_WIN
	// optimization to not destroy/create the file & semaphore
	if(key == d->key && mode == Create && d->createdSemaphore && d->createdFile)
	{
		d->initialValue = initialValue;
		d->unix_key = -1;
		d->handle(mode);
		return;
	}
#endif
	d->cleanHandle();
	d->key = key;
	d->initialValue = initialValue;
	// cache the file name so it doesn't have to be generated all the time.
	d->fileName = d->makeKeyFileName();
	d->handle(mode);
}

QString SystemSemaphore::key() const
{
	return d->key;
}

bool SystemSemaphore::acquire()
{
	return d->modifySemaphore(-1);
}

bool SystemSemaphore::release(int n)
{
	if(n == 0)
		return true;

	if(n < 0)
	{
		qWarning("SystemSemaphore::release: n is negative.");
		return false;
	}

	return d->modifySemaphore(n);
}

bool SystemSemaphore::tryAcquire(int timeout)
{
	return d->modifySemaphore(-1, timeout);
}

SystemSemaphore::SystemSemaphoreError SystemSemaphore::error() const
{
	return d->error;
}

QString SystemSemaphore::errorString() const
{
	return d->errorString;
}
