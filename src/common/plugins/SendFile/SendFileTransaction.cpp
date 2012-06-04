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
#include <QHostAddress>

#include "SendFileTransaction.h"
#include "sglobal.h"

namespace SendFile {

Hosts::Hosts(const QVariantList &json)
  : m_externalPort(0)
  , m_internalPort(0)
{
  if (json.size() != 4)
    return;

  m_externalAddress = json.at(0).toString();
  m_externalPort    = json.at(1).toInt();
  m_internalAddress = json.at(2).toString();
  m_internalPort    = json.at(3).toInt();
}


bool Hosts::isValid() const
{
  if (!m_externalPort)
    return false;

  if (!m_internalPort)
    return false;

  if (QHostAddress(m_externalAddress).isNull())
    return false;

  if (QHostAddress(m_internalPort).isNull())
    return false;

  return true;
}


QVariantList Hosts::toJSON() const
{
  QVariantList out;
  if (!isValid())
    return out;

  out.append(m_externalAddress);
  out.append(m_externalPort);
  out.append(m_internalAddress);
  out.append(m_internalPort);
  return out;
}


Transaction::Transaction(const QByteArray &dest, const QByteArray &id, const QString &file)
  : m_id(id)
  , m_user(dest)
  , m_role(SenderRole)
{
  addLocalFile(file);
}


Transaction::Transaction(const QByteArray &sender, const QByteArray &id, const QVariantMap &data)
  : m_id(id)
  , m_user(sender)
  , m_role(ReceiverRole)
{
  m_remote = Hosts(data.value(LS("hosts")).toList());
  if (!m_remote.isValid())
    return;

  m_file.name = data.value(LS("name")).toString();
  m_file.size = data.value(LS("size")).toLongLong();
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

  json[LS("name")]  = fileName();
  json[LS("size")]  = m_file.size;
  json[LS("hosts")] = m_local.toJSON();

  return json;
}

} // namespace SendFile
