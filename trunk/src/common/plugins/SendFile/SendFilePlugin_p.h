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

#ifndef SENDFILEPLUGIN_P_H_
#define SENDFILEPLUGIN_P_H_

#include <QVariant>

#include "plugins/ChatPlugin.h"
#include "SendFileTransaction.h"

class ChatView;

class SendFilePluginImpl : public ChatPlugin
{
  Q_OBJECT

public:
  SendFilePluginImpl(QObject *parent);
  bool send(const QByteArray &dest, const QVariantMap &data, const QByteArray &id);
  bool sendFile(const QByteArray &dest, const QString &file);

private slots:
  void init(ChatView *view);
  void loadFinished(ChatView *view);

private:
  QHash<QByteArray, SendFileTransaction> m_outgoing; ///< Отправленные файлы.
};

#endif /* SENDFILEPLUGIN_P_H_ */
