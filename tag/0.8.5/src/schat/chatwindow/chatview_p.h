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

#ifndef CHATVIEW_P_H_
#define CHATVIEW_P_H_

#include <QPointer>

class ChatView;
class ChatWindowStyleOutput;

/*!
 * \brief Приватный D-класс для класса ChatView.
 */
class ChatViewPrivate
{
public:
  #ifndef SCHAT_NO_WEBKIT
  ChatViewPrivate(const QString &styleName, const QString &styleVariant, ChatView *parent);
  #else
  ChatViewPrivate(ChatView *parent);
  #endif
  ~ChatViewPrivate();
  static bool prepareCmd(const QString &cmd, QString &msg, bool cut = true);
  void toLog(const QString &text);

  bool empty;
  bool log;
  bool strict;
  ChatView *q;
  QAction *autoScroll;
  QAction *clear;
  QAction *copy;

  QAction *serviceMessages;
  QPointer<ChannelLog> channelLog;
  QString channel;

# if QT_VERSION >= 0x040500
  QAction *selectAll;
# endif

  #ifndef SCHAT_NO_WEBKIT
    bool cleanStyle(const QString &styleName, const QString &styleVariant);

    bool grouping;
    bool loaded;
    ChatWindowStyleOutput *style;
    QQueue<QString> pendingJs;
    QString chatStyle;
    QString chatStyleVariant;
    QString prev;
  #else
    static QString makeMessage(const QString &sender, const QString &message, bool action = false);
    static QString makeStatus(const QString &message);
  #endif
};

#endif /* CHATVIEW_P_H_ */
