#ifndef SPELLBACKEND_H
#define SPELLBACKEND_H

#include <QList>
#include <QString>
#include <QObject>

class SpellBackend : public QObject
{
public:
	static SpellBackend* instance();
	virtual QList<QString> suggestions(const QString &AWord);
	virtual bool isCorrect(const QString &AWord);
	virtual bool add(const QString &AWord);
	virtual bool available() const;
	virtual bool writable() const;
	virtual QList<QString> dictionaries();
	virtual void setLangs(const QList<QString> &dicts);
	virtual QString actuallLang();
protected:
	SpellBackend();
	virtual ~SpellBackend();
private:
	static SpellBackend *FInstance;
};

#endif // SPELLBACKEND_H
