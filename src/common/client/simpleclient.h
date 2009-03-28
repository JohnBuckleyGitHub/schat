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

#ifndef SIMPLECLIENT_H_
#define SIMPLECLIENT_H_

#include <QObject>
#include <QPair>

/*!
 * \brief Универсальный и полностью самодостаточный клиент чата.
 */
class SimpleClient : public QObject
{
  Q_OBJECT

public:
  /// Состояние подключения.
  enum State {
    UnconnectedState, ///< Нет подключения.
    ConnectingState,  ///< Идёт подключение.
    ConnectedState,   ///< Подключение установлено.
    ClosingState      ///< Подключение закрывается.
  };

  SimpleClient(QObject *parent = 0);
  ~SimpleClient();
  SimpleClient::State state() const;
  void link();
  void link(QPair<QString, quint16> remote);
  void setFullName(const QString &fullName);
  void setGender(quint8 gender);
  void setNick(const QString &nick);
  void setPassword(const QString &password);
  void setUserName(const QString &userName);

private slots:
  void connected();
  void disconnected();
  void readyRead();

private:
  void createSocket();

  class Private;
  Private* const d;
};


#endif /* SIMPLECLIENT_H_ */
