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

#ifndef USER_H_
#define USER_H_

#include <QHash>
#include <QSharedPointer>
#include <QStringList>
#include <QVariant>

#include "acl/Groups.h"
#include "Gender.h"
#include "schat.h"

/*!
 * Пользователь.
 *
 * \deprecated Этот класс является устаревшим.
 */
class SCHAT_EXPORT User
{
public:
  /// Ограничения.
  enum Limits {
    MinNickLengh = 3,     ///< Минимальная длина ника.
    MaxNickLength = 20,   ///< Максимальная длина ника.
    MaxByeMsgLength = 128 ///< Максимальная длина сообщения при выходе.
  };

  User();
  User(const QString &nick);
  User(const User *other);
  virtual ~User();
  inline bool isValid() const { return m_valid; }
  inline quint8 serverNumber() const { return m_serverNumber; }
  inline void setServerNumber(quint8 number) { m_serverNumber = number; }
  void clear();

  // m_id.
  bool setId(const QByteArray &id);
  inline QByteArray id() const { return m_id; }

  // m_nick.
  bool setNick(const QString &nick);
  inline QString nick() const { return m_nick; }
  static bool isValidNick(const QString &nick);

  inline const Gender& gender() const { return m_gender; }
  inline Gender& gender() { return m_gender; }

  // m_channels.
  bool addChannel(const QByteArray &id);
  bool removeChannel(const QByteArray &id);
  inline bool isChannel(const QByteArray &id) { return m_channels.contains(id); }
  inline int channelsCount() const { return m_channels.size(); }
  inline QByteArray channel() const { return m_channels.at(0); }
  inline QList<QByteArray> channels() const { return m_channels; }

  // network.
  inline QString host() const { return m_host; }
  inline QString userAgent() const { return m_userAgent; }
  inline void setHost(const QString &adderss) { m_host = adderss; }
  inline void setUserAgent(const QString &agent) { m_userAgent = agent; }

  // m_status
  bool setStatus(const QString &text);
  inline int status() const { return m_status; }
  inline QString statusToString() { return statusToString(m_status, m_statuses.value(m_status)); }
  inline void setStatus(int status) { m_status = status; }
  QString statusText(int status = -1) const;
  static QString statusToString(int status, const QString &text);
  void setStatus(int status, const QString &text);

  // groups.
  inline const Groups& groups() const { return m_groups; }
  inline Groups& groups() { return m_groups; }

  // m_account
  inline QString account() const { return m_account; }
  inline void setAccount(const QString &account) { m_account = account; }

protected:
  inline bool validate(bool valid) { if (valid) return true; else m_valid = false; return false; }

  bool m_valid;                         ///< true все данные корректны.
  Groups m_groups;                      ///< Группы пользователя.
  Gender m_gender;                      ///< Пол и цвет иконки.
  int m_status;                         ///< Код статуса.
  QByteArray m_id;                      ///< Идентификатор пользователя.
  QHash<int, QString> m_statuses;       ///< Статусы.
  QList<QByteArray> m_channels;         ///< Каналы.
  QString m_account;                    ///< Зарегистрированное имя пользователя.
  QString m_host;                       ///< Адрес пользователя.
  QString m_nick;                       ///< Ник пользователя.
  QString m_userAgent;                  ///< User Agent пользователя.
  quint8 m_serverNumber;                ///< Номер сервера.
};

typedef QSharedPointer<User> ClientUser;

#endif /* USER_H_ */
