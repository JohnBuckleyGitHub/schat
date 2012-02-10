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

#ifndef SCHAT_NO_WEBKIT
  #include <QWebView>
#else
  #include <QTextBrowser>
#endif

class ChatViewPrivate;
class QUrl;


/*!
 * \brief Обеспечивает отображение текста в чате.
 *
 * По умолчанию используется движок WebKit, но при компиляции
 * можно отказаться от него в пользу QTextBrowser.
 */
#ifndef SCHAT_NO_WEBKIT
class ChatView : public QWebView
#else
class ChatView : public QTextBrowser
#endif
{
  Q_OBJECT

public:

  /// Опции сообщения.
  enum MsgOptions {
    MsgSend    = 0x1, ///< Пользователь является отправителем сообщения.
    MsgRecived = 0x2, ///< Сообщение получено от другого пользователя.
    MsgPublic  = 0x4  ///< Публичное сообщение в главном канале.
  };

  ChatView(QWidget *parent = 0);
  ~ChatView();
  QString channel() const;
  static QString statusChangedNick(quint8 gender, const QString &oldNick, const QString &newNick);
  static QString statusNewUser(quint8 gender, const QString &nick);
  static QString statusUserLeft(quint8 gender, const QString &nick, const QString &bye);
  void addFilteredMsg(const QString &msg, bool strict = false);
  void addMsg(const QString &sender, const QString &message, int options = MsgSend, bool notice = false);
  void addServiceMsg(const QString &msg);
  void channel(const QString &ch);
  void log(bool enable);
  void scroll();

signals:
  void emoticonsClicked(const QString &emo);
  void nickClicked(const QString &nick);
  void popupMsg(const QString &nick, const QString &time, const QString &html, bool pub);

public slots:
  bool copy();
  void clear();

protected:
  void changeEvent(QEvent *event);
  void contextMenuEvent(QContextMenuEvent *event);

private slots:
  void linkClicked(const QUrl &url);
  void notify(int notify);
  void toggleServiceMessages(bool checked);
# ifndef SCHAT_NO_WEBKIT
  void loadFinished();
  void selectAll();
# endif

private:
  void appendMessage(const QString &message, bool sameFrom = false);
  void createActions();
  void retranslateUi();

  ChatViewPrivate *d;
};

#endif /*CHATVIEW_H_*/
