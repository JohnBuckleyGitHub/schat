/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include <QPointer>
#include <QWidget>

#include "channellog.h"

class ChatWindowStyleOutput;
class QUrl;
class QWebView;
class Settings;

/*!
 * \brief Обеспечивает отображение текста в чате.
 *
 * По умолчанию используется движок WebKit, но при компиляции
 * можно отказаться от него в пользу QTextBrowser.
 */
class ChatView : public QWidget
{
  Q_OBJECT

public:
  ChatView(QWidget *parent = 0);
  ~ChatView();
  inline QString channel() const         { return m_channel; }
  inline void channel(const QString &ch) { m_channel = ch; if (m_channelLog) m_channelLog->setChannel(ch); }
  void addMsg(const QString &sender, const QString &message, bool direction = true);
  void addServiceMsg(const QString &msg);
  void log(bool enable);

private slots:
  void linkClicked(const QUrl &url);

private:
  void appendMessage(QString message, bool same_from = false);

  bool m_log;
  ChatWindowStyleOutput *m_style;
  QPointer<ChannelLog> m_channelLog;
  QString m_channel;
  QString m_prev;
  QWebView *m_view;
  Settings *m_settings;
};

#endif /*CHATVIEW_H_*/
