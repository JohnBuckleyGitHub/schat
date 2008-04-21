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
  Profile(const QString &nick, const QString &fullName, quint8 sex, QObject *parent = 0);
  Profile(QObject *parent = 0);
  Profile(quint8 sex, const QStringList &list, QObject *parent = 0);
  
  bool fromList(const QStringList &list);
  bool isValidUserAgent() const;
  inline bool isValidNick() const                  { return isValidNick(Nick); }
  inline QString fullName() const                  { return FullName; } 
  inline QString host()                            { return Host; }
  inline QString nick() const                      { return Nick; } 
  inline QString userAgent() const                 { return UserAgent; }
  inline quint8 sex() const                        { return Sex; } 
  inline void setFullName(const QString &fullName) { FullName = fullName.simplified(); }
  inline void setHost(const QString &host)         { Host = host; }
  inline void setNick(const QString &nick)         { Nick = nick.simplified(); }
  inline void setSex(const quint8 sex = 0)         { sex ? Sex = 1 : Sex = 0; }
  inline void setUserAgent(const QString &agent)   { UserAgent = agent.simplified(); }
  QString toolTip();
  QStringList toList() const;
  static bool isValidNick(const QString &n);
  static QString sexIconString(quint8 sex);
  void toStream(QDataStream &stream) const;
  
private:
  QString FullName;
  QString Host;
  QString Nick;
  QString UserAgent;
  quint8 Sex;
};

#endif /*PROFILE_H_*/
