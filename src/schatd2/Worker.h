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

#ifndef WORKER_H_
#define WORKER_H_

#include <QHash>
#include <QObject>

class Core;
class Server;
class SimpleSocket;

class Worker : public QObject
{
  Q_OBJECT

public:
  Worker(int id, Core *parent = 0);
  ~Worker();
  static void setDefaultSslConf(const QString &crtFile, const QString &keyFile);
  bool start();

protected:
  void customEvent(QEvent *event);

private slots:
  void newConnection(int socketDescriptor);
  void newPackets(quint64 id, const QList<QByteArray> &packets);
  void released(quint64 id);

private:
  const int m_id;                          ///< Идентификатор.
  Core *m_core;                            ///< Указатель на объект Core для посылки событий.
  QHash<quint64, SimpleSocket*> m_sockets; ///< Список сокетов.
  quint64 m_nextSocketId;                  ///< Идентификатор следующего сокета.
  Server *m_server;                        ///< Объект Server принимающий новые подключения.
};

#endif /* WORKER_H_ */
