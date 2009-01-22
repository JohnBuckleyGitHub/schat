/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 - 2009 IMPOMEZIA <schat@impomezia.com>
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

#ifndef CHATBROWSER_H_
#define CHATBROWSER_H_

#include <QHash>
#include <QPointer>
#include <QQueue>
#include <QTextBrowser>
#include <QTime>
#include <QTimer>

#include "channellog.h"

class EmoticonMovie;
class QAction;
class Settings;


/*!
 * \brief Обеспечивает отображение текста в чате.
 */
class ChatBrowser : public QTextBrowser {
  Q_OBJECT

public:
  ChatBrowser(QWidget *parent = 0);
  inline QString channel() const                 { return m_channel; }
  inline void setChannel(const QString &channel) { m_channel = channel; if (m_channelLog) m_channelLog->setChannel(channel); }
  static inline QString currentTime()            { return QTime::currentTime().toString("hh:mm:ss"); }
  static QString msgAccessDenied(quint16 reason);
  static QString msgBadNickName(const QString &nick);
  static QString msgChangedNick(quint16 sex, const QString &oldNick, const QString &newNick);
  static QString msgDisconnect();
  static QString msgLinkLeave(const QString &network, const QString &name);
  static QString msgNewLink(const QString &network, const QString &name);
  static QString msgNewUser(quint8 sex, const QString &nick);
  static QString msgOldClientProtocol();
  static QString msgOldServerProtocol();
  static QString msgReadyForUse(const QString &addr);
  static QString msgReadyForUse(const QString &network, const QString &addr);
  static QString msgUnknownCmd(const QString &command);
  static QString msgUserLeft(quint8 sex, const QString &nick, const QString &bye);
  void log(bool enable);
  void msg(const QString &text);
  void scroll();

signals:
  void nickClicked(const QString &hex);
  void pauseAnimations(bool paused);
  void pauseIfHidden(int min, int max);

protected:
  inline bool viewportEvent(QEvent *event)     { setAnimations(); return QTextBrowser::viewportEvent(event); }
  inline void hideEvent(QHideEvent* /*event*/) { emit setPauseAnimations(true); }
  inline void showEvent(QShowEvent* /*event*/) { emit setPauseAnimations(false); }
  void contextMenuEvent(QContextMenuEvent *event);

public slots:
  void clear();
  void msgNewMessage(const QString &nick, const QString &message);

private slots:
  void animate();
  void animate(const QString &key);
  void linkClicked(const QUrl &link);
  void setAnimations();
  void setSettings();

private:
  bool prepareCmd(const QString &cmd, QString &msg, bool cut = true) const;
  void addAnimation(const QString &fileName, int pos = -1, int starts = -1);
  void createActions();
  void setPauseAnimations(bool paused);
  void toLog(const QString &text);

  bool m_emoticonsRequireSpaces;
  bool m_log;
  bool m_useAnimatedEmoticons;
  bool m_useEmoticons;
  QAction *m_clearAction;
  QAction *m_copyAction;
  QAction *m_selectAllAction;
  QHash<QString, EmoticonMovie*> m_aemoticon;
  QPointer<ChannelLog> m_channelLog;
  QQueue<int> m_animateQueue;
  QString m_channel;
  QString m_style;
  QTimer m_animateTimer;
  Settings *m_settings;
};

#endif /*CHATBROWSER_H_*/
