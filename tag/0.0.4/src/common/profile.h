/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef PROFILE_H_
#define PROFILE_H_

#include <QObject>
#include <QDataStream>
#include <QStringList>

class Profile : public QObject {
  Q_OBJECT
  
public:
  enum {
    MaxNickLength = 64,
    MaxNameLength = 255,
    MaxByeMsgLength = 255
  };  
  
  Profile(const QString &nick, const QString &fullName, quint8 sex, QObject *parent = 0);
  Profile(QObject *parent = 0);
  Profile(quint8 sex, const QStringList &list, QObject *parent = 0);
  
  bool fromList(const QStringList &list);
  bool isValidUserAgent() const;
  inline bool isValidNick() const                  { return isValidNick(m_nick); }
  inline QString byeMsg()                          { return m_byeMsg; }
  inline QString fullName() const                  { return m_fullName; } 
  inline QString host()                            { return m_host; }
  inline QString nick() const                      { return m_nick; } 
  inline QString userAgent() const                 { return m_userAgent; }
  inline quint8 sex() const                        { return m_sex; } 
  inline void setByeMsg(const QString &msg)        { m_byeMsg = msg.simplified().left(MaxByeMsgLength); }
  inline void setFullName(const QString &fullName) { m_fullName = fullName.simplified().left(MaxNameLength); }
  inline void setHost(const QString &host)         { m_host = host; }
  inline void setNick(const QString &nick)         { m_nick = nick.simplified().left(MaxNickLength); }
  inline void setSex(const quint8 sex = 0)         { sex ? m_sex = 1 : m_sex = 0; }
  inline void setUserAgent(const QString &agent)   { m_userAgent = agent.simplified(); }
  QString toolTip();
  QStringList toList() const;
  static bool isValidNick(const QString &n);
  static QString sexIconString(quint8 sex);
  void toStream(QDataStream &stream) const;
  
private:
  QString m_byeMsg;
  QString m_fullName;
  QString m_host;
  QString m_nick;
  QString m_userAgent;
  quint8 m_sex;
};

#endif /*PROFILE_H_*/
