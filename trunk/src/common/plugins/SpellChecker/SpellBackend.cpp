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

#include <QCoreApplication>
#include "SpellBackend.h"
#include "HunSpellChecker.h"

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
