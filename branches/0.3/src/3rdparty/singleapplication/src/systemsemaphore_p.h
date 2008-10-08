#ifndef SYSTEMSEMAPHORE_P_H
#define SYSTEMSEMAPHORE_P_H

#include "systemsemaphore.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#else
#include <sys/sem.h>
#endif

class SystemSemaphorePrivate
{
public:
	SystemSemaphorePrivate();

	QString makeKeyFileName();

#ifdef Q_OS_WIN
	HANDLE handle(SystemSemaphore::AccessMode mode = SystemSemaphore::Open);
#else
	key_t handle(SystemSemaphore::AccessMode mode = SystemSemaphore::Open);
#endif
	void setErrorString(const QString& function);
	void cleanHandle();
	bool modifySemaphore(int count, int timeout = -1);

	QString key;
	QString fileName;
	int initialValue;
#ifdef Q_OS_WIN
	HANDLE semaphore;
	HANDLE semaphoreLock;
#else
	int semaphore;
	bool createdFile;
	bool createdSemaphore;
	key_t unix_key;
#endif

	QString errorString;
	SystemSemaphore::SystemSemaphoreError error;
};

#endif // SYSTEMSEMAPHORE_P_H
