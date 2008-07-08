/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <QString>
#include <QStringList>

static const QString FailBackServer = "192.168.5.130"; // TODO в публичных релизах заменить на `localhost`.
static const quint16 FailBackPort   = 7666;

class Network {
  
public:
  Network();
  inline bool isNetwork() const      { return m_network; }
  inline QString description() const { return m_description; }
  inline QString name() const        { return m_name; }
  inline QString site() const        { return m_site; }
  QString config() const;
  QString server();
  quint16 port();
  static QString server(const QString &s);
  static quint16 port(const QString &s);
  void fromConfig(const QString &s);
  void fromFile(const QString &file);
  void fromString(const QString &s);
  
private:
  bool m_network;
  bool m_valid;
  int m_index;
  QString m_description;
  QString m_file;
  QString m_name;
  QString m_networksPath; 
  QString m_site;
  QString m_server;
  QStringList m_servers;
  quint16 m_port;
};

#endif /*NETWORK_H_*/
