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
#include "settings.h"
#include "update.h"
#include "verifythread.h"

/*!
 * Конструктор класса Update.
 */
Update::Update(QObject *parent)
  : QObject(parent)
{
  m_settings   = settings;
  m_appPath    = qApp->applicationDirPath();
  m_targetPath = m_appPath + "/updates";
  m_download   = new DownloadManager(m_targetPath, this);
  m_state      = Unknown;

  QStringList mirrors = m_settings->getList("Updates/Mirrors");
  if (!mirrors.isEmpty())
    foreach (QString mirror, mirrors)
      m_mirrors.enqueue(QUrl(mirror));

  m_downloadAll = m_settings->getBool("Updates/AutoDownload");

  connect(m_download, SIGNAL(finished()), SLOT(downloadFinished()));
  connect(m_download, SIGNAL(error()), SLOT(execute()));
}


/*!
 * Запуск процедуры проверки обновлений.
 *
 * В случае если список \a m_mirrors пуст, то это критическая ситуация, сообщаем об ошибке.
 *
 * Иначе пытаемся скачать xml файл с информацией об новых версиях.
 * Очищаем списки \a m_version и \a m_files, устанавливаем состояние \a GettingUpdateXml,
 * получаем адрес файла из очереди \a m_mirrors и добавляем этот адрес в менеджер закачки.
 */
void Update::execute()
{
  if (m_mirrors.isEmpty()) {
    m_settings->notify(Settings::UpdateError);
    return;
  }

  m_version.clear();
  m_files.clear();
  m_state  = GettingUpdateXml;
  m_xmlUrl = m_mirrors.dequeue();
  m_download->append(m_xmlUrl);
}


void Update::checkLocalFilesDone(const QStringList &urls, qint64 size)
{
  qDebug() << "Update::checkLocalFilesDone()";

  m_settings->setInt("Updates/DownloadSize", size);

  if (m_state == GettingUpdateXml) {
    if (!urls.isEmpty()) {
      if (m_downloadAll) {
        m_state = GettingUpdates;
        m_download->append(urls);
      }
      else
        m_settings->notify(Settings::UpdateAvailable);
    }
    else
      m_settings->notify(Settings::UpdateReady);
  }
  else {
    if (urls.isEmpty())
      m_settings->notify(Settings::UpdateReady);
    else
      execute();
  }
}


/*!
 * Уведомление об успешном скачивании очереди файлов.
 *
 * Если состояние равно \a GettingUpdateXml, то пытаемся прочитать скачанный xml файл.
 * В случае успеха вызываем функцию checkVersion(), иначе пытаемся скачать следующий в очереди xml файл.
 *
 * Если состояние равно \a GettingUpdates, то запускаем проверку скачаных файлов checkLocalFiles(),
 * если результатом работы функции будет пустой список, то скачивание прошло успешно, уведомляем о
 * готовности к установке, иначе пытаемся скачать следующий в очереди xml файл.
 */
void Update::downloadFinished()
{
  qDebug() << "Update::downloadFinished()";

  if (m_state == GettingUpdateXml) {
    if (m_reader.readFile(m_targetPath + "/" + DownloadManager::saveFileName(m_xmlUrl))) {
      qDebug() << "read ok";

      if (m_reader.isValid())
        checkVersion();
      else
        execute();
    }
    else
      execute();
  }
  else if (m_state == GettingUpdates) {
    checkLocalFiles();
//    if (urls.isEmpty())
//      m_settings->notify(Settings::UpdateReady);
//    else
//      execute();
  }
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
 * Формирование списка файлов необходимых для установки обновления.
 */
void Update::checkFiles()
{
  qDebug() << "Update::checkFiles()";

  QMultiMap<int, FileInfo> map = m_reader.files();
  QStringList files;
  qint64 size = 0;
  m_files.clear();

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
    m_settings->notify(Settings::UpdateError);
    return;
  }

  m_settings->setList("Updates/Files", files);

  checkLocalFiles();
//  if (!urls.isEmpty()) {
//    if (m_downloadAll) {
//      m_state = GettingUpdates;
//      m_download->append(urls);
//    }
//    else
//      m_settings->notify(Settings::UpdateAvailable);
//  }
//  else
//    m_settings->notify(Settings::UpdateReady);

  foreach (FileInfo file, m_files)
    qDebug() << "check" << file.size << file.level << file.type << file.name << file.md5;

  qDebug() << "size:" << size;
}


void Update::checkLocalFiles()
{
  qDebug() << "Update::checkLocalFiles()";
//
//  QStringList out;
//  qint64 size = 0;
  QString url = QFileInfo(m_xmlUrl.toString()).path() + "/" + m_reader.platform() + "/";
//
//  foreach (FileInfo file, m_files) {
//    if (!verifyFile(file)) {
//      out << (url + file.name);
//      size += file.size;
//    }
//  }
//
//  m_settings->setInt("Updates/DownloadSize", size);

//  return out;
  VerifyThread *thread = new VerifyThread(m_files, m_targetPath, url, this);
  connect(thread, SIGNAL(finished(const QStringList &, qint64)), SLOT(checkLocalFilesDone(const QStringList &, qint64)));
  thread->start();
}


/*!
 * Анализ списка версий полученных из xml файла.
 *
 * Функция заполняет список \a m_version версиями для обновления.
 * Если список окажется пустым, то происходит уведомление об отсутствии новых версий,
 * иначе происходит разбор файлов для обновления.
 *
 * При необходимости в настройки записывается ключ "Updates/LastVersion".
 */
void Update::checkVersion()
{
  int levelQt   = m_settings->getInt("Updates/QtLevel");
  int levelCore = m_settings->getInt("Updates/LevelCore");
  QList<VersionInfo> versions = m_reader.version();
  m_version.clear();

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
    m_settings->notify(Settings::UpdateNoAvailable);
  else
    checkFiles();
}
