/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#ifndef CHATVIEW_H_
#define CHATVIEW_H_

#include <QQueue>
#include <QVariant>
#include <QWebView>

#include "schat.h"

class UserMessage;
class ChatViewPrivate;

class SCHAT_CORE_EXPORT ChatView : public QWebView
{
  Q_OBJECT

public:
  ChatView(const QByteArray &id, const QString &url, QWidget *parent = 0);
  void evaluateJavaScript(const QString &js);

protected:
  void changeEvent(QEvent *event);
  void contextMenuEvent(QContextMenuEvent *event);

private slots:
  void loadFinished();
  void menuTriggered(QAction *action);
  void openUrl(const QUrl &url);
  void populateJavaScriptWindowObject();
  void settingsChanged(const QString &key, const QVariant &value);

private:
  void createActions();
  void retranslateUi();
  void showSeconds(bool show);
  void showService(bool show);

  bool m_loaded;                      ///< true если документ загружен.
  QAction *m_clear;                   ///< Очистить.
  QAction *m_copy;                    ///< Копировать.
  QAction *m_copyLink;                ///< Копировать ссылку.
  QAction *m_seconds;                 ///< Секунды.
  QAction *m_selectAll;               ///< Выделить всё.
  QAction *m_service;                 ///< Сервисные сообщения.
  QByteArray m_id;                    ///< Идентификатор.
  QQueue<QString> m_pendingJs;        ///< Очередь сообщений ожидающих загрузки документа.
};

#endif /* CHATVIEW_H_ */
