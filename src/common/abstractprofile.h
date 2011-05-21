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

#ifndef ABSTRACTPROFILE_H_
#define ABSTRACTPROFILE_H_

#include <QObject>
#include <QStringList>

/*!
 * \brief Абстрактный класс, хранящий информацию о пользователе.
 *
 * Класс хранит в себе всю информацию о пользователе и предоставляет все необходимые методы для манипуляций с профилем.
 */
class AbstractProfile : public QObject {
  Q_OBJECT

public:
  enum {
    MaxNickLength = 24,
    MaxNameLength = 130,
    MaxByeMsgLength = 130
  };
  enum pack {
    Nick,
    FullName,
    ByeMsg,
    UserAgent,
    Host,
    Gender
  };

  AbstractProfile(const AbstractProfile &other);
  AbstractProfile(const QStringList &list, QObject *parent = 0);
  AbstractProfile(QObject *parent = 0);
  AbstractProfile& operator=(const AbstractProfile &other);
  inline bool isFemale() const                     { return !m_male; }
  inline bool isMale() const                       { return m_male; }
  inline bool isValidNick() const                  { return isValidNick(m_nick); }
  inline bool isValidUserAgent() const             { return isValidUserAgent(m_userAgent); }
  inline QString byeMsg() const                    { return m_byeMsg; }
  inline QString fullName() const                  { return m_fullName; }
  inline QString gender() const                    { if (m_male) return "male"; else return "female"; }
  inline QString host() const                      { return m_host; }
  inline QString nick() const                      { return m_nick; }
  inline QString userAgent() const                 { return m_userAgent; }
  inline quint32 status() const                    { return m_status; }
  inline quint8 genderNum() const                  { if (m_male) return 0; else return 1; }
  inline static QString gender(bool male)          { if (male) return "male"; else return "female"; }
  inline static QString gender(quint8 g)           { if (g) return "female"; else return "male"; }
  inline static quint8 genderNum(const QString &gender) { if (gender == "female") return 1; else return 0; }
  inline void setByeMsg(const QString &msg)        { m_byeMsg = msg.simplified().left(MaxByeMsgLength); }
  inline void setFullName(const QString &fullName) { m_fullName = fullName.simplified().left(MaxNameLength); }
  inline void setGender(bool male)                 { m_male = male; }
  inline void setGender(const QString &gender)     { if (gender == "female") m_male = false; else m_male = true; }
  inline void setGender(quint8 _gender)            { setGender(gender(_gender)); }
  inline void setHost(const QString &host)         { m_host = host; }
  inline void setNick(const QString &nick)         { m_nick = nick.simplified().left(MaxNickLength); }
  inline void setUserAgent(const QString &agent)   { m_userAgent = agent.simplified(); }
  QStringList pack() const;
  static bool isValidNick(const QString &nick);
  static bool isValidUserAgent(const QString &a);
  static QString defaultNick();
  void setStatus(quint32 status);
  void unpack(const QStringList &list);


signals:
  void statusChanged(quint32 newStatus);

private:
  bool m_male;
  QString m_byeMsg;
  QString m_fullName;
  QString m_host;
  QString m_nick;
  QString m_userAgent;
  quint32 m_status;
};

#endif /*ABSTRACTPROFILE_H_*/
