#ifndef HUNSPELLCHECKER_H
#define HUNSPELLCHECKER_H

#include <QList>
#include <QString>
#include <QTextCodec>
#include <QMap>
#include <QDebug>

#include <hunspell/hunspell.hxx>
#include "SpellBackend.h"

class HunspellChecker : public SpellBackend
{
public:
	HunspellChecker();
	~HunspellChecker();
	virtual QList<QString> suggestions(const QString &AWord);
	virtual bool isCorrect(const QString &AWord);
	virtual bool add(const QString &AWord);
	virtual bool available() const;
	virtual bool writable() const;
	virtual QList<QString> dictionaries();
	virtual void setLangs(const QList<QString> &dicts);
	virtual QString actuallLang();

private:
	void loadHunspell(const QList<QString> &dicts);
	QMap <QString, Hunspell*> FHunSpellMap;
	QMap <QString, QByteArray> FDictionaryMap;
	QString dictPath;
	QString lang;
};

#endif
