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

#include <QApplication>
#include <QFileInfo>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QtPlugin>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "FileLocations.h"
#include "JSON.h"
#include "sglobal.h"
#include "tools/Ver.h"
#include "UpdatePlugin.h"
#include "UpdatePlugin_p.h"
#include "version.h"

UpdatePluginImpl::UpdatePluginImpl(QObject *parent)
  : ChatPlugin(parent)
  , m_state(Idle)
  , m_current(0)
  , m_prefix(LS("Update"))
  , m_status(Unknown)
{
  ChatCore::settings()->setLocalDefault(m_prefix + LS("/Url"),          LS("http://buildbot.local/update.json"));
  ChatCore::settings()->setLocalDefault(m_prefix + LS("/Channel"),      LS("devel"));
  ChatCore::settings()->setLocalDefault(m_prefix + LS("/AutoDownload"), true);

  QTimer::singleShot(0, this, SLOT(check()));
}


/*!
 * Запуск проверки обновлений.
 *
 * Проверка обновлений будет невозможно, если не установлена ревизия чата или исполняемый файл был переименован.
 * Ревизия автоматически устанавливается в официальных сборках собранных в http://buildbot.schat.me.
 */
void UpdatePluginImpl::check()
{
  if (m_state != Idle)
    return;

  if (!SCHAT_REVISION)
    return setDone(Unsupported);

  if (ChatCore::locations()->path(FileLocations::BaseName) != LS("schat2"))
    return setDone(Unsupported);

  m_state = DownloadJSON;
  m_rawJSON.clear();
  m_revision = 0;
  m_size = 0;
  m_hash.clear();
  m_version.clear();

  m_url = ChatCore::settings()->value(m_prefix + LS("/Url")).toUrl();
  if (!m_url.isValid())
    return setDone(CheckError);;

  QTimer::singleShot(0, this, SLOT(startDownload()));
}


void UpdatePluginImpl::download()
{
  m_state = DownloadUpdate;

  QString fileName = QFileInfo(m_url.path()).fileName();
  if (fileName.isEmpty())
    fileName = LS("update.exe");

  m_file.setFileName(QApplication::applicationDirPath() + LS("/.schat2/") + fileName);
  if (!m_file.open(QIODevice::WriteOnly))
    return setDone(DownloadError);

  startDownload();
}


void UpdatePluginImpl::finished()
{
  qDebug() << "";
  qDebug() << "--- [Update] finished()" << m_rawJSON << m_current->error() << m_current->errorString();
  qDebug() << "";

  if (!m_current->error()) {
    if (m_state == DownloadJSON)
      readJSON();
    else
      setDone(DownloadError);
  }
  else
    setDone(m_state == DownloadJSON ? CheckError : DownloadError);

  m_current->deleteLater();
}


void UpdatePluginImpl::readyRead()
{
  qDebug() << "";
  qDebug() << "--- [Update] readyRead()" << m_current->bytesAvailable();
  qDebug() << "";

  if (m_state == DownloadJSON)
    m_rawJSON.append(m_current->readAll());
  else
    m_file.write(m_current->readAll());
}


void UpdatePluginImpl::startDownload()
{
  QNetworkRequest request(m_url);
  m_current = m_manager.get(request);
  connect(m_current, SIGNAL(finished()), SLOT(finished()));
  connect(m_current, SIGNAL(readyRead()), SLOT(readyRead()));
}


/*!
 * Чтение и проверка JSON данных с информацией об обновлениях.
 */
void UpdatePluginImpl::readJSON()
{
  QVariantMap data = JSON::parse(m_rawJSON).toMap();
  m_rawJSON.clear();
  if (data.isEmpty())
    return setDone(CheckError);

  QVariantMap json = data.value(ChatCore::settings()->value(m_prefix + LS("/Channel")).toString()).toMap();
  if (json.isEmpty())
    return setDone(CheckError);

  m_version = json.value(LS("version")).toString();
  if (m_version.isEmpty() || Ver(m_version) < LS("1.99.25"))
    return setDone(CheckError);

  m_revision = json.value(LS("rev")).toInt();
  if (m_revision < 1)
    return setDone(CheckError);

  if (SCHAT_REVISION >= m_revision)
    return setDone(NoUpdates);

  QVariantMap file = json.value(LS("win32")).toMap();
  if (file.isEmpty())
    return setDone(CheckError);

  m_url = file.value(LS("file")).toUrl();
  if (!m_url.isValid())
    return setDone(CheckError);

  m_size = file.value(LS("size")).toInt();
  if (m_size < 1)
    return setDone(CheckError);

  m_hash = QByteArray::fromHex(file.value(LS("hash")).toByteArray());
  if (m_hash.size() != 20)
    return setDone(CheckError);

  setDone(UpdateAvailable);

  if (ChatCore::settings()->value(m_prefix + LS("/AutoDownload")) == true)
    QTimer::singleShot(0, this, SLOT(download()));
}


void UpdatePluginImpl::setDone(Status status)
{
  qDebug() << "== setDone()" << status;
  m_status = status;
  m_state = Idle;

  if (m_file.isOpen())
    m_file.close();

  emit done(status);
}


ChatPlugin *UpdatePlugin::create()
{
  m_plugin = new UpdatePluginImpl(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(Cache, UpdatePlugin);
