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

#include <QStringList>

#include "plugins/Ver.h"
#include "sglobal.h"

Ver::Ver()
  : m_major(0)
  , m_minor(0)
  , m_patch(0)
{
}


Ver::Ver(const QString &version)
  : m_major(0)
  , m_minor(0)
  , m_patch(0)
{
  setVersion(version);
}


QString Ver::toString() const
{
  return QString::number(m_major) + LC('.') + QString::number(m_minor) + LC('.') + QString::number(m_patch);
}


void Ver::setVersion(const QString &version)
{
  QStringList split = version.split(LC('.'));
  if (split.size() > 0)
    m_major = split.at(0).toInt();

  if (split.size() > 1)
    m_minor = split.at(1).toInt();

  if (split.size() > 2)
    m_patch = split.at(2).toInt();
}
