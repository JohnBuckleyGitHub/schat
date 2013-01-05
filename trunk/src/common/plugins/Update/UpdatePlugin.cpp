/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include <QApplication>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QLabel>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProgressBar>
#include <QTimer>
#include <QtPlugin>

#if QT_VERSION >= 0x050000
# include <QWebPage>
#else
# include <qwebkitversion.h>
#endif

#include "ChatCore.h"
#include "ChatNotify.h"
#include "ChatSettings.h"
#include "JSON.h"
#include "Path.h"
#include "sglobal.h"
#include "tools/OsInfo.h"
#include "tools/Ver.h"
#include "Translation.h"
#include "ui/BgOperationWidget.h"
#include "UpdatePlugin.h"
#include "UpdatePlugin_p.h"
#include "UpdateSettings.h"
#include "version.h"

UpdatePluginImpl::UpdatePluginImpl(QObject *parent)
  : ChatPlugin(parent)
  , m_settings(ChatCore::settings())
  , m_prefix(LS("Update"))
  , m_state(Idle)
  , m_current(0)
  , m_status(Unknown)
{
  m_settings->setLocalDefault(m_prefix + LS("/Url"),          LS("http://download.schat.me/schat2/update.json"));
  m_settings->setLocalDefault(m_prefix + LS("/Channel"),      LS("stable"));
  m_settings->setLocalDefault(m_prefix + LS("/AutoDownload"), true);
  m_settings->setLocalDefault(m_prefix + LS("/Ready"),        false);
  m_settings->setLocalDefault(m_prefix + LS("/Version"),      QString());
  m_settings->setLocalDefault(m_prefix + LS("/Revision"),     0);

  ChatCore::translation()->addOther(LS("update"));

  m_sha1 = new QCryptographicHash(QCryptographicHash::Sha1);
  QTimer::singleShot(0, this, SLOT(start()));
}


UpdatePluginImpl::~UpdatePluginImpl()
{
  delete m_sha1;
}


bool UpdatePluginImpl::supportDownload()
{
# if defined(Q_OS_WIN)
  return true;
# else
  return false;
# endif
}


/*!
 * Запуск проверки обновлений.
 *
 * Проверка обновлений будет невозможна, если не установлена ревизия чата или исполняемый файл был переименован.
 * Ревизия автоматически устанавливается в официальных сборках собранных в http://buildbot.schat.me.
 */
void UpdatePluginImpl::check()
{
  if (m_state != Idle)
    return;

  m_state = DownloadJSON;
  m_rawJSON.clear();
  m_revision = 0;
  m_size = 0;
  m_hash.clear();
  m_version.clear();

  m_url = QUrl(m_settings->value(m_prefix + LS("/Url")).toString() + LC('?') + QString::number(QDateTime::currentDateTime().toTime_t()));
  if (!m_url.isValid())
    return setDone(CheckError);

  QTimer::singleShot(0, this, SLOT(startDownload()));
}


void UpdatePluginImpl::clicked(const QString &key)
{
  if (m_prefix != key)
    return;

  if (m_status == UpdateReady)
    QTimer::singleShot(0, this, SLOT(restart()));
}


/*!
 * Запуск загрузки обновления.
 */
void UpdatePluginImpl::download()
{
  m_state = DownloadUpdate;
  m_sha1->reset();
  m_file.setFileName(Path::cache() + LS("/schat2-") + m_version + LS(".") + QString::number(m_revision) + LS(".exe"));
  if (!m_file.open(QIODevice::WriteOnly))
    return setDone(DownloadError);

  if (BgOperationWidget::lock(m_prefix, tr("Downloading update"))) {
    BgOperationWidget::progress()->setRange(0, m_size);
    BgOperationWidget::progress()->setVisible(true);
  }

  startDownload();
}


void UpdatePluginImpl::downloadProgress(qint64 bytesReceived)
{
  if (m_state != DownloadUpdate || !BgOperationWidget::lock(m_prefix))
    return;

  BgOperationWidget::progress()->setValue(bytesReceived);
}


void UpdatePluginImpl::finished()
{
  if (!m_current->error()) {
    if (m_state == DownloadJSON)
      readJSON();
    else
      checkUpdate();
  }
  else
    setDone(m_state == DownloadJSON ? CheckError : DownloadError);

  m_current->deleteLater();
}


void UpdatePluginImpl::readyRead()
{
  if (m_state == DownloadUpdate) {
    QByteArray data = m_current->readAll();
    m_sha1->addData(data);
    m_file.write(data);
  }
  else
    m_rawJSON.append(m_current->readAll());
}


void UpdatePluginImpl::restart()
{
  ChatNotify::start(Notify::Restart);
}


