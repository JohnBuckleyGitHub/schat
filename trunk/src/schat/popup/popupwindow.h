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

#ifndef POPUPWINDOW_H_
#define POPUPWINDOW_H_

#include <QTextBrowser>

/*!
 * \brief Вспомогательный класс для PopupWindow обеспечивающий отображение текста.
 */
class PopupTextBrowser : public QTextBrowser
{
  Q_OBJECT

public:
  PopupTextBrowser(QWidget *parent = 0);

signals:
  void closeWindow();
  void openChat();

protected:
  void mouseReleaseEvent(QMouseEvent * event);
};


/*!
 * \brief Всплывающее окно извещателя.
 */
class PopupWindow : public QFrame
{
  Q_OBJECT

public:
  PopupWindow(const QString &nick, const QString &time, const QString &html, QWidget *parent = 0);
  ~PopupWindow();
  void start();

signals:
  void aboutToClose(const QString &nick);
  void openChat(const QString &nick);

public slots:
  void close();

protected:
  void mouseReleaseEvent(QMouseEvent *event);

private slots:
  void flash();
  void openChat();

private:
  class Private;
  Private *const d;
};

#endif /* POPUPWINDOW_H_ */
