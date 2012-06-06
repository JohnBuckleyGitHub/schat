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

#include "net/packets/MessageNotice.h"
#include "plugins/ChatPlugin.h"
#include "SendFileTransaction.h"

class ChatView;
class SendFileTr;

namespace SendFile {
  class Thread;
}

class SendFilePluginImpl : public ChatPlugin
{
  Q_OBJECT

public:
  SendFilePluginImpl(QObject *parent);
  ~SendFilePluginImpl();

  bool sendFile(const QByteArray &dest, const QString &file);
  void read(const MessagePacket &packet);

signals:
  void accepted(const QString &id, const QString &fileName);
  void cancelled(const QString &id);

private slots:
  void init(ChatView *view);
  void loadFinished(ChatView *view);
  void openUrl(const QUrl &url);

private:
  MessagePacket reply(const SendFileTransaction &transaction, const QString &text);
  quint16 getPort() const;
  SendFile::Hosts localHosts() const;

  void accept(const MessagePacket &packet);
  void cancel(const MessagePacket &packet);
  void incomingFile(const MessagePacket &packet);

  // Реакция на действия пользователя.
  void cancel(const QByteArray &id);
  void saveAs(const QByteArray &id);

  QHash<QByteArray, SendFileTransaction> m_transactions; ///< Отправленные файлы и входящие файлы.
  quint16 m_port;                                        ///< Порт который будет использоваться для передачи файлов.
  SendFile::Thread *m_thread;                            ///< Рабочий поток передачи файлов.
  SendFileTr *m_tr;                                      ///< Класс перевода строк.
};

#endif /* SENDFILEPLUGIN_P_H_ */
