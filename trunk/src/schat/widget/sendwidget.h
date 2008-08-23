/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SENDWIDGET_H_
#define SENDWIDGET_H_

#include <QTextCharFormat>
#include <QWidget>

#include "widget/inputwidget.h"

class InputWidget;
class QAction;
class QToolButton;

class SendWidget : public QWidget
{
  Q_OBJECT

public:
  SendWidget(QWidget *parent = 0);
  inline void clear() { m_input->clearMsg(); }

signals:
  void sendMsg(const QString &message);

private slots:
  void currentCharFormatChanged(const QTextCharFormat &format);
  void setBold(bool b);
  void setItalic(bool b);
  void setUnderline (bool b);

private:
  void createButtons();
  void mergeFormat(const QTextCharFormat &format);

  InputWidget *m_input;
  QAction *m_boldAction;
  QAction *m_italicAction;
  QAction *m_sendAction;
  QAction *m_underlineAction;
  QToolButton *m_boldButton;
  QToolButton *m_italicButton;
  QToolButton *m_sendButton;
  QToolButton *m_underlineButton;
};

#endif /*SENDWIDGET_H_*/
