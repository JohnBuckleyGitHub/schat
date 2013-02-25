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
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

#if QT_VERSION >= 0x050000
# include <QWebPage>
#else
# include <qwebkitversion.h>
#endif

#include "migrate/JSON.h"
#include "migrate/Migrate.h"
#include "settings.h"
#include "version.h"

typedef QLatin1Char   LC;
typedef QLatin1String LS;

UpdateInfo::UpdateInfo(const QUrl &url)
  : revision(0)
  , size(0)
  , url(url)
{
}


UpdateInfo::UpdateInfo(const QVariantMap &data)
  : revision(0)
  , size(0)
{
  if (data.isEmpty())
    return;

  version  = data.value(LS("version")).toString();
  revision = data.value(LS("revision")).toInt();
  url      = data.value(LS("file")).toUrl();
  size     = data.value(LS("size")).toInt();
  hash     = QByteArray::fromHex(data.value(LS("hash")).toByteArray());
  notes    = data.value(LS("notes")).toUrl();
  page     = data.value(LS("page")).toString();

  if (notes.isEmpty())
    notes = LS("http://wiki.schat.me/Simple_Chat_") + version;

  if (page.isEmpty())
    page = url.toString();
}


/*!
 * Проверка корректности данных обновления.
 */
bool UpdateInfo::isValid() const
{
  if (version.isEmpty())
    return false;

  if (revision < 3361)
    return false;

  if (!url.isValid() || size < 1 || hash.size() != 20)
    return false;

  return true;
}


Migrate::Migrate(QObject *parent)
  : QObject(parent)
  , m_prefix(LS("Migrate"))
  , m_state(Idle)
  , m_current(0)
  , m_settings(SimpleSettings)
  , m_status(Unknown)
{
  m_sha1 = new QCryptographicHash(QCryptographicHash::Sha1);
  QTimer::singleShot(0, this, SLOT(start()));
}


Migrate::~Migrate()
{
  delete m_sha1;
}


/*!
 * Запуск проверки обновлений.
 *
 * Проверка обновлений будет невозможна, если не установлена ревизия чата или исполняемый файл был переименован.
 * Ревизия автоматически устанавливается в официальных сборках собранных в http://buildbot.schat.me.
 */
void Migrate::check()
{
  if (m_state != Idle)
    return;

  m_state = DownloadJSON;
  m_rawJSON.clear();

  m_info = UpdateInfo(m_settings->getString(m_prefix + LS("/Url")) + LC('?') + QString::number(QDateTime::currentDateTime().toTime_t()));
  if (!m_info.url.isValid())
    return setDone(CheckError);

  QTimer::singleShot(0, this, SLOT(startDownload()));
}


/*!
 * Запуск загрузки обновления.
 */
void Migrate::download()
{
  m_state = DownloadUpdate;
  m_sha1->reset();

  const QString path = QApplication::applicationDirPath() + LS("/updates");
  QDir().mkpath(path);

  m_file.setFileName(path + LS("/schat2-") + m_info.version + LS(".") + QString::number(m_info.revision) + LS(".exe"));
  if (!m_file.open(QIODevice::WriteOnly))
    return setDone(DownloadError);

  startDownload();
}


void Migrate::finished()
{
  if (!m_current->error()) {
    if (m_state == DownloadJSON)
      readJSON();
    else
      checkUpdate();
  }
  else if (m_state != Paused)
    setDone(m_state == DownloadJSON ? CheckError : DownloadError);

  m_current->deleteLater();
}


void Migrate::readyRead()
{
  if (m_state == DownloadUpdate) {
    QByteArray data = m_current->readAll();
    m_sha1->addData(data);
    m_file.write(data);
  }
  else
    m_rawJSON.append(m_current->readAll());
}


void Migrate::start()
{
  check();
}


/*!
 * Загрузка файла.
 */
void Migrate::startDownload()
{
  QNetworkRequest request(m_info.url);
  request.setRawHeader("Referer", m_info.url.toEncoded());
  request.setRawHeader("User-Agent", QString(LS("Mozilla/5.0 (%1) Qt/%2 AppleWebKit/%3 Simple Chat/%4"))
      .arg(LS("win"))
      .arg(qVersion())
      .arg(qWebKitVersion())
      .arg(QCoreApplication::applicationVersion()).toLatin1());

  qint64 pos = m_file.pos();
  if (pos)
    request.setRawHeader("Range", "bytes=" + QByteArray::number(pos) + "-");

  m_current = m_manager.get(request);
  connect(m_current, SIGNAL(finished()), SLOT(finished()));
  connect(m_current, SIGNAL(readyRead()), SLOT(readyRead()));
  connect(m_current, SIGNAL(downloadProgress(qint64,qint64)), SIGNAL(downloadProgress(qint64,qint64)));
}


/*!
 * Проверка корректности скачанного файла обновлений, методом проверки SHA1 хэша.
 */
void Migrate::checkUpdate()
{
  m_file.close();
  if (m_info.hash == m_sha1->result()) {
    m_settings->setString(m_prefix + LS("/Version"),  m_info.version);
    m_settings->setInt(m_prefix + LS("/Revision"),    m_info.revision);
    setDone(UpdateReady);
  }
  else
    setDone(DownloadError);
}


/*!
 * Чтение и проверка JSON данных с информацией об обновлениях.
 */
void Migrate::readJSON()
{
  QVariantMap data = JSON::parse(m_rawJSON).toMap();
  m_rawJSON.clear();
  if (data.isEmpty())
    return setDone(CheckError);

  QVariantMap json = data.value(m_settings->getString(m_prefix + LS("/Channel"))).toMap();
  if (json.isEmpty())
    return setDone(CheckError);

  m_info = UpdateInfo(json.value(LS("win32")).toMap());
  if (!m_info.isValid())
    return setDone(CheckError);

  if (SCHAT_REVISION >= m_info.revision)
    return setDone(NoUpdates);

  setDone(UpdateAvailable);

  if (m_settings->getBool(m_prefix + LS("/AutoDownload")))
    QTimer::singleShot(0, this, SLOT(download()));
}


/*!
 * Обработка завершения операций.
 *
 * \param status Статус проверки обновлений.
 */
void Migrate::setDone(Status status)
{
  m_status = status;
  m_state = Idle;

  if (m_file.isOpen())
    m_file.close();

  m_settings->setBool(m_prefix + LS("/Ready"), status == UpdateReady);

  emit done(status);
}
