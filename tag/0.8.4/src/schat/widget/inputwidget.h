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

#ifndef INPUTWIDGET_H_
#define INPUTWIDGET_H_

#include <QTextEdit>

class QAction;

/*!
 * \brief Базовый виджет для ввода текста.
 */
class InputWidget : public QTextEdit
{
  Q_OBJECT

public:
  InputWidget(QWidget *parent = 0);

signals:
  void needCopy();
  void sendMsg(const QString &message);

public slots:
  void clearMsg();
  void paste();
  void sendMsg();

protected:
  void changeEvent(QEvent *event);
  void contextMenuEvent(QContextMenuEvent *event);
  void keyPressEvent(QKeyEvent *event);

private slots:
  void textChanged();

private:
  void createActions();
  void nextMsg();
  void prevMsg();
  void retranslateUi();
  void setHeight(int lines);

  int m_current;
  int m_lines;               ///< Высота текста в строчках.
  QAction *m_clearAction;
  QAction *m_copyAction;
  QAction *m_cutAction;
  QAction *m_pasteAction;
  QAction *m_selectAllAction;
  QStringList m_msg;
  QTextCharFormat m_default;
};

#endif /*INPUTWIDGET_H_*/
