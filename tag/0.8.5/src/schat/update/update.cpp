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

#include <QApplication>
#include <QtCore>

#include "downloadmanager.h"
#include "settings.h"
#include "update.h"

#ifndef SCHAT_NO_UPDATE
  #include "verifythread.h"
#endif

/*!
 * Конструктор класса Update.
 */
Update::Update(QObject *parent)
  : QObject(parent)
{
  m_settings   = SimpleSettings;
  m_targetPath = QApplication::applicationDirPath() + "/updates";
  m_download   = new DownloadManager(m_targetPath, this);
  m_state      = Unknown;

  QStringList mirrors = m_settings->getList("Updates/Mirrors");
  if (!mirrors.isEmpty())
    foreach (QString mirror, mirrors)
      m_mirrors.enqueue(QUrl(mirror));

  connect(m_download, SIGNAL(finished()), SLOT(downloadFinished()));
  connect(m_download, SIGNAL(error()), SLOT(execute()));

  #ifndef SCHAT_NO_UPDATE
    m_downloadAll = m_settings->getBool("Updates/AutoDownload");
  #endif
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
  #ifndef SCHAT_NO_UPDATE
    m_files.clear();
  #endif
  m_state  = GettingUpdateXml;
  m_xmlUrl = m_mirrors.dequeue();
  m_download->clear();
  m_download->append(m_xmlUrl);
}


/*!
 * Уведомление об успешном скачивании очереди файлов.
 *
 * Если состояние равно \a GettingUpdateXml, то пытаемся прочитать скачанный xml файл.
 * В случае успеха вызываем функцию checkVersion(), иначе пытаемся скачать следующий в очереди xml файл.
 *
 * Если состояние равно \a GettingUpdates, то запускаем проверку скачаных файлов checkLocalFiles().
 */
void Update::downloadFinished()
{
  if (m_state == GettingUpdateXml) {
    if (m_reader.readFile(m_download->mirrorXml())) {

      if (m_reader.isValid())
        checkVersion();
      else
        execute();
    }
    else
      execute();
  }
  #ifndef SCHAT_NO_UPDATE
    else if (m_state == GettingUpdates)
      checkLocalFiles();
  #endif
}


/*!
 * Обработка завершения проверки локальных файлов.
 *
 * Если состояние равно \a GettingUpdateXml и список файлов которые необходимо скачать
 * не пуст, то в зависимости от члена \a m_downloadAll либо скачиваем файлы, либо уведомляем
 * о наличии новой версии.
 *
 * Если состояние равно \a GettingUpdates, то при пустом списке уведомляем о готовности к установке
 * обновлений, иначе пытаемся скачать следующий xml файл.
 *
 * \param urls Список адресов файлов, которые необходимо скачать для установки обновления (пустой список означает, что все файлы скачаны).
 * \param size Размер обновлений в байтах.
 */
#ifndef SCHAT_NO_UPDATE
void Update::checkLocalFilesDone(const QStringList &urls, qint64 size)
{
  m_settings->setInt("Updates/DownloadSize", size);

  if (m_state == GettingUpdateXml) {
    if (!urls.isEmpty()) {
      if (m_downloadAll) {
        m_state = GettingUpdates;
        m_settings->notify(Settings::UpdateGetting);
        m_download->append(urls);
      }
      else
        m_settings->notify(Settings::UpdateAvailable);
    }
    else
      m_settings->notify(Settings::UpdateReady);
  }
  else if (m_state == GettingUpdates) {
    if (urls.isEmpty())
      m_settings->notify(Settings::UpdateReady);
    else
      execute();
  }
}
#endif


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
  int levelQt   = m_settings->getInt("Updates/LevelQt");
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
    #ifndef SCHAT_NO_UPDATE
      checkFiles();
    #else
      m_settings->notify(Settings::UpdateAvailable);
    #endif
}


/*!
 * Формирование списка файлов необходимых для установки обновления.
 *
 * Функция заполняет список \a m_files, если список будет пустым, пытаемся
 * скачать следующий xml файл и выходим из функции.
 *
 * Список файлов также записывается в настройки "Updates/Files".
 * В конце выполняется проверка скачанных файлов checkLocalFiles().
 */
#ifndef SCHAT_NO_UPDATE
void Update::checkFiles()
{
  QMultiMap<int, FileInfo> map = m_reader.files();
  QStringList files;
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
    execute();
    return;
  }

  m_settings->setList("Updates/Files", files);

  checkLocalFiles();
}
#endif


/*!
 * Проверка локальных файлов.
 * Наличие, размер, контрольная сумма.
 *
 * Проверка запускается в отдельном потоке, результатом будет вызов слота checkLocalFilesDone(const QStringList &urls, qint64 size)
 * Поток самостоятельно удалит себя при завершении.
 */
#ifndef SCHAT_NO_UPDATE
void Update::checkLocalFiles()
{
  QString url = m_reader.baseUrl();
  if (url.isEmpty())
    url = QFileInfo(m_xmlUrl.toString()).path() + "/";

  VerifyThread *thread = new VerifyThread(m_files, m_targetPath, url);
  connect(thread, SIGNAL(finished(const QStringList &, qint64)), SLOT(checkLocalFilesDone(const QStringList &, qint64)));
  thread->start();
}
#endif
