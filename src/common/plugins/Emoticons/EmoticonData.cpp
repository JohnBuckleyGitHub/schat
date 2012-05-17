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

#include <QFile>
#include <QVariant>

#include "EmoticonData.h"

EmoticonData::EmoticonData(const QString &file, const QString &id, const QVariantList &data)
  : m_height(0)
  , m_width(0)
  , m_file(file)
  , m_id(id)
{
  if (data.size() < 3)
    return;

  m_width = data.at(0).toInt();
  m_height = data.at(1).toInt();

  for (int i = 2; i < data.size(); ++i) {
    if (!m_texts.contains(data.at(i).toString()))
      m_texts.append(data.at(i).toString());
  }
}


bool EmoticonData::isValid() const
{
  if (!QFile::exists(m_file))
    return false;

  if (m_width <= 0)
    return false;

  if (m_height <= 0)
    return false;

  if (m_texts.isEmpty())
    return false;

  return true;
}
