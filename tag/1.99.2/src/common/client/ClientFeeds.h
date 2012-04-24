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

#ifndef CLIENTFEEDS_H_
#define CLIENTFEEDS_H_

#include <QObject>
#include <QVariant>

#include "schat.h"

class FeedPacket;

namespace Hooks
{
  class Feeds;
}

class SCHAT_EXPORT ClientFeeds : public QObject
{
  Q_OBJECT

public:
  ClientFeeds(QObject *parent = 0);
  bool add(const QByteArray &id, const QString &name, const QVariantMap &json);
  bool clear(const QByteArray &id, const QString &name);
  bool get(const QByteArray &id, const QString &name);
  bool headers(const QByteArray &id);
  bool query(const QByteArray &id, const QString &name, const QVariantMap &json);
  bool remove(const QByteArray &id, const QString &name);
  bool update(const QByteArray &id, const QString &name, const QVariantMap &json);
  inline Hooks::Feeds *hooks() const { return m_hooks; }

private slots:
  void notice(int type);

private:
  FeedPacket *m_packet;     ///< Текущий прочитанный пакет.
  Hooks::Feeds *m_hooks;    ///< Хуки.
};

#endif /* CLIENTFEEDS_H_ */