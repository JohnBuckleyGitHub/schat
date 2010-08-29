/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#ifndef DAEMONSERVICE_H_
#define DAEMONSERVICE_H_

#include <QDataStream>
#include <QTcpSocket>
#include <QTimer>

#include "AbstractPeer.h"
#include "protocol.h"

class AbstractProfile;
class PacketReader;

/*!
 * \brief Универсальный класс, обслуживающий клиентов.
 *
 * При наличии валидного сокета, инициализируется сокет.
 */
class DaemonService : public AbstractPeer
{
  Q_OBJECT

public:
  DaemonService(QTcpSocket *socket, QObject *parent = 0);
  QString host() const;
  inline void sendPrivateMessage(quint8 flag, const QString &nick, const QString &message) { sendPacket(OpcodePrivateMessage, flag, nick, parseCmd(message)); }
  void accessDenied(quint16 reason = 0);
  void accessGranted(quint16 numeric = 0);
  void quit(bool kill = false);
  void sendNumerics(const QList<quint8> &numerics);
  void sendServerMessage(const QString &msg);
  void sendSyncUsersEnd();

signals:
  void greeting(const QStringList &list, quint8 flag);
  void leave(const QString &nick, quint8 flag, const QString &err);
  void message(const QString &channel, const QString &sender, const QString &message);
  void newBye(const QString &nick, const QString &bye);
  void newNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void newProfile(quint8 gender, const QString &nick, const QString &name);
  void newUser(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void relayMessage(const QString &channel, const QString &sender, const QString &message);
  void universal(quint16 sub, const QList<quint32> &data1, const QStringList &data2, quint8 numeric);
  void universal(quint16 sub, const QString &nick, const QList<quint32> &data1, const QStringList &data2);
  void universalLite(quint16 sub, const QList<quint32> &data1);
  void userLeave(const QString &nick, const QString &bye, quint8 flag);

public slots:
  bool sendUniversal(quint16 sub, const QList<quint32> &data1, const QStringList &data2);
  inline void sendLinkLeave(quint8 numeric, const QString &network, const QString &ip)     { sendPacket(OpcodeLinkLeave, numeric, network, ip); }
  inline void sendNewLink(quint8 numeric, const QString &network, const QString &ip)       { sendPacket(OpcodeNewLink, numeric, network, ip); }
  inline void sendUserLeave(const QString &nick, const QString &bye, quint8 flag)          { sendPacket(OpcodeUserLeave, flag, nick, bye); }
  void sendMessage(const QString &sender, const QString &message);
  void sendNewNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void sendNewProfile(quint8 gender, const QString &nick, const QString &name);
  void sendNewUser(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void sendRelayMessage(const QString &channel, const QString &sender, const QString &message);
  void sendSyncBye(const QString &nick, const QString &bye);

protected:
  void readPacket(int pcode, const QByteArray &block);

private slots:
  void disconnected();
  void ping();

private:
  bool opcodeGreeting(const PacketReader &reader);
  bool sendPacket(int pcode, int flag, const QString &nick, const QString &message);
  QString parseCmd(const QString &message) const;
  quint16 verifyGreeting(quint16 version);
  void messagePacket(const PacketReader &reader);
  void opcodeByeMsg(const PacketReader &reader);
  void opcodeNewNick(const PacketReader &reader);
  void opcodeNewProfile(const PacketReader &reader);
  void opcodeNewUser(const PacketReader &reader);
  void opcodeRelayMessage(const PacketReader &reader);
  void opcodeSyncByeMsg(const PacketReader &reader);
  void opcodeUniversal(const PacketReader &reader);
  void opcodeUserLeave(const PacketReader &reader);

  AbstractProfile *m_profile;
  bool m_kill;
  int m_pings;
  QString m_error;
  QTimer m_ping;
  quint8 m_flag;
  quint8 m_numeric;
};

/*! \fn void DaemonService::sendPrivateMessage(quint8 flag, const QString &nick, const QString &message)
 * \brief Отправка пакета \b OpcodePrivateMessage.
 *
 * \param flag Флаг эха, если 1 то это подтверждение отправки сообщения.
 * \param nick Ник, отправившего сообщение (flag = 0), ник того кому предназначается сообщение (flag = 1).
 * \param message Сообщение.
 */

/*! \fn void DaemonService::message(const QString &channel, const QString &sender, const QString &message)
 * \brief Уведомление о новом сообщении от пользователя.
 *
 * \param channel Канал/ник для кого предназначено сообщение (пустая строка - главный канал).
 * \param sender Ник отправителя сообщения.
 * \param message Сообщение.
 */

/*! \fn void DaemonService::sendLinkLeave(quint8 numeric, const QString &network, const QString &ip)
 * \brief Отправка пакета \b OpcodeLinkLeave.
 *
 * \param numeric Номер сервера подключившегося к сети.
 * \param network Название сети.
 * \param ip Адрес сервера
 */

/*! \fn void DaemonService::sendNewLink(quint8 numeric, const QString &network, const QString &ip)
 * \brief Отправка пакета \b OpcodeNewLink.
 *
 * \param numeric Номер сервера подключившегося к сети.
 * \param network Название сети.
 * \param ip Адрес сервера
 */

#endif /*DAEMONSERVICE_H_*/
