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

#include <QApplication>
#include <QtCore>

#include "downloadmanager.h"
#include "update.h"
#include "settings.h"

/*!
 * Конструктор класса Update.
 */
Update::Update(QObject *parent)
  : QObject(parent)
{
  m_settings   = settings;
//  m_url        = QUrl(m_settings->getList("Updates/Mirrors").at(0));
  m_appPath    = qApp->applicationDirPath();
  m_targetPath = m_appPath + "/updates";
  m_download   = new DownloadManager(m_targetPath, this);
//  m_urlPath    = QFileInfo(m_url.toString()).path();
  m_state      = Unknown;

  QStringList mirrors = m_settings->getList("Updates/Mirrors");
  if (!mirrors.isEmpty())
    foreach (QString mirror, mirrors)
      m_mirrors.enqueue(QUrl(mirror));

  connect(m_download, SIGNAL(finished()), SLOT(downloadFinished()));
  connect(m_download, SIGNAL(error()), SLOT(downloadError()));
}


/*!
 * Запуск процедуры проверки обновлений.
 */
void Update::execute()
{
  if (m_mirrors.isEmpty()) {
    emit error();
    return;
  }

  m_version.clear();
  m_state = GettingUpdateXml;
  m_xmlUrl = m_mirrors.dequeue();
  m_download->append(m_xmlUrl);
}


/*!
 * Обработка ошибки при скачивании файла.
 *
 * Если состояние равно \a GettingUpdateXml и если очередь \a m_mirrors
 * не пуста то скачиваем xml файл со следующего зеркала.
 */
void Update::downloadError()
{
  if (m_state == GettingUpdateXml) {
    if (!m_mirrors.isEmpty()) {
      m_xmlUrl = m_mirrors.dequeue();
      m_download->append(m_xmlUrl);
    }
    else
      emit error();
  }
}


/*!
 * Уведомление об успешном скачивании файла.
 */
void Update::downloadFinished()
{
  qDebug() << "Update::downloadFinished()";

  if (m_state == GettingUpdateXml) {
    if (m_reader.readFile(m_targetPath + "/" + DownloadManager::saveFileName(m_xmlUrl))) {
      qDebug() << "read ok";

      if (m_reader.isValid()) {
        checkVersion();
//      QList<VersionInfo> versions = m_reader.version();
//        foreach (VersionInfo ver, versions)
//          qDebug() << ver.level << ver.type << ver.version;
//
//        QList<FileInfo> files = m_reader.files();
//        foreach (FileInfo file, files)
//          qDebug() << file.size << file.level << file.type << file.name << file.md5;
      }
    }
    else {
      emit error();
      return;
    }
  }

//  writeSettings();

//  qApp->exit(0);
}


/** [private slots]
 * Слот вызывается при успешном сохранении файла.
 * Инициатор: `Download::saveToDisk(const QString &, QIODevice *)`
 */
void Update::saved(const QString &filename)
{
  if (m_state == GettingUpdateXml) {

    if (!createQueue(filename))
      return;

    m_state = GettingUpdates;
    downloadNext();
  }
  else if (m_state == GettingUpdates) {
    if (verifyFile())
      downloadNext();
    else
      error(405);
  }
}


/** [private]
 *
 */
bool Update::createQueue(const QString &filename)
{
//  if (!m_reader.readFile(filename)) {
//    error(401);
//    return false;
//  }
//
//  if (!m_reader.isUpdateAvailable()) {
//    error(400);
//    return false;
//  }
//
//  QList<FileInfo> list = m_reader.list();
//
//  foreach (FileInfo fileInfo, list) {
//    m_files << fileInfo.name;
//    if (!verifyFile(fileInfo))
//      m_queue.enqueue(fileInfo);
//  }

  return true;
}


/*!
 * Проверка локального файла.
 * Наличие, размер, контрольная сумма.
 * \todo Эта функция для проверки md5 суммы загружает файл целиком в память, это не оптимально.
 *
 * \param fileInfo Структура содержащая информацию о файле.
 * \return \a true в случае успешной проверки файла, иначе \a false;
 */
bool Update::verifyFile(const FileInfo &fileInfo) const
{
  QString fileName = m_targetPath + '/' + fileInfo.name;
  QFile file(fileName);

  if (!file.exists())
    return false;

  if (file.size() != fileInfo.size)
    return false;

  QCryptographicHash hash(QCryptographicHash::Md5);
  QByteArray result;

  if(!file.open(QIODevice::ReadOnly))
    return false;

  hash.addData(file.readAll());
  result = hash.result();

  if (result.toHex() != fileInfo.md5)
    return false;

  return true;
}


/*!
 * Формирование списка файлов.
 */
void Update::checkFiles()
{
  qDebug() << "Update::checkFiles()";

  QMultiMap<int, FileInfo> map = m_reader.files();
  QStringList files;
  qint64 size = 0;

  foreach (VersionInfo ver, m_version) {
    QList<FileInfo> info = map.values(ver.level);
    if (!info.isEmpty()) {
      foreach (FileInfo fileInfo, info)
        if (fileInfo.type == ver.type) {
          m_files << fileInfo;
          files << fileInfo.name;
        }
    }
  }

  if (m_files.isEmpty()) {
    emit error();
    return;
  }

  m_settings->setList("Updates/Files", files);


  foreach (FileInfo file, m_files)
    qDebug() << file.size << file.level << file.type << file.name << file.md5;

  qDebug() << "size:" << size;
}


/*!
 * Анализ списка версий полученных из xml файла.
 * Функция заполняет список \a m_version версиями для обновления.
 * Если список окажется пустым, то происходит уведомление об отсутствии новых версий,
 * иначе происходит разбор файлов для обновления.
 */
void Update::checkVersion()
{
  int levelQt   = m_settings->getInt("Updates/QtLevel");
  int levelCore = m_settings->getInt("Updates/LevelCore");
  QList<VersionInfo> versions = m_reader.version();

  foreach (VersionInfo ver, versions) {
    if (ver.type == "core") {
      if (ver.level > levelCore) {
        m_version << ver;
        m_settings->setString("Updates/LastVersion", ver.version);
      }
    }
    else if (ver.type == "qt") {
      if (ver.level > levelQt)
        m_version << ver;
    }
  }

  if (m_version.isEmpty())
    emit updateAvailable(false);
  else
    checkFiles();
}


/** [private]
 *
 */
void Update::downloadNext()
{
  if (!m_queue.isEmpty()) {
    currentFile = m_queue.dequeue();
    m_download->append(QUrl(m_urlPath + "/win32/" + currentFile.name));
  }
  else
    finished();
}


/** [private]
 *
 */
void Update::error(int err)
{
  writeSettings(true);
//  qApp->exit(err);
}


/** [private]
 *
 */
void Update::writeSettings(bool err) const
{
//  QSettings s(m_appPath + "/schat.conf", QSettings::IniFormat);
//  s.beginGroup("Updates");
//
//  if (!err) {
//    s.setValue("ReadyToInstall", true);
//    s.setValue("Files", m_files);
//  }
//  else {
//    s.setValue("ReadyToInstall", false);
//    return;
//  }

//  s.setValue("LastDownloadedQtLevel", m_reader.qtLevel());
//  s.setValue("LastDownloadedQtVersion", m_reader.qt());
//  s.setValue("LastDownloadedCoreLevel", m_reader.coreLevel());
//  s.setValue("LastDownloadedCoreVersion", m_reader.core());
}
