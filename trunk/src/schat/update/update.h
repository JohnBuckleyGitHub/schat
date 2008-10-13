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

class Download;
class Settings;

static const QString DefaultUpdateXmlUrl = "http://192.168.5.1/schat/updates/update.xml";

/*!
 * \brief Базовый класс для получения обновлений.
 */
class Update : public QObject {
  Q_OBJECT

public:
  enum State {
    GettingUpdateXml,
    GettingUpdates
  };

  Update(QObject *parent = 0);

public slots:
  void execute();

private slots:
  inline void error() { error(404); }
  void saved(const QString &filename);

private:
  bool createQueue(const QString &filename);
  bool verifyFile(const FileInfo &fileInfo);
  inline bool verifyFile() { return verifyFile(currentFile); };
  void downloadNext();
  void error(int err);
  void finished();
  void writeSettings(bool err = false) const;

  Download *m_download;
  FileInfo currentFile;
  QQueue<FileInfo> m_queue;
  QString m_appPath;
  QString m_targetPath;
  QString m_urlPath;
  QStringList m_files;
  QUrl m_url;
  Settings *m_settings;
  State m_state;
  UpdateXmlReader m_reader;
};

#endif /*UPDATE_H_*/
