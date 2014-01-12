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

#ifndef UPLOADDATA_H_
#define UPLOADDATA_H_

#include <QList>
#include <QUrl>
#include <QVariant>

#include "id/ChatId.h"

class QNetworkReply;

class UploadData
{
public:
  enum Type {
    Local,
    Remote
  };

  UploadData(const ChatId &roomId, const QList<QUrl> &urls, int type);
  inline const ChatId& id() const          { return m_id; }
  inline const ChatId& roomId() const      { return m_roomId; }
  inline const QList<QUrl>& urls() const   { return m_urls; }
  inline const QString& desc() const       { return m_desc; }
  inline const QString& title() const      { return m_title; }
  inline int type() const                  { return m_type; }
  QVariantMap data() const;
  void setDesc(const QString &desc);

  QNetworkReply *reply;

private:
  ChatId m_id;
  ChatId m_roomId;
  int m_type;
  QList<QUrl> m_urls;
  QString m_desc;
  QString m_title;
};

#endif /* UPLOADDATA_H_ */
