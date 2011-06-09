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

#include <QSharedPointer>

#include "User.h"

class AuthRequest;
class AuthRequestData;
class QSqlQuery;

class ServerUser : public User
{
public:
  enum SqlColumns {
    IdColumn,
    UserIdColumn,
    NickColumn,
    NormalNickColumn,
    GenderColumn,
    HostColumn,
    UserAgentColumn
  };

  ServerUser(const QByteArray &session, const QString &normalNick, const QByteArray &id, AuthRequestData *authRequestData, int workerId, quint64 socketId);
  ServerUser(const QSqlQuery &query);
  ~ServerUser();
  inline bool isOnline() const { return m_online; }
  inline int workerId() const { return m_workerId; }
  inline QByteArray session() const { return m_session; }
  inline qint64 key() const { return m_key; }
  inline QString normalNick() const { return m_normalNick; }
  inline quint64 socketId() const { return m_socketId; }
  inline void setKey(qint64 key) { m_key = key; }
  inline void setNormalNick(const QString &nick) { m_normalNick = nick; }
  inline void setOnline(bool online) { m_online = online; }

  // m_users.
  bool addId(int type, const QByteArray &id);
  bool addUser(const QByteArray &id);
  bool removeUser(const QByteArray &id);
  inline bool isUser(const QByteArray &id) const { return m_users.contains(id); }
  inline QList<QByteArray> users() const { return m_users; }
  void addUsers(const QList<QByteArray> &users);
  void removeUsers(const QList<QByteArray> &users);

private:
  bool m_online;             ///< true если пользователь в сети.
  int m_workerId;            ///< Идентификатор объекта Worker.
  QByteArray m_session;      ///< Сессия.
  qint64 m_key;              ///< Ключ в таблице users.
  QList<QByteArray> m_users; ///< Список идентификаторов ассоциированных пользователей.
  QString m_normalNick;      ///< Нормализованный ник.
  quint64 m_socketId;        ///< Идентификатор сокета.
};

typedef QSharedPointer<ServerUser> ChatUser;

#endif /* SERVERUSER_H_ */
