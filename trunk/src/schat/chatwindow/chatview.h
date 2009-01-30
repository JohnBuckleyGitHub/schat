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

#include <QWebView>

class QUrl;


/*!
 * \brief Обеспечивает отображение текста в чате.
 *
 * По умолчанию используется движок WebKit, но при компиляции
 * можно отказаться от него в пользу QTextBrowser.
 */
class ChatView : public QWebView
{
  Q_OBJECT

public:
  ChatView(QWidget *parent = 0);
  ~ChatView();
  QString channel() const;
  static QString statusChangedNick(quint8 gender, const QString &oldNick, const QString &newNick);
  static QString statusNewUser(quint8 gender, const QString &nick);
  static QString statusUserLeft(quint8 gender, const QString &nick, const QString &bye);
  void addFilteredMsg(const QString &msg, bool strict = false);
  void addMsg(const QString &sender, const QString &message, bool direction = true);
  void addServiceMsg(const QString &msg);
  void channel(const QString &ch);
  void log(bool enable);

signals:
  void nickClicked(const QString &hex);

public slots:
  bool copy();
  void clear();

protected:
  void contextMenuEvent(QContextMenuEvent *event);

private slots:
  void linkClicked(const QUrl &url);
  void notify(int notify);

private:
  bool prepareCmd(const QString &cmd, QString &msg, bool cut = true) const;
  void appendMessage(QString message, bool same_from = false);
  void createActions();
  void toLog(const QString &text);

  class ChatViewPrivate;
  ChatViewPrivate *d;
};

#endif /*CHATVIEW_H_*/
