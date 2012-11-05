/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOCALCLIENTSERVICE_H_
#define LOCALCLIENTSERVICE_H_

#include <QLocalSocket>
#include <QPointer>
#include <QTimer>

/*!
 * \brief Клиентский сокет для коммуникации между программой управления сервером
 * и сервером.
 */
class LocalClientService : public QObject
{
  Q_OBJECT

public:
  enum Reason {
    Start,
    Stop
  };

  LocalClientService(QObject *parent = 0);

signals:
  void notify(LocalClientService::Reason reason);

public slots:
  void connectToServer();
  void exit();
  void leave();

private slots:
  void connected();
  void disconnected();
  void readyRead();

private:
  bool send(quint16 opcode);
  void unknownOpcode();

  QDataStream m_stream;
  QPointer<QLocalSocket> m_socket;
  QString m_key;
  QTimer m_reconnectTimer;
  quint16 m_nextBlockSize;
  quint16 m_opcode;
};

#endif /*LOCALCLIENTSERVICE_H_*/
