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

#ifndef SERVERUSER_H_
#define SERVERUSER_H_

#include <QList>

#include "User.h"

class AuthRequest;

class ServerUser : public User
{
public:
  ServerUser(const QByteArray &session, const QString &normalNick, const QByteArray &id, AuthRequestData *authRequestData, int workerId, quint64 socketId);
  inline int workerId() const { return m_workerId; }
  inline QByteArray session() const { return m_session; }
  inline QString normalNick() const { return m_normalNick; }
  inline quint64 socketId() const { return m_socketId; }
  inline void setNormalNick(const QString &nick) { m_normalNick = nick; }

  // m_users.
  bool addId(int type, const QByteArray &id);
  bool addUser(const QByteArray &id);
  bool removeUser(const QByteArray &id);
  inline bool isUser(const QByteArray &id) const { return m_users.contains(id); }
  inline QList<QByteArray> users() const { return m_users; }
  void addUsers(const QList<QByteArray> &users);
  void removeUsers(const QList<QByteArray> &users);

private:
  int m_workerId;            ///< Идентификатор объекта Worker.
  QByteArray m_session;      ///< Сессия.
  QList<QByteArray> m_users; ///< Список идентификаторов ассоциированных пользователей.
  QString m_normalNick;      ///< Нормализованный ник.
  quint64 m_socketId;        ///< Идентификатор сокета.
};

#endif /* SERVERUSER_H_ */
