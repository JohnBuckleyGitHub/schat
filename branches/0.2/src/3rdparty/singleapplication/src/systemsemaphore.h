#ifndef SYSTEMSEMAPHORE_H
#define SYSTEMSEMAPHORE_H

#include <QtCore/QString>

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

class SystemSemaphorePrivate;
class SINGLEAPPLICATION_EXPORT SystemSemaphore
{
public:
	enum AccessMode
	{
		Open,
		Create
	};

	enum SystemSemaphoreError
	{
		NoError,
		PermissionDenied,
		KeyError,
		AlreadyExists,
		NotFound,
		OutOfResources,
		UnknownError
	};

	SystemSemaphore(const QString& key, int initialValue = 0, AccessMode mode = Open);
	~SystemSemaphore();

	void setKey(const QString& key, int initialValue = 0, AccessMode mode = Open);
	QString key() const;

	bool acquire();
	bool tryAcquire(int timeout);
	bool release(int n = 1);

	SystemSemaphoreError error() const;
	QString errorString() const;

private:
	Q_DISABLE_COPY(SystemSemaphore)

	SystemSemaphorePrivate* d;
};

#endif // SYSTEMSEMAPHORE_H
