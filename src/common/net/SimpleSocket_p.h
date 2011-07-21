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

#ifndef SIMPLESOCKET_P_H_
#define SIMPLESOCKET_P_H_

#include "net/SimpleSocket.h"

class SimpleSocketPrivate
{
  Q_DECLARE_PUBLIC(SimpleSocket);

public:
  /// Состояние таймера.
  enum TimerState {
    WaitingConnect,   ///< Ожидание установки соединения.
    WaitingHandshake, ///< Ожидание авторизации.
    Idling,           ///< Ожидание активности.
    WaitingReply,     ///< Ожидания ответа на пакет.
    Leaving           ///< Состояние завершения соединения.
  };

  SimpleSocketPrivate();
  virtual ~SimpleSocketPrivate();
  bool readTransport();
  bool transmit(const QByteArray &packet, quint8 options = 0x0, quint8 type = Protocol::GenericTransport, quint8 subversion = Protocol::V4_0);
  bool transmit(const QList<QByteArray> &packets, quint8 options = 0x0, quint8 type = Protocol::GenericTransport, quint8 subversion = Protocol::V4_0);
  void releaseSocket();
  void setTimerState(TimerState state);
  void sslHandshake(int option);
  void timerEvent();

  bool authorized;                     ///< true после авторизации.
  bool release;                        ///< true если сокет находится в состоянии закрытия.
  bool serverSide;                     ///< true если сокет работает на стороне сервера, иначе на строне клиента.
  bool sslAvailable;                   ///< true если доступна поддержка SSL.
  QBasicTimer *timer;                  ///< Таймер обслуживающий соединение.
  QByteArray readBuffer;               ///< Буфер чтения виртуальных пакетов.
  QByteArray sendBuffer;               ///< Буфер отправки виртуальных пакетов.
  QByteArray txBuffer;                 ///< Буфер отправки транспортных пакетов.
  QByteArray userId;                   ///< Идентификатор клиента.
  QDataStream *readStream;             ///< Поток чтения виртуальных пакетов.
  QDataStream *sendStream;             ///< Поток отправки виртуальных пакетов.
  QDataStream *rxStream;               ///< Поток чтения транспортных пакетов.
  QDataStream *txStream;               ///< Поток отправки транспортных пакетов.
  qint64 timestamp;                    ///< Отметка времени.
  QList<QByteArray> readQueue;         ///< Список прочитанных виртуальных пакетов.
  QList<quint64> deliveryConfirm;      ///< Список sequence пакетов, используется сервером для формирования отчёта о доставке или клиентом для проверки доставки.
  quint32 nextBlockSize;               ///< Размер следующего блока данных.
  quint64 id;                          ///< Идентификационный номер сокета.
  quint64 rx;                          ///< Счётчик полученных (receive) байт.
  quint64 rxSeq;                       ///< sequence полученных пакетов.
  quint64 tx;                          ///< Счётчик отправленных (transmit) байт.
  quint64 txSeq;                       ///< sequence отправленных пакетов.
  SimpleSocket *q_ptr;                 ///< D указатель.
  TimerState timerState;               ///< Состояние таймера.
};

#endif /* SIMPLESOCKET_P_H_ */
