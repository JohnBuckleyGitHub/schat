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

#ifndef SIMPLESOCKET_H_
#define SIMPLESOCKET_H_

#if defined(SCHAT_NO_SSL)
  #include <QTcpSocket>
  #define QSslSocket QTcpSocket
#else
  #include <QSslSocket>
#endif

#include <QTime>

#include "net/Protocol.h"

class QBasicTimer;

/*!
 * Наследник QTcpSocket выполняющий все операции на транспортном уровне протокола чата.
 */
class SimpleSocket : public QSslSocket
{
  Q_OBJECT

public:
  /// Состояние таймера.
  enum TimerState {
    WaitingConnect,   ///< Ожидание установки соединения.
    WaitingHandshake, ///< Ожидание авторизации.
    Idling,           ///< Ожидание активности.
    WaitingReply,     ///< Ожидания ответа на пакет.
    Leaving           ///< Состояние завершения соединения.
  };

  SimpleSocket(quint64 id, QObject *parent = 0);
  ~SimpleSocket();
  bool send(const QByteArray &packet);
  bool send(const QList<QByteArray> &packets);
  bool setSocketDescriptor(int socketDescriptor);
  inline bool isAuthorized() const { return m_authorized; }
  inline bool isReady() const { if (state() == QTcpSocket::ConnectedState) return true; return false; }
  inline QByteArray userId() const { return m_userId; }
  inline QDataStream *sendStream() { return m_sendStream; }
  inline qint64 timestamp() const { return m_timestamp; }
  inline quint64 rx() const { return m_rx; }
  inline quint64 tx() const { return m_tx; }
  inline void setTimestamp(qint64 timestamp) { m_timestamp = timestamp; }
  void leave();
  void setAuthorized(const QByteArray &userId);

signals:
  void allDelivered(quint64 id);
  void newPackets(quint64 id, const QList<QByteArray> &packets);
  void released(quint64 id);
  void requestAuth(quint64 id);

protected:
  virtual void newPacketsImpl();
  void timerEvent(QTimerEvent *event);

protected slots:
  void connectToHostImplementation(const QString &hostName, quint16 port, OpenMode openMode = ReadWrite);

private slots:
  void connected();
  void disconnected();
  void error(QAbstractSocket::SocketError socketError);
  void readyRead();

  #if !defined(SCHAT_NO_SSL)
  void encrypted();
  void sslErrors(const QList<QSslError> &errors);
  #endif

protected:
  QByteArray m_readBuffer;               ///< Буфер чтения виртуальных пакетов.
  QDataStream *m_readStream;             ///< Поток чтения виртуальных пакетов.
  QDataStream *m_sendStream;             ///< Поток отправки виртуальных пакетов.
  QList<QByteArray> m_readQueue;         ///< Список прочитанных виртуальных пакетов.

private:
  bool readTransport();
  bool transmit(const QByteArray &packet, quint8 options = 0x0, quint8 type = Protocol::GenericTransport, quint8 subversion = Protocol::V4_0);
  bool transmit(const QList<QByteArray> &packets, quint8 options = 0x0, quint8 type = Protocol::GenericTransport, quint8 subversion = Protocol::V4_0);
  void release();
  void setTimerState(TimerState state);
  void sslHandshake(int option);

  bool m_authorized;                     ///< true после авторизации.
  bool m_release;                        ///< true если сокет находится в состоянии закрытия.
  bool m_serverSide;                     ///< true если сокет работает на стороне сервера, иначе на строне клиента.
  bool m_sslAvailable;                   ///< true если доступна поддержка SSL.
  const quint64 m_id;                    ///< Идентификационный номер сокета.
  QBasicTimer *m_timer;                  ///< Таймер обслуживающий соединение.
  QByteArray m_sendBuffer;               ///< Буфер отправки виртуальных пакетов.
  QByteArray m_txBuffer;                 ///< Буфер отправки транспортных пакетов.
  QByteArray m_userId;                   ///< Идентификатор клиента.
  QDataStream *m_rxStream;               ///< Поток чтения транспортных пакетов.
  QDataStream *m_txStream;               ///< Поток отправки транспортных пакетов.
  qint64 m_timestamp;                    ///< Отметка времени.
  QList<quint64> m_deliveryConfirm;      ///< Список sequence пакетов, используется сервером для формирования отчёта о доставке или клиентом для проверки доставки.
  quint32 m_nextBlockSize;               ///< Размер следующего блока данных.
  quint64 m_rx;                          ///< Счётчик полученных (receive) байт.
  quint64 m_rxSeq;                       ///< sequence полученных пакетов.
  quint64 m_tx;                          ///< Счётчик отправленных (transmit) байт.
  quint64 m_txSeq;                       ///< sequence отправленных пакетов.
  TimerState m_timerState;               ///< Состояние таймера.
};

#endif /* SIMPLESOCKET_H_ */
