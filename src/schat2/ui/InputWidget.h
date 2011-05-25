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

#ifndef INPUTWIDGET_H_
#define INPUTWIDGET_H_

#include <QTextEdit>

class InputWidget : public QTextEdit
{
  Q_OBJECT

public:
  InputWidget(QWidget *parent = 0);
  inline QStringList history() const { return m_history; }
  void setMsg(int index);

signals:
  void send(const QString &text);

protected:
  void keyPressEvent(QKeyEvent *event);

public slots:
  void clear();
  void send();

private slots:
  void textChanged();

private:
  void nextMsg();
  void prevMsg();
  void setHeight(int lines);

  int m_current;             ///< Текущее сообщение в истории.
  int m_lines;               ///< Высота текста в строчках.
  QStringList m_history;     ///< Отправленные сообщения.
  QTextCharFormat m_default; ///< Формат текста по умолчанию.
};

#endif /* INPUTWIDGET_H_ */
