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

#ifndef NETWORKREADER_H_
#define NETWORKREADER_H_

#include <QXmlStreamReader>

struct ServerInfo {
  QString address;
  quint16 port;
};

class NetworkReader : public QXmlStreamReader {

public:
  NetworkReader();
  bool readFile(const QString &fileName);
  inline bool isRandom() const             { return m_random; }
  inline QList<ServerInfo> servers() const { return m_servers; }
  inline QString description() const       { return m_description; }
  inline QString key() const               { return m_key; }
  inline QString networkName() const       { return m_name; }
  inline QString site() const              { return m_site; }

private:
  void readMeta();
  void readNetwork();
  void readServers();
  void readUnknownElement();

  bool m_random;
  QList<ServerInfo> m_servers;
  QString m_description;
  QString m_key;
  QString m_name;
  QString m_site;
};

#endif /*NETWORKREADER_H_*/
