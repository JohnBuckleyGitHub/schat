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

#include <QNetworkAccessManager>
#include <QUrl>

#include "plugins/ChatPlugin.h"

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

  UpdatePluginImpl(QObject *parent);

public slots:
  void check();
  void finished();
  void readyRead();
  void startDownload();

private:
  DownloadState m_state;           ///< Состояние закачки.
  QByteArray m_rawJSON;            ///< Сырые JSON данные.
  QNetworkAccessManager m_manager; ///< Менеджер доступа к сети.
  QNetworkReply *m_current;        ///< Текущий ответ за запрос скачивания файла.
  QString m_prefix;                ///< Префикс настроек.
  QUrl m_url;                      ///< Текущий адрес.
};

#endif /* UPDATEPLUGIN_P_H_ */
