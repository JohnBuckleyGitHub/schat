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

#include <QString>

/*!
 * Пользователь.
 */
class User
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
  virtual ~User() {}
  bool addChannel(const QByteArray &id);
  bool addTalk(const QByteArray &id);
  bool removeChannel(const QByteArray &id);
  bool removeTalk(const QByteArray &id);
  bool setId(const QByteArray &id);
  bool setNick(const QString &nick);
  inline bool isJoined(const QByteArray &id) { return m_channels.contains(id); }
  inline bool isValid() const                { return m_valid; }
  inline int channelCount() const            { return m_channels.size(); }
  inline QByteArray id() const               { return m_id; }
  inline QList<QByteArray> channels() const  { return m_channels; }
  inline QList<QByteArray> talks() const     { return m_talks; }
  inline QString nick() const                { return m_nick; }
  inline void clearChannels()                { m_channels.clear(); }
  inline void clearTalks()                   { m_talks.clear(); }
  static QString defaultNick();

private:
  inline bool validate(bool valid) { if (valid) return true; else m_valid = false; return false; }

  bool m_valid;                 ///< true все данные корректны.
  QByteArray m_id;              ///< Идентификатор пользователя.
  QList<QByteArray> m_channels; ///< Список каналов в которых находиться пользователь.
  QList<QByteArray> m_talks;    ///< Список идентификаторов пользователей, с которыми открыт приватный разговор.
  QString m_nick;               ///< Ник пользователя.
};

#endif /* USER_H_ */
