/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#include "client/AbstractClient.h"
#include "client/NetworkPool.h"
#include "jdns/qjdns.h"

NetworkPool::NetworkPool(AbstractClient *client)
  : QObject(client)
  , m_client(client)
  , m_last(-1)
  , m_current(-1)
  , m_jdns(0)
{
}


bool NetworkPool::open(const QUrl &url)
{
  if (url.port() != -1)
    return false;

  if (!QHostAddress(url.host()).isNull())
    return false;

  if (m_url != url) {
    m_current = -1;
    m_urls.clear();
  }

  m_last = -1;
  m_url = url;
  dns();

  if (!m_jdns->init(QJDns::Unicast, QHostAddress::Any))
    return false;

  QList<QJDns::NameServer> addrs = QJDns::systemInfo().nameServers;
  if (addrs.isEmpty())
    addrs.append(QJDns::NameServer(QHostAddress("208.67.222.222")));

  m_jdns->setNameServers(addrs);
  m_jdns->queryStart("_schat._tcp." + url.host().toLatin1(), QJDns::Srv);

  return true;
}


QUrl NetworkPool::last() const
{
  if (m_last == -1 || m_last >= m_urls.size())
    return next();

  return m_urls.at(m_last);
}


QUrl NetworkPool::next() const
{
  if (m_urls.isEmpty())
    return m_url;

  if (m_urls.size() == 1)
    return m_urls.at(0);

  m_current++;
  if (m_current == m_urls.size())
    m_current = 0;

  return m_urls.at(m_current);
}


/*!
 * Получение адреса случайного сервера.
 */
QUrl NetworkPool::random() const
{
  if (m_urls.isEmpty())
    return m_url;

  if (m_urls.size() == 1)
    return m_urls.at(0);

  int rand = qrand() % m_urls.size();
  if (m_current == rand) {
    rand++;
    if (rand == m_urls.size())
      rand = 0;
  }

  m_current = rand;
  return m_urls.at(m_current);
}


void NetworkPool::reset()
{
  m_url.clear();
  m_urls.clear();
  m_current = -1;
  m_last = -1;
}


void NetworkPool::error(int id, int e)
{
  Q_UNUSED(id)
  Q_UNUSED(e)
  m_jdns->shutdown();

  QUrl url = m_url;
  url.setPort(Protocol::DefaultPort);
  m_client->openUrl(url, m_client->cookie(), AbstractClient::NoOptions);
}


void NetworkPool::ready(int id, const QJDnsResponse &results)
{
  Q_UNUSED(id)

  for (int i = 0; i < results.answerRecords.count(); ++i) {
    QJDnsRecord r = results.answerRecords.at(i);

    if (r.type == QJDns::Srv) {
      QUrl url = m_url;
      url.setHost(r.name.endsWith('.') ? r.name.left(r.name.size() - 1) : r.name);
      url.setPort(r.port);
      qDebug() << url;
      if (!m_urls.contains(url))
        m_urls.append(url);
    }
  }

  m_jdns->shutdown();
  m_client->openUrl(random(), m_client->cookie(), AbstractClient::NoOptions);
}


void NetworkPool::shutdownFinished()
{
  qDebug() << "shutdownFinished()";

  m_jdns->deleteLater();
  m_jdns = 0;
}


QJDns *NetworkPool::dns()
{
  if (!m_jdns) {
    m_jdns = new QJDns(this);
    connect(m_jdns, SIGNAL(resultsReady(int, const QJDnsResponse &)), SLOT(ready(int, const QJDnsResponse &)));
    connect(m_jdns, SIGNAL(error(int, int)), SLOT(error(int, int)));
    connect(m_jdns, SIGNAL(shutdownFinished()), SLOT(shutdownFinished()));
  }

  return m_jdns;
}
