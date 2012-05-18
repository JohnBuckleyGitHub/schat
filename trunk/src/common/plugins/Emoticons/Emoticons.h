/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#ifndef EMOTICONS_H_
#define EMOTICONS_H_

#include <QObject>

#include "EmoticonData.h"

class Extension;

class Emoticons : public QObject
{
  Q_OBJECT

public:
  Emoticons(QObject *parent = 0);
  bool load(Extension *extension);
  Emoticon get(const QString &key);
  inline const QString& index() const { return m_index; }
  QString find(const QString &text, int pos);

private:
  void add(Emoticon emoticon);
  void makeIndex();

  QMap<QString, Emoticon> m_emoticons; ///< Таблица смайлов.
  QString m_index;                     ///< Первые символы текстовых сокращений смайлов.
};

#endif /* EMOTICONS_H_ */
