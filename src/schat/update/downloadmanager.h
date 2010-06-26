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

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QFile>
#include <QObject>
#include <QQueue>
#include <QTime>
#include <QUrl>
#include <QNetworkAccessManager>

class QTemporaryFile;

/*!
 * \brief Базовый класс для скачивания файла.
 *
 * Поддерживаются протоколы http и ftp, специальные возможности, например докачка не поддерживаются.
 */
class DownloadManager: public QObject
{
  Q_OBJECT

public:
  DownloadManager(const QString &targetPath, QObject *parent = 0);
  ~DownloadManager();
  inline void clear() { m_downloadQueue.clear(); }
  QString mirrorXml() const;
  static QString saveFileName(const QUrl &url);
  void append(const QStringList &urlList);
  void append(const QUrl &url);

signals:
  void error();
  void finished();

private slots:
  void startNextDownload();
  void downloadFinished();
  void downloadReadyRead();

private:
  bool m_getMirror;
  QFile m_output;
  QNetworkAccessManager m_manager;
  QNetworkReply *m_current;
  QQueue<QUrl> m_downloadQueue;
  QString m_targetPath;
  QTemporaryFile *m_mirrorXml;
};

#endif
