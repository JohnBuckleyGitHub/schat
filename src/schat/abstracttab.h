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

#ifndef ABSTRACTTAB_H_
#define ABSTRACTTAB_H_

#include <QIcon>
#include <QWidget>

#include "chatwindow/chatview.h"

class Settings;

/*!
 * \brief Абстрактный базовый класс для вкладок чата.
 */
class AbstractTab : public QWidget {
  Q_OBJECT

public:
  enum Type {
    Unknown,
    Main,
    Private
  };

  AbstractTab(const QIcon &icon, QWidget *parent = 0);
  inline bool notice()                                                { return m_notice; }
  inline QIcon icon()                                                 { return m_icon; }
  inline Type type() const                                            { return m_type; }
  inline void addFilteredMsg(const QString &msg, bool strict = false) { m_view->addFilteredMsg(msg, strict); }
  inline void msg(const QString &text)                                { m_view->addServiceMsg(text); }
  inline void notice(bool enable)                                     { m_notice = enable; }
  inline void setChannel(const QString &ch)                           { m_view->channel(ch); }
  inline void setIcon(const QIcon &icon)                              { m_icon = icon; }

signals:
  void emoticonsClicked(const QString &emo);
  void nickClicked(const QString &hex);

public slots:
  inline bool copy()  { return m_view->copy(); }
  inline void clear() { m_view->clear(); }
  inline void addMsg(const QString &nick, const QString &message, int options = ChatView::MsgSend, bool notice = false) { m_view->addMsg(nick, message, options, notice); }

protected:
  bool m_notice;
  ChatView *m_view;
  QIcon m_icon;
  Type m_type;
};

#endif /*ABSTRACTTAB_H_*/
