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

#include <QCoreApplication>
#include <QDir>
#include <QNetworkReply>
#include <QTemporaryFile>
#include <QTimer>

#include "downloadmanager.h"

/*!
 * Конструктор класса Download.
 */
DownloadManager::DownloadManager(const QString &targetPath, QObject *parent) :
  QObject(parent),
  m_getMirror(false),
  m_targetPath(targetPath),
  m_mirrorXml(0)
{
}


DownloadManager::~DownloadManager()
{
  if (m_mirrorXml)
    m_mirrorXml->remove();
}


/*!
 * Добавление в очередь списка файлов.
 * В случае если очередь оказывается пустой, высылается сигнал \a void finished().
 */
void DownloadManager::append(const QStringList &urlList)
{
  foreach (QString url, urlList)
    append(QUrl(url));

  if (m_downloadQueue.isEmpty())
    QTimer::singleShot(0, this, SIGNAL(finished()));
}


/*!
 * Добавление в очередь одиночного адреса, в случае если очередь пуста
 * вызывается слот startNextDownload() для старта закачки, в противном случае адрес
 * просто добавляется в очередь.
 */
void DownloadManager::append(const QUrl &url)
{
  if (m_downloadQueue.isEmpty())
    QTimer::singleShot(0, this, SLOT(startNextDownload()));

  m_downloadQueue.enqueue(url);
}


QString DownloadManager::mirrorXml() const
{
  if (m_mirrorXml)
    return m_mirrorXml->fileName();

  return "";
}

/*!
 * Статическая функция извлекающая имя файла из Url.
 * Результат может быть пустой строкой, в случае некорректного адреса.
 */
QString DownloadManager::saveFileName(const QUrl &url)
{
  return QFileInfo(url.path()).fileName();
}


/*!
 * Начало закачки следующего файла. Если очередь пуста, значит все закачки завершены.
 */
void DownloadManager::startNextDownload()
{
  if (m_downloadQueue.isEmpty()) {
    emit finished();
    return;
  }

  QUrl url = m_downloadQueue.dequeue();

  // Ошибка если имя файла будет пустым.
  QString fileName = saveFileName(url);
  if (fileName.isEmpty()) {
    emit error();
    return;
  }

  m_getMirror = fileName == "mirror.xml";

  if (!m_getMirror) {
    if (!QDir().exists(m_targetPath)) {
      if (!QDir().mkpath(m_targetPath)) {
        emit error();
        return;
      }
    }

    m_output.setFileName(m_targetPath + "/" + fileName);

    // Ошибка если не удалось открыть файл для записи.
    if (!m_output.open(QIODevice::WriteOnly)) {
      emit error();
      return;
    }
  }
  else {
    m_mirrorXml = new QTemporaryFile(QDir::tempPath() + "/" + fileName, this);
    m_mirrorXml->setAutoRemove(false);

    if (!m_mirrorXml->open()) {
      emit error();
      return;
    }
  }

  QNetworkRequest request(url.toString() + (m_getMirror ? '?' + QString::number(QDateTime::currentDateTime().toTime_t()) : QString()));
  request.setRawHeader("Referer", url.toEncoded());
  request.setRawHeader("User-Agent", QString("Mozilla/5.0 Qt/%1 %2/%3").arg(qVersion()).arg(QCoreApplication::applicationName()).arg(QCoreApplication::applicationVersion()).toLatin1());
  m_current = m_manager.get(request);
  m_current->setReadBufferSize(65536);

  connect(m_current, SIGNAL(finished()), SLOT(downloadFinished()));
  connect(m_current, SIGNAL(readyRead()), SLOT(downloadReadyRead()));
}


/*!
 * Завершение закачки файла.
 */
void DownloadManager::downloadFinished()
{
  if (m_getMirror && m_mirrorXml)
    m_mirrorXml->close();
  else
    m_output.close();

  if (m_current->error()) {
    m_output.remove();
    emit error();
    return;
  }

  m_current->deleteLater();
  startNextDownload();
}


/*!
 * Слот вызывается при поступлении новой порции данных, для записи в файл.
 */
void DownloadManager::downloadReadyRead()
{
  if (m_getMirror && m_mirrorXml)
    m_mirrorXml->write(m_current->readAll());
  else
    m_output.write(m_current->readAll());
}
