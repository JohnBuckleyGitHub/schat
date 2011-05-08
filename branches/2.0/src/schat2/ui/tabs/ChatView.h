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

#include <QWidget>

class ChatViewPrivate;
class MessageData;
class QVBoxLayout;
class User;

class ChatView : public QWidget
{
public:
  ChatView(QWidget *parent = 0);
  QVBoxLayout* layout();
  static QString timeStamp();
  void append(int status, User *user, const MessageData &data);
  void appendRawText(const QString &text);

private:
  ChatViewPrivate *d;
};

#endif /* CHATVIEW_H_ */
