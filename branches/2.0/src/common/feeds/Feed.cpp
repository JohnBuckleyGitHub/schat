/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#include <QDebug>

#include "feeds/Feed.h"
#include "net/SimpleID.h"


/*!
 * Конструктор создающий пустой объект.
 */
FeedHeader::FeedHeader()
  : time(0)
{
}


/*!
 * Преобразование заголовка в строку.
 *
 * Если фид содержит отметку времени отличную от 0 то она будет добавлена
 * к имени фида через символ двоеточия.
 */
QString FeedHeader::toString() const
{
  QString out = name;
  if (time != 0)
    out += ":" + QString::number(time);

  return out;
}


/*!
 * Установка имени фида.
 *
 * Имя не может быть пустым и содержать символ запятой.
 * Если имя содержит символа двоеточия, то текст после него преобразуется в отметку времени.
 *
 * \param Имя фида.
 * \return \b false если имя фида некорректно.
 */
bool FeedHeader::setName(const QString &name)
{
  if (name.isEmpty())
    return false;

  if (name.contains(','))
    return false;

  int index = name.indexOf(':');
  if (index != -1) {
    bool ok = false;
    time = name.mid(index + 1).toLongLong(&ok);
    if (!ok)
      return false;

    this->name = name.left(index);
    if (this->name.isEmpty())
      return false;
  }
  else
    this->name = name;

  return true;
}

Feed::Feed()
{
}


Feed::Feed(const QString &name)
{
  setName(name);
}


/*!
 * Возвращает \b true если данные фида корректны.
 */
bool Feed::isValid() const
{
  if (m_header.name.isEmpty())
    return false;

  if (SimpleID::typeOf(m_id) != SimpleID::ChannelId)
    return false;

  return true;
}
