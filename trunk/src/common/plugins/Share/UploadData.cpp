/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2014 IMPOMEZIA <schat@impomezia.com>
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

#include <QFileInfo>

#include "QtEscape.h"
#include "sglobal.h"
#include "UploadData.h"

#include <QDebug>

UploadData::UploadData(const ChatId &roomId, const QList<QUrl> &urls, int type)
  : reply(0)
  , m_roomId(roomId)
  , m_type(type)
  , m_urls(urls)
{
  m_id.init(ObjectId::gen());

  if (type == Local) {
    if (urls.size() == 1)
      m_title = QFileInfo(urls.first().toLocalFile()).fileName();
    else
      m_title = QObject::tr("Upload %n files", "", urls.size());
  }
  else {
    if (urls.size() == 1)
      m_title = urls.first().toString();
    else
      m_title = QObject::tr("Upload %n links", "", urls.size());
  }
}


QVariantMap UploadData::data() const
{
  QVariantMap data;
  data.insert(LS("roomId"), m_roomId.toString());
  data.insert(LS("id"),     m_id.toString());
  data.insert(LS("type"),   m_type);
  data.insert(LS("title"),  m_title);

  return data;
}


void UploadData::setDesc(const QString &desc)
{
  m_desc = Qt::escape(desc);
  qDebug() << m_desc;
}
