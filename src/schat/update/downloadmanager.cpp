/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright � 2008 IMPOMEZIA <schat@impomezia.com>
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

#include <QtCore>
#include <QtNetwork>

#include "downloadmanager.h"

/*!
 * ����������� ������ Download.
 */
DownloadManager::DownloadManager(const QString &targetPath, QObject *parent) :
  QObject(parent), m_targetPath(targetPath)
{
}


/*!
 * ���������� � ������� ������ ������.
 * � ������ ���� ������� ����������� ������, ���������� ������ \a void finished().
 */
void DownloadManager::append(const QStringList &urlList)
{
  foreach (QString url, urlList)
    append(QUrl(url));

  if (m_downloadQueue.isEmpty())
    QTimer::singleShot(0, this, SIGNAL(finished()));
}


/*!
 * ���������� � ������� ���������� ������, � ������ ���� ������� �����
 * ���������� ���� startNextDownload() ��� ������ �������, � ��������� ������ �����
 * ������ ����������� � �������.
 */
void DownloadManager::append(const QUrl &url)
{
  if (m_downloadQueue.isEmpty())
    QTimer::singleShot(0, this, SLOT(startNextDownload()));

  m_downloadQueue.enqueue(url);
}


/*!
 * ����������� ������� ����������� ��� ����� �� Url.
 * ��������� ����� ���� ������ �������, � ������ ������������� ������.
 */
QString DownloadManager::saveFileName(const QUrl &url)
{
  return QFileInfo(url.path()).fileName();
}


/*!
 * ������ ������� ���������� �����. ���� ������� �����, ������ ��� ������� ���������.
 */
void DownloadManager::startNextDownload()
{
  if (m_downloadQueue.isEmpty()) {
    emit finished();
    return;
  }

  QUrl url = m_downloadQueue.dequeue();

  // ������ ���� ��� ����� ����� ������.
  QString fileName = saveFileName(url);
  if (fileName.isEmpty()) {
    emit error();
    return;
  }

  // ������ ���� �� ������� ������� ���������� ����������.
  if (!QDir().exists(m_targetPath))
    if (!QDir().mkpath(m_targetPath)) {
      emit error();
      return;
    }

  m_output.setFileName(m_targetPath + "/" + fileName);

  // ������ ���� �� ������� ������� ���� ��� ������.
  if (!m_output.open(QIODevice::WriteOnly)) {
    emit error();
    return;
  }

  QNetworkRequest request(url);
  m_current = m_manager.get(request);
  m_current->setReadBufferSize(65536);

  connect(m_current, SIGNAL(finished()), SLOT(downloadFinished()));
  connect(m_current, SIGNAL(readyRead()), SLOT(downloadReadyRead()));
}


/*!
 * ���������� ������� �����.
 */
void DownloadManager::downloadFinished()
{
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
 * ���� ���������� ��� ����������� ����� ������ ������, ��� ������ � ����.
 */
void DownloadManager::downloadReadyRead()
{
  m_output.write(m_current->readAll());
}
