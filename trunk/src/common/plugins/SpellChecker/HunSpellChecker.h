/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
 * Copyright © 2012 Alexey Ivanov <alexey.ivanes@gmail.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
