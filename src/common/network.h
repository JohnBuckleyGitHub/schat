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

#ifndef NETWORK_H_
#define NETWORK_H_

#include <QString>
#include <QStringList>

#include "networkreader.h"

/*!
 * \brief Сервисный класс для работы с сетью.
 *
 * \sa NetworkReader
 */
class Network : public QObject {
  Q_OBJECT

public:
  enum {
    MaxName = 64,
    MaxKey  = 64,
    MaxSite = 255,
    MaxDesc = 255
  };

  Network(QObject *parent = 0);
  Network(const QStringList &paths, QObject *parent = 0);
  bool fromConfig(const QString &s);
  bool fromFile(const QString &file);
  bool fromString(const QString &s);
  inline bool isNetwork() const      { return m_network; }
  inline bool single() const         { return m_single; }
  inline int count() const           { return m_servers.count(); }
  inline QString description() const { return m_description; }
  inline QString error() const       { return m_error; }
  inline QString key() const         { return m_key; }
  inline QString name() const        { return m_name; }
  inline QString site() const        { return m_site; }
  inline void setFailBack(bool b)    { m_failBack = b; }
  inline void setPaths(const QStringList &paths) { m_networksPath = paths; }
  inline void setSingle(bool single) { m_single = single; }
  QString config() const;
  quint16 port() const;
  ServerInfo server() const;
  static ServerInfo failBack();
  static ServerInfo serverInfo(const QString &s);

  static const QString failBackServer;
  static const quint16 failBackPort;

private:
  bool m_failBack;
  bool m_network;
  bool m_random;
  bool m_single;
  bool m_valid;
  QList<ServerInfo> m_servers;
  QString m_description;
  QString m_error;
  QString m_file;
  QString m_key;
  QString m_name;
  QString m_site;
  QStringList m_networksPath;
};

#endif /*NETWORK_H_*/
