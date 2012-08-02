#include <QCoreApplication>
#include "SpellBackend.h"
#include "HunSpellchecker.h"

SpellBackend* SpellBackend::FInstance = NULL;

SpellBackend* SpellBackend::instance() 
{
	if (!FInstance) 
	{
		FInstance = new HunspellChecker();
	}
	return FInstance;
}

SpellBackend::SpellBackend() : QObject(QCoreApplication::instance())
{

}

SpellBackend::~SpellBackend()
{
}

bool SpellBackend::available() const
{
	return false;
}

bool SpellBackend::writable() const
{
	return false;
}

bool SpellBackend::isCorrect(const QString &AWord)
{
	Q_UNUSED(AWord);
	return true;
}

QList<QString> SpellBackend::suggestions(const QString &AWord)
{
	Q_UNUSED(AWord);
	return QList<QString>();
}

bool SpellBackend::add(const QString &AWord)
{
	Q_UNUSED(AWord);
	return false;
}

QList< QString > SpellBackend::dictionaries()
{
	return QList<QString>();
}

void SpellBackend::setLangs(const QList<QString> &dicts)
{
	//return dicts;
}

QString SpellBackend::actuallLang()
{
	return QString();
}
