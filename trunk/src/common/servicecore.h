/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#ifndef SERVICECORE_H_
#define SERVICECORE_H_

#include <QObject>
#include <QPointer>
#include <QTcpSocket>
#include <QTimer>

class AbstractProfile;
class AbstractRawPacket;

/*!
 * \brief Базовый класс для клиентского и серверного сервиса.
 */
class ServiceCore : public QObject
{
  Q_OBJECT

public:
  ServiceCore(QObject *parent = 0);
  bool isReady() const;
  bool rawSend(const AbstractRawPacket &packet);
  QString nick() const;

protected:
  AbstractProfile *m_profile;    ///< Профиль пользователя ассоциированный с данным соединением.
  bool m_accepted;               ///< \a true в случае успешной установки соедидения.
  QDataStream m_stream;          ///< Поток для чтения данный поступивших из сокета.
  qint64 m_rx;                   ///< Счётчик полученных (receive) байт.
  qint64 m_tx;                   ///< Счётчик отправленных (transmit) байт.
  QPointer<QTcpSocket> m_socket; ///< TCP сокет обслуживающий подключение.
  QTimer m_ping;                 ///< Ping-таймер.
  quint16 m_nextBlockSize;       ///< Размер следующего блока данных.
};


#endif /* SERVICECORE_H_ */
