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

#include "k8json/k8json.h"
#include "SimpleJSon.h"

QByteArray SimpleJSon::generate(const QVariant &data)
{
  QByteArray res;
  K8JSON::generate(res, data, 0);
  if (res == "{}")
    return QByteArray();

  return res;
}

QString SimpleJSon::quote(const QString &text)
{
  return K8JSON::quote(text);
}


QVariant SimpleJSon::parse(const QByteArray &data)
{
  QVariant res;
  if (data.isEmpty())
    return res;

  int size = data.size();
  K8JSON::parseRecord(res, reinterpret_cast<const uchar *>(data.constData()), &size);
  return res;
}
