#include "systemsemaphore.h"
#include "systemsemaphore_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFile>

#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/shm.h>

#include <sys/sem.h>

// OpenBSD 4.2 doesn't define EIDRM, see BUGS section:
// http://www.openbsd.org/cgi-bin/man.cgi?query=semop&manpath=OpenBSD+4.2
#if defined(Q_OS_OPENBSD) && !defined(EIDRM)
#define EIDRM EINVAL
#endif

// We have to define this as on some sem.h will have it
union _t_semun
{
	int val;		/* value for SETVAL */
	struct semid_ds* buf;	/* buffer for IPC_STAT, IPC_SET */
	unsigned short* array;	/* array for GETALL, SETALL */
};

static int createUnixKeyFile(const QString& fileName)
{
	if(QFile::exists(fileName))
		return 0;

	int fd = open(QFile::encodeName(fileName).constData(),
			O_EXCL | O_CREAT | O_RDWR, 0640);
	if(fd == -1)
	{
		if(errno == EEXIST)
			return 0;
		return -1;
	}
	else
	{
		close(fd);
	}

	return 1;
}


SystemSemaphorePrivate::SystemSemaphorePrivate() :
	semaphore(-1), createdFile(false),
	createdSemaphore(false), unix_key(-1), error(SystemSemaphore::NoError)
{
}

void SystemSemaphorePrivate::setErrorString(const QString& function)
{
	// EINVAL is handled in functions so they can give better error strings
	switch(errno)
	{
		case EPERM:
		case EACCES:
			errorString = QCoreApplication::tr("%1: permission denied", "SystemSemaphore").arg(function);
			error = SystemSemaphore::PermissionDenied;
			break;
		case EEXIST:
			errorString = QCoreApplication::tr("%1: already exists", "SystemSemaphore").arg(function);
			error = SystemSemaphore::AlreadyExists;
			break;
		case ENOENT:
			errorString = QCoreApplication::tr("%1: doesn't exists", "SystemSemaphore").arg(function);
			error = SystemSemaphore::NotFound;
			break;
		case ERANGE:
		case ENOSPC:
			errorString = QCoreApplication::tr("%1: out of resources", "SystemSemaphore").arg(function);
			error = SystemSemaphore::OutOfResources;
			break;
		default:
			errorString = QCoreApplication::tr("%1: unknown error %2", "SystemSemaphore").arg(function).arg(errno);
			error = SystemSemaphore::UnknownError;
#if defined SYSTEMSEMAPHORE_DEBUG
			qDebug() << errorString << "key" << key << "errno" << errno << EINVAL;
#endif
	}
}

key_t SystemSemaphorePrivate::handle(SystemSemaphore::AccessMode mode)
{
	if(key.isEmpty())
	{
		errorString = QCoreApplication::tr("%1: key is empty", "SystemSemaphore").arg(QLatin1String("SystemSemaphore::handle:"));
		error = SystemSemaphore::KeyError;
		return -1;
	}

	// ftok requires that an actual file exists somewhere
	if(unix_key != -1)
		return unix_key;

	// Create the file needed for ftok
	int built = createUnixKeyFile(fileName);
	if(built == -1)
	{
		errorString = QCoreApplication::tr("%1: unable to make key", "SystemSemaphore").arg(QLatin1String("SystemSemaphore::handle:"));
		error = SystemSemaphore::KeyError;
		return -1;
	}
	createdFile = (built == 1);

	// Get the unix key for the created file
	unix_key = ftok(QFile::encodeName(fileName).constData(), 'Q');
	if(unix_key == -1)
	{
		errorString = QCoreApplication::tr("%1: ftok failed", "SystemSemaphore").arg(QLatin1String("SystemSemaphore::handle:"));
		error = SystemSemaphore::KeyError;
		return -1;
	}

	// Get semaphore
	semaphore = semget(unix_key, 1, 0666 | IPC_CREAT | IPC_EXCL);
	if(semaphore == -1)
	{
		if(errno == EEXIST)
			semaphore = semget(unix_key, 1, 0666 | IPC_CREAT);
		if(semaphore == -1)
		{
			setErrorString(QLatin1String("SystemSemaphore::handle"));
			cleanHandle();
			return -1;
		}
	}
	else
	{
		createdSemaphore = true;
		// Force cleanup of file, it is possible that it can be left over from a crash
		createdFile = true;
	}

	if(mode == SystemSemaphore::Create)
	{
		createdSemaphore = true;
		createdFile = true;
	}

	// Created semaphore so initialize its value.
	if(createdSemaphore && initialValue >= 0)
	{
		_t_semun init_op;
		init_op.val = initialValue;
		if(semctl(semaphore, 0, SETVAL, init_op) == -1)
		{
			setErrorString(QLatin1String("SystemSemaphore::handle"));
			cleanHandle();
			return -1;
		}
	}

	return unix_key;
}

void SystemSemaphorePrivate::cleanHandle()
{
	unix_key = -1;

	// remove the file if we made it
	if(createdFile)
	{
		QFile::remove(fileName);
		createdFile = false;
	}

	if(createdSemaphore)
	{
		if(semaphore != -1)
		{
			if(semctl(semaphore, 0, IPC_RMID, 0) == -1)
			{
				setErrorString(QLatin1String("SystemSemaphore::cleanHandle"));
#if defined SYSTEMSEMAPHORE_DEBUG
				qDebug() << QLatin1String("SystemSemaphore::cleanHandle semctl failed.");
#endif
			}
			semaphore = -1;
		}
		createdSemaphore = false;
	}
}

bool SystemSemaphorePrivate::modifySemaphore(int count, int timeout)
{
	if(handle() == -1)
		return false;

	struct sembuf operation;
	operation.sem_num = 0;
	operation.sem_op = count;
	operation.sem_flg = SEM_UNDO;
	struct timespec ts_timeout;
	if(timeout >= 0)
	{
		ts_timeout.tv_sec = timeout / 1000;
		ts_timeout.tv_nsec = (timeout - ts_timeout.tv_sec * 1000) / 1000000;
	}
	if(semtimedop(semaphore, &operation, 1, (timeout < 0 ? 0 : &ts_timeout)) == -1)
	{
		// Timeout
		if(errno == EAGAIN)
			return false;
		// If the semaphore was removed be nice and create it and then modifySemaphore again
		if(errno == EINVAL || errno == EIDRM)
		{
			semaphore = -1;
			cleanHandle();
			handle();
			return modifySemaphore(count, timeout);
		}
		setErrorString(QLatin1String("SystemSemaphore::modifySemaphore"));
#if defined SYSTEMSEMAPHORE_DEBUG
		qDebug() << QLatin1String("SystemSemaphore::modify failed") << count << semctl(semaphore, 0, GETVAL) << errno << EIDRM << EINVAL;
#endif
		return false;
	}

	return true;
}
