/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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

class EmoticonSelector;
class InputWidget;
class QAction;
class QToolButton;
class Settings;

/*!
 * \brief Виджет полностью берущий на себя ввода текста.
 */
class SendWidget : public QWidget
{
  Q_OBJECT

public:
  SendWidget(QWidget *parent = 0);
  inline void clear() { m_input->clearMsg(); }

signals:
  void needCopy();
  void sendMsg(const QString &message);

public slots:
  void copy()                          { m_input->copy(); }
  void insertHtml(const QString &text) { m_input->insertHtml(text); }

private slots:
  void cursorPositionChanged();
  void log();
  void setBold(bool b);
  void setItalic(bool b);
  void setSettings();
  void setUnderline (bool b);

private:
  void createButtons();
  void mergeFormat(const QTextCharFormat &format);

  EmoticonSelector *m_emoticonSelector;
  InputWidget *m_input;
  QAction *m_boldAction;
  QAction *m_italicAction;
  QAction *m_sendAction;
  QAction *m_underlineAction;
  QAction *m_logAction;
  QMenu *m_popup;
  QToolButton *m_boldButton;
  QToolButton *m_emoticonButton;
  QToolButton *m_italicButton;
  QToolButton *m_sendButton;
  QToolButton *m_underlineButton;
  QToolButton *m_logButton;
  Settings *m_settings;
};

#endif /*SENDWIDGET_H_*/
