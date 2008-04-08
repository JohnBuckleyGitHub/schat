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
  
  bool fromList(const QStringList &list);
  bool isValidNick() const;
  bool isValidUserAgent() const;
  QString fullName() const                  { return FullName; } 
  QString host()                            { return Host; }
  QString nick() const                      { return Nick; } 
  QString toolTip();
  QString userAgent() const                 { return UserAgent; }
  QStringList toList() const;
  quint8 sex() const                        { return Sex; } 
  static QString sexIconString(quint8 sex);
  void setFullName(const QString &fullName) { FullName = fullName.simplified(); }
  void setHost(const QString &host)         { Host = host; }
  void setNick(const QString &nick)         { Nick = nick.simplified(); }
  void setSex(const quint8 sex = 0)         { sex ? Sex = 1 : Sex = 0; }
  void setUserAgent(const QString &agent)   { UserAgent = agent.simplified(); }
  void toStream(QDataStream &stream) const;
  
private:
  QString FullName;
  QString Host;
  QString Nick;
  QString UserAgent;
  quint8 Sex;
};

#endif /*PROFILE_H_*/