void UpdatePluginImpl::start()
{
  if (SCHAT_REVISION)
    QFile::remove(Path::cache() + LS("/schat2-") + QApplication::applicationVersion() + LS(".") + QString::number(SCHAT_REVISION) + LS(".exe"));

  connect(BgOperationWidget::i(), SIGNAL(clicked(QString)), SLOT(clicked(QString)));
  check();
}


/*!
 * Загрузка файла.
 */
void UpdatePluginImpl::startDownload()
{
  QNetworkRequest request(m_url);
  request.setRawHeader("Referer", m_url.toEncoded());
  request.setRawHeader("User-Agent", QString(LS("Mozilla/5.0 (%1) Qt/%2 AppleWebKit/%3 Simple Chat/%4"))
      .arg(OsInfo::json().value(LS("os")).toString())
      .arg(qVersion())
      .arg(qWebKitVersion())
      .arg(QCoreApplication::applicationVersion()).toLatin1());

  m_current = m_manager.get(request);
  connect(m_current, SIGNAL(finished()), SLOT(finished()));
  connect(m_current, SIGNAL(readyRead()), SLOT(readyRead()));
  connect(m_current, SIGNAL(downloadProgress(qint64,qint64)), SLOT(downloadProgress(qint64)));
}


/*!
 * Проверка корректности скачанного файла обновлений, методом проверки SHA1 хэша.
 */
void UpdatePluginImpl::checkUpdate()
{
  m_file.close();
  if (m_hash == m_sha1->result()) {
    m_settings->setValue(m_prefix + LS("/Version"),  m_version);
    m_settings->setValue(m_prefix + LS("/Revision"), m_revision);
    setDone(UpdateReady);
  }
  else
    setDone(DownloadError);
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

  QVariantMap json = data.value(m_settings->value(m_prefix + LS("/Channel")).toString()).toMap();
  if (json.isEmpty())
    return setDone(CheckError);

  QVariantMap os = json.value(LS("win32")).toMap();
  if (os.isEmpty())
    return setDone(CheckError);

  m_version = os.value(LS("version")).toString();
  if (m_version.isEmpty() || Ver(m_version) < LS("1.99.25"))
    return setDone(CheckError);

  m_revision = os.value(LS("revision")).toInt();
  if (m_revision < 1)
    return setDone(CheckError);

  if (SCHAT_REVISION >= m_revision)
    return setDone(NoUpdates);

  m_url = os.value(LS("file")).toUrl();
  if (!m_url.isValid())
    return setDone(CheckError);

  m_size = os.value(LS("size")).toInt();
  if (m_size < 1)
    return setDone(CheckError);

  m_hash = QByteArray::fromHex(os.value(LS("hash")).toByteArray());
  if (m_hash.size() != 20)
    return setDone(CheckError);

  setDone(UpdateAvailable);

  if (supportDownload() && m_settings->value(m_prefix + LS("/AutoDownload")).toBool() == true)
    QTimer::singleShot(0, this, SLOT(download()));
}


/*!
 * Обработка завершения операций.
 *
 * \param status Статус проверки обновлений.
 */
void UpdatePluginImpl::setDone(Status status)
{
  m_status = status;
  m_state = Idle;

  if (m_file.isOpen())
    m_file.close();

  m_settings->setValue(m_prefix + LS("/Ready"), status == UpdateReady);

  emit done(status);

  if (!BgOperationWidget::lock(m_prefix))
    return;

  BgOperationWidget::progress()->setVisible(false);

  if (supportDownload()) {
    if (status == UpdateReady) {
      BgOperationWidget::setText(QString(LS("<a href='#' style='text-decoration:none; color:#216ea7;'>%1</a>")).arg(tr("Install Update Now")));
      return;
    }

    if (status == DownloadError)
      BgOperationWidget::setText(tr("Update Error"));
  }

  if (status == UpdateAvailable)
    BgOperationWidget::setText(QString(LS("<a href='#' style='text-decoration:none; color:#216ea7;'>%1</a>")).arg(tr("Update Available")));

  BgOperationWidget::unlock(m_prefix, false);
}


bool UpdatePlugin::check() const
{
  if (!SCHAT_REVISION)
    return false;

  if (UpdatePluginImpl::supportDownload() && Path::app() != LS("schat2"))
    return false;

  return true;
}


ChatPlugin *UpdatePlugin::create()
{
  m_plugin = new UpdatePluginImpl(this);
  return m_plugin;
}


QWidget *UpdatePlugin::settings(QWidget *parent)
{
  if (UpdatePluginImpl::supportDownload())
    return new UpdateSettings(parent);

  return 0;
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(Update, UpdatePlugin);
#endif
