/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include <QDebug>

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QtPlugin>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "sglobal.h"
#include "UpdatePlugin.h"
#include "UpdatePlugin_p.h"
#include "version.h"

UpdatePluginImpl::UpdatePluginImpl(QObject *parent)
  : ChatPlugin(parent)
  , m_state(Idle)
  , m_current(0)
  , m_prefix(LS("Update"))
{
  ChatCore::settings()->setLocalDefault(m_prefix + LS("/Url"), LS("http://buildbot.local/update.json"));

  QTimer::singleShot(0, this, SLOT(check()));
}


void UpdatePluginImpl::check()
{
  if (m_state != Idle)
    return;

  if (!SCHAT_REVISION)
    return;

  m_state = DownloadJSON;
  m_rawJSON.clear();
  m_url = ChatCore::settings()->value(m_prefix + LS("/Url")).toUrl();
  if (!m_url.isValid())
    return;

  QTimer::singleShot(0, this, SLOT(startDownload()));
}


void UpdatePluginImpl::finished()
{
  qDebug() << "";
  qDebug() << "--- [Update] finished()" << m_rawJSON;
  qDebug() << "";

  m_current->deleteLater();
  m_state = Idle;
}


void UpdatePluginImpl::readyRead()
{
  qDebug() << "";
  qDebug() << "--- [Update] readyRead()" << SCHAT_REVISION;
  qDebug() << "";

  if (m_state == DownloadJSON)
    m_rawJSON.append(m_current->readAll());
}


void UpdatePluginImpl::startDownload()
{
  QNetworkRequest request(m_url);
  m_current = m_manager.get(request);
  connect(m_current, SIGNAL(finished()), SLOT(finished()));
  connect(m_current, SIGNAL(readyRead()), SLOT(readyRead()));
}


ChatPlugin *UpdatePlugin::create()
{
  m_plugin = new UpdatePluginImpl(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(Cache, UpdatePlugin);
