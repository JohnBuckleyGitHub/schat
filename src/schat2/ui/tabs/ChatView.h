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

class ChatMessage;
class ChatViewPrivate;

class ChatView : public QWebView
{
  Q_OBJECT

public:
  /// Опции.
  enum Options {
    ShowSeconds,     ///< Показывать секунды.
    EnableDeveloper, ///< Разрешить использование средств разработки.
    UserServerTime   ///< Использовать время сервера для сообщений.
  };

  ChatView(QWidget *parent = 0);
  QVariant option(Options key) const;
  void append(const ChatMessage &message);
  void appendRawMessage(const QString &message);
  void setOption(Options key, const QVariant &value);

signals:
  void nickClicked(const QByteArray &userId);

public slots:
  void nickClicked(const QString &arg1);

private slots:
  void loadFinished();
  void populateJavaScriptWindowObject();

private:
  QByteArray userIdFromClass(const QString &text);
  void appendUserMessage(const ChatMessage &message);
  void setMessageState(const QString &id, const QString &state);
  void setText(QString &html, const QString &text);
  void setTimeStamp(QString &html);

  bool m_loaded;                      ///< true если документ загружен.
  ChatViewPrivate *m_d;               ///< Приватный класс.
  QHash<Options, QVariant> m_options; ///< Опции.
  QQueue<QString> m_pendingJs;        ///< Очередь сообщений ожидающих загрузки документа.
};

#endif /* CHATVIEW_H_ */
