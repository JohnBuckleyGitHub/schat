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
#include <QFileInfo>

#include "ChatCore.h"
#include "SendFileTransaction.h"
#include "sglobal.h"

namespace SendFile {

Transaction::Transaction(const QByteArray &dest, const QString &file)
  : m_user(dest)
  , m_role(SenderRole)
{
  m_id = ChatCore::randomId();

  addLocalFile(file);
}


bool Transaction::addLocalFile(const QString &name)
{
  if (!QFile::exists(name))
    return false;

  QFileInfo info(name);
  m_file.name = info.absoluteFilePath();
  m_file.size = info.size();
  return true;
}


bool Transaction::isValid() const
{
  if (m_file.name.isEmpty())
    return false;

  return true;
}


QString Transaction::fileName() const
{
  return QFileInfo(m_file.name).fileName();
}


/*!
 * Формирование запроса с информацией о файлах для отправки получателю.
 */
QVariantMap Transaction::toReceiver() const
{
  QVariantMap json;
  if (!isValid())
    return json;

  json[LS("action")] = LS("file");
  json[LS("name")]   = fileName();
  json[LS("size")]   = m_file.size;

  return json;
}

} // namespace SendFile
