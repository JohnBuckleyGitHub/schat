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

#ifndef CHATVIEW_H_
#define CHATVIEW_H_

#include <QQueue>
#include <QVariant>
#include <QWebView>

#include "schat.h"

class FeedNotify;
class Message;
class Notify;

class SCHAT_CORE_EXPORT ChatView : public QWebView
{
  Q_OBJECT

public:
  ChatView(const QByteArray &id, const QString &url, QWidget *parent = 0);
  ~ChatView();

  inline const QByteArray& id() const { return m_id; }
  void add(const Message &msg);
  void addCSS(const QString &file);
  void copy();
  void evaluateJavaScript(const QString &func, const QVariant &param);
  void evaluateJavaScript(const QString &js);
  void setId(const QByteArray &id);

  Q_INVOKABLE QStringList jsfiles() const     { return m_jsfiles; }
  Q_INVOKABLE void addJS(const QString &file) { if (!m_jsfiles.contains(file)) m_jsfiles.append(file); }
  Q_INVOKABLE void loadFinished();

signals:
  void feed(const QVariantMap &data);
  void message(const QVariantMap &data);
  void reload();
  void reloaded();

protected:
  void changeEvent(QEvent *event);
  void contextMenuEvent(QContextMenuEvent *event);
  void showEvent(QShowEvent *event);

private slots:
  void menuTriggered(QAction *action);
  void notify(const Notify &notify);
  void openUrl(const QUrl &url);
  void populateJavaScriptWindowObject();
  void settingsChanged(const QString &key, const QVariant &value);
  void start();

private:
  QVariantMap addHint(const Message &message);
  void clearPage();
  void createActions();
  void retranslateUi();

  bool m_loaded;                       ///< true если документ загружен.
  QAction *m_clear;                    ///< Очистить.
  QAction *m_copy;                     ///< Копировать.
  QAction *m_copyLink;                 ///< Копировать ссылку.
  QAction *m_reload;                   ///< Обновить.
  QAction *m_seconds;                  ///< Секунды.
  QAction *m_selectAll;                ///< Выделить всё.
  QAction *m_service;                  ///< Сервисные сообщения.
  QByteArray m_id;                     ///< Идентификатор.
  QMap<qint64, QByteArray> m_messages; ///< Сортированные по времени сообщения.
  QQueue<QString> m_pendingJs;         ///< Очередь сообщений ожидающих загрузки документа.
  QQueue<QVariantMap> m_pending;       ///< Очередь сообщений ожидающих загрузки документа.
  QStringList m_jsfiles;               ///< Дополнительные динамически загружаемые JavaScript скрипты.
};

#endif /* CHATVIEW_H_ */
