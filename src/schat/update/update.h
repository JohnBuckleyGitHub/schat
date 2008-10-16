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

/*!
 * \brief Класс предназначен для проверки наличия новых версий и скачивания обновлений.
 *
 * Класс поддерживает скачивание xml файла обновления с нескольких источников, анализ
 * это файла, формирование списка файлов и их скачивание, а также проверку целостности скачанных файлов.
 *
 * Для уведомлений класс использует механизм \a notify объекта Settings.
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
  inline bool downloadAll() const   { return m_downloadAll; }
  inline void downloadAll(bool get) { m_downloadAll = get; }

public slots:
  void execute();

private slots:
  void checkLocalFilesDone(const QStringList &urls, qint64 size);
  void downloadFinished();

private:
  void checkFiles();
  void checkLocalFiles();
  void checkVersion();

  bool m_downloadAll;
  DownloadManager *m_download;
  QList<FileInfo> m_files;
  QList<VersionInfo> m_version;
  QQueue<QUrl> m_mirrors;
  QString m_appPath;
  QString m_targetPath;
  QUrl m_xmlUrl;
  Settings *m_settings;
  State m_state;
  UpdateXmlReader m_reader;
};

#endif /*UPDATE_H_*/
