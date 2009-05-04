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

#ifndef SENDWIDGET_H_
#define SENDWIDGET_H_

#include <QPointer>
#include <QTextCharFormat>
#include <QWidget>

#include "widget/inputwidget.h"

class EmoticonSelector;
class InputWidget;
class QAction;
class QToolBar;
class QToolButton;

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
  void statusShortcut(int key);

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
  bool eventFilter(QObject *object, QEvent *event);
  QAction* createAction(const QString &name);
  void buildToolBar(const QStringList &actions);
  void initToolBar();
  void mergeFormat(const QTextCharFormat &format);

  InputWidget* const m_input;        ///< Виджет ввода текста.
  QPointer<QAction> m_bold;          ///< "Полужирный".
  QPointer<QAction> m_italic;        ///< "Курсив".
  QPointer<QAction> m_underline;     ///< "Подчёркнутый".
  QPointer<QToolButton> m_emoticons; ///< Кнопка для выбора смайлов.
  QPointer<QToolButton> m_send;      ///< Кнопка отправки.
  QStringList m_availableActions;    ///< Список кнопок которые можно добавить на панель инструментов.
  QToolBar *m_toolBar;               ///< Панель инструментов.
};

#endif /*SENDWIDGET_H_*/
