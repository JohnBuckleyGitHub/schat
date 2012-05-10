/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#ifndef UPDATEPLUGIN_P_H_
#define UPDATEPLUGIN_P_H_

#include <QFile>
#include <QNetworkAccessManager>
#include <QUrl>

#include "plugins/ChatPlugin.h"

class QCryptographicHash;
class QNetworkReply;

class UpdatePluginImpl : public ChatPlugin
{
  Q_OBJECT

public:
  /// Состояние закачки.
  enum DownloadState {
    Idle,          ///< Нет активной закачки.
    DownloadJSON,  ///< Закачка JSON файла с информацией об обновлении.
    DownloadUpdate ///< Закачка файла обновления.
  };

  /// Статус проверки обновлений.
  enum Status {
    Unknown,         ///< Обновления не проверялись.
    Unsupported,     ///< Обновления для текущей платформы не поддерживаются.
    CheckError,      ///< Не удалось получить информацию об обновлениях.
    NoUpdates,       ///< Нет доступных обновлений, установлена последняя версия.
    UpdateAvailable, ///< Доступно обновление.
    DownloadError,   ///< Ошибка при скачивании обновления.
    UpdateReady      ///< Обновление скачано и готово к применению.
  };

  UpdatePluginImpl(QObject *parent);
  ~UpdatePluginImpl();
  inline Status status() const { return m_status; }

signals:
  void done(int status);

public slots:
  void check();
  void download();
  void finished();
  void readyRead();
  void startDownload();

private:
  void checkUpdate();
  void readJSON();
  void setDone(Status status);

  DownloadState m_state;           ///< Состояние закачки.
  QByteArray m_rawJSON;            ///< Сырые JSON данные.
  QCryptographicHash *m_sha1;      ///< Класс для проверки SHA1 хеша файла.
  QNetworkAccessManager m_manager; ///< Менеджер доступа к сети.
  QNetworkReply *m_current;        ///< Текущий ответ за запрос скачивания файла.
  QString m_prefix;                ///< Префикс настроек.
  QUrl m_url;                      ///< Текущий адрес.
  Status m_status;                 ///< Статус проверки обновлений.

  // Информация об обновлении.
  int m_revision;                  ///< SVN ревизия.
  int m_size;                      ///< Размер обновлений.
  QByteArray m_hash;               ///< SHA1 контрольная сумма обновлений.
  QFile m_file;                    ///< Файл обновлений.
  QString m_version;               ///< Доступная версия.
};

#endif /* UPDATEPLUGIN_P_H_ */
