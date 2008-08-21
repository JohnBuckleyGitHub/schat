/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <QString>
#include <QStringList>

#include "networkreader.h"

static const QString FailBackServer = "192.168.5.130";
static const quint16 FailBackPort   = 7666;

class Network {
  
public:
  Network();
  Network(const QString &path);
  bool fromConfig(const QString &s);
  bool fromFile(const QString &file);
  bool fromString(const QString &s);
  inline bool isNetwork() const      { return m_network; }
  inline int count() const           { return m_servers.count(); }
  inline QString description() const { return m_description; }
  inline QString key() const         { return m_key; }
  inline QString name() const        { return m_name; }
  inline QString site() const        { return m_site; }
  QString config() const;
  quint16 port() const;
  ServerInfo server() const;
  static ServerInfo failBack();
  static ServerInfo serverInfo(const QString &s);

private:
  bool m_network;
  bool m_valid;
  QList<ServerInfo> m_servers;
  QString m_description;
  QString m_file;
  QString m_key;
  QString m_name;
  QString m_networksPath;
  QString m_site;
};

#endif /*NETWORK_H_*/
