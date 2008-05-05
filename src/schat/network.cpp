/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>

#include "network.h"
#include "networkreader.h"

Network::Network()
{
  m_networksPath = qApp->applicationDirPath() + "/networks/";
  m_index = 0;
}


/** [public]
 * 
 */
QString Network::config() const
{
  if (m_valid) {
    if (m_network)
      return m_file;
    else
      return m_server + ':' + QString().setNum(m_port);
  }
  else
    return "localhost:7666";
}


/** [public]
 * 
 */
QString Network::server()
{
  if (m_valid) {
    if (m_network) {
      if (m_index >= m_servers.size())
        m_index = 0;
      m_server = server(m_servers.at(m_index));
      m_port   = port(m_servers.at(m_index));
      ++m_index;
    }
    return m_server;
  }
  else
    return "localhost";
}


/** [public]
 * 
 */
quint16 Network::port()
{
  if (m_valid)
    return m_port;
  else
    return 7666;
}


/** [public static]
 * 
 */
QString Network::server(const QString &s)
{
  QStringList list = s.split(':');
  if (list.size() == 2) {
    return list.at(0);
  }
  else
    return "";    
}


/** [public static]
 * 
 */
quint16 Network::port(const QString &s)
{
  QStringList list = s.split(':');
  if (list.size() == 2) {
    return quint16(list.at(1).toUInt());
  }
  else
    return 0; 
}


/** [public]
 * 
 */
void Network::fromConfig(const QString &s)
{
  if (QFile::exists(m_networksPath + s))
    fromFile(s);
  else
    fromString(s);  
}


/** [public]
 * 
 */
void Network::fromFile(const QString &file)
{
  NetworkReader reader;
  
  if (reader.readFile(m_networksPath + file)) {
    m_valid       = true;
    m_network     = true;
    m_description = reader.description();
    m_name        = reader.networkName();
    m_site        = reader.site();
    m_servers     = reader.servers();
    m_file        = file;
    m_index       = 0;
  }
  else {
    m_valid   = false;
    m_network = false;
  }
}


/** [public]
 * 
 */
void Network::fromString(const QString &s)
{
  QStringList list = s.split(':');
  if (list.size() == 2) {
    m_valid   = true;
    m_network = false;
    m_server  = list.at(0);
    m_port    = quint16(list.at(1).toUInt());
  }
  else {
    m_valid   = false;
    m_network = false;
  }
}
