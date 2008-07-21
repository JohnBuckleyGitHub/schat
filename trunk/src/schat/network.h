/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <QString>
#include <QStringList>

#include "networkreader.h"

// TODO реализовать возможность константного использования и специальную структуры для пары адрес/порт.

static const QString FailBackServer = "192.168.5.130"; // TODO в публичных релизах заменить на `localhost`.
static const quint16 FailBackPort   = 7666;

class Network {
  
public:
  Network();
  inline bool isNetwork() const      { return m_network; }
  inline int count() const           { return m_servers.count(); }
  inline QString description() const { return m_description; }
  inline QString name() const        { return m_name; }
  inline QString site() const        { return m_site; }
  QString config() const;
  quint16 port() const;
  ServerInfo server() const;
  static ServerInfo failBack();
  static ServerInfo serverInfo(const QString &s);
  void fromConfig(const QString &s);
  void fromFile(const QString &file);
  void fromString(const QString &s);
  
private:
  bool m_network;
  bool m_valid;
  QList<ServerInfo> m_servers;
  QString m_description;
  QString m_file;
  QString m_name;
  QString m_networksPath;
  QString m_site;
};

#endif /*NETWORK_H_*/
