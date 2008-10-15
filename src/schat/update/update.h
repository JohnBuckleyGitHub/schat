/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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

#ifndef UPDATE_H_
#define UPDATE_H_

#include <QObject>
#include <QQueue>
#include <QUrl>

#include "updatexmlreader.h"

class DownloadManager;
class Settings;

static const QString DefaultUpdateXmlUrl = "http://192.168.5.1/schat/updates/update.xml";

/*!
 * \brief Базовый класс для получения обновлений.
 */
class Update : public QObject {
  Q_OBJECT

public:
  enum State {
    Unknown,
    GettingUpdateXml,
    GettingUpdates
  };

  Update(QObject *parent = 0);

signals:
  void error();
  void finished();
  void updateAvailable(bool available);

public slots:
  void execute();

private slots:
  void downloadError();
  void downloadFinished();
  void saved(const QString &filename);

private:
  bool createQueue(const QString &filename);
  bool verifyFile(const FileInfo &fileInfo) const;
  inline bool verifyFile() { return verifyFile(currentFile); };
  void checkFiles();
  void checkVersion();
  void downloadNext();
  void error(int err);
  void writeSettings(bool err = false) const;

  DownloadManager *m_download;
  FileInfo currentFile;
  QList<FileInfo> m_files;
  QList<VersionInfo> m_version;
  QQueue<FileInfo> m_queue;
  QQueue<QUrl> m_mirrors;
  QString m_appPath;
  QString m_targetPath;
  QString m_urlPath;
  QUrl m_xmlUrl;
  Settings *m_settings;
  State m_state;
  UpdateXmlReader m_reader;
};

#endif /*UPDATE_H_*/
