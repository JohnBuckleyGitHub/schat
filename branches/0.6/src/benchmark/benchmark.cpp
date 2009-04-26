/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include "abstractsettings.h"
#include "benchmark.h"
#include "network.h"
#include "abstractprofile.h"
#include "clientservice.h"
#include "client/simpleclient.h"

#include <QtCore>

/*!
 * Конструктор класса Benchmark.
 */
Benchmark::Benchmark(QObject *parent)
  : QObject(parent),
  m_connectInterval(200),
  m_count(0),
  m_usersCount(10),
  m_nickPrefix("test_"),
  m_serverAddr("192.168.5.134:7777")
{
  m_settings = new AbstractSettings(QCoreApplication::applicationDirPath() + "/benchmark.conf", this);
  m_settings->setInt("ConnectInterval", m_connectInterval);
  m_settings->setInt("UsersCount",      m_usersCount);
  m_settings->setString("NickPrefix",   m_nickPrefix);
  m_settings->setString("Network",      m_serverAddr);

  QTimer::singleShot(0, this, SLOT(init()));
}


void Benchmark::accessDenied(quint16 reason)
{
  qDebug() << "Benchmark::accessDenied()" << reason;
}


void Benchmark::connectToHost()
{
  if (m_count < m_usersCount) {
    SimpleClient *client = new SimpleClient(this);
    client->setNick(m_nickPrefix + QString::number(m_count));
    client->link(m_serverAddr);
    m_count++;
    QTimer::singleShot(m_connectInterval, this, SLOT(connectToHost()));
  }
}


void Benchmark::init()
{
  m_settings->read();
  m_connectInterval = m_settings->getInt("ConnectInterval");
  m_usersCount      = m_settings->getInt("UsersCount");
  m_nickPrefix      = m_settings->getString("NickPrefix");
  m_serverAddr      = m_settings->getString("Network");

  connectToHost();
}
