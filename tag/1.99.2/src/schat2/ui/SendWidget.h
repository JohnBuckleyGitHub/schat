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

#ifndef SENDWIDGET_H_
#define SENDWIDGET_H_

#include <QWidget>

#include "schat.h"

class InputWidget;
class Notify;
class QMenu;
class QToolBar;
class QToolButton;

class SCHAT_CORE_EXPORT SendWidget : public QWidget
{
  Q_OBJECT

public:
  SendWidget(QWidget *parent = 0);
  inline InputWidget *input() { return m_input; }
  void setInputFocus();

signals:
  void send(const QString &text);

public slots:
  void insertHtml(const QString &text);

protected:
  bool event(QEvent *event);
  void changeEvent(QEvent *event);

private slots:
  void notify(const Notify &notify);
  void sendMsg(const QString &text);
  void showHistoryItem();
  void showHistoryMenu();

private:
  void fillToolBar();
  void retranslateUi();
  void updateStyleSheet();

  InputWidget *m_input;      ///< Виджет ввода текста.
  QAction *m_sendAction;     ///< Кнопка отправки.
  QMenu *m_history;          ///< Меню отправленных сообщений.
  QToolBar *m_toolBar;       ///< Панель инструментов.
  QToolButton *m_sendButton; ///< Кнопка отправки.
};

#endif /* SENDWIDGET_H_ */
