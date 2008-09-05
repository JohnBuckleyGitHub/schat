#include "systemsemaphore.h"
#include "systemsemaphore_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

SystemSemaphorePrivate::SystemSemaphorePrivate() :
	semaphore(0), error(SystemSemaphore::NoError)
{
}

void SystemSemaphorePrivate::setErrorString(const QString& function)
{
	BOOL windowsError = GetLastError();
	if(!windowsError)
		return;

	switch(windowsError)
	{
		case ERROR_NO_SYSTEM_RESOURCES:
		case ERROR_NOT_ENOUGH_MEMORY:
			error = SystemSemaphore::OutOfResources;
			errorString = QCoreApplication::translate("SystemSemaphore", "%1: out of resources").arg(function);
			break;
		case ERROR_ACCESS_DENIED:
			error = SystemSemaphore::PermissionDenied;
			errorString = QCoreApplication::translate("SystemSemaphore", "%1: permission denied").arg(function);
			break;
		default:
			errorString = QCoreApplication::translate("SystemSemaphore", "%1: unknown error %2").arg(function).arg(windowsError);
			error = SystemSemaphore::UnknownError;
#if defined SYSTEMSEMAPHORE_DEBUG
			qDebug() << errorString << "key" << key;
#endif
	}
}

HANDLE SystemSemaphorePrivate::handle(SystemSemaphore::AccessMode)
{
	// don't allow making handles on empty keys
	if(key.isEmpty())
		return 0;

	// Create it if it doesn't already exists.
	if(!semaphore)
	{
		QString safeName = makeKeyFileName();
		QT_WA({
			semaphore = CreateSemaphoreW(0, initialValue, MAXLONG, (TCHAR*)safeName.utf16());
		}, {
			semaphore = CreateSemaphoreA(0, initialValue, MAXLONG, safeName.toLocal8Bit().constData());
		});
		if(!semaphore)
			setErrorString(QLatin1String("SystemSemaphore::handle"));
	}

	return semaphore;
}

void SystemSemaphorePrivate::cleanHandle()
{
	if(semaphore && !CloseHandle(semaphore))
	{
#if defined SYSTEMSEMAPHORE_DEBUG
		qDebug() << QLatin1String("SystemSemaphorePrivate::CloseHandle: sem failed");
#endif
	}
	semaphore = 0;
}

bool SystemSemaphorePrivate::modifySemaphore(int count, int timeout)
{
	if(!handle())
		return false;

	if(count > 0)
	{
		if(!ReleaseSemaphore(semaphore, count, 0))
		{
			setErrorString(QLatin1String("SystemSemaphore::modifySemaphore"));
#if defined SYSTEMSEMAPHORE_DEBUG
			qDebug() << QLatin1String("SystemSemaphore::modifySemaphore ReleaseSemaphore failed");
#endif
			return false;
		}
	}
	else
	{
		DWORD dwMilliseconds = (timeout < 0 ? INFINITE : (timeout == 0 ? 1 : timeout));
		DWORD dwWaitResult = WaitForSingleObject(semaphore, dwMilliseconds);
		if(dwWaitResult == WAIT_TIMEOUT)
			return false;
		if(dwWaitResult != WAIT_OBJECT_0)
		{
			setErrorString(QLatin1String("SystemSemaphore::modifySemaphore"));
#if defined SYSTEMSEMAPHORE_DEBUG
			qDebug() << QLatin1String("SystemSemaphore::modifySemaphore WaitForSingleObject failed");
#endif
			return false;
		}
	}

	return true;
}
