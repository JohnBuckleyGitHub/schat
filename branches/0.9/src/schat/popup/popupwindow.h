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
  void openChat(bool open);

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

  /// Содержит сообщение, которое будет показано во всплывающем окне.
  struct Message {
    Message() {}
    Message(const QString &n, const QString &t, const QString &h, bool p)
    : nick(n), time(t), html(h), pub(p) {}

    QString nick; ///< Ник отправителя.
    QString time; ///< Время сообщения.
    QString html; ///< Тело сообщения.
    bool pub;     ///< Сообщение является обращением по имени из основного канала.
  };

  /// Установки для фиксированного размера окна.
  enum Size {
    Width = 240, ///< Ширина.
    Height = 90, ///< Высота.
    Space = 3    ///< Свободное пространство между окнами.
  };

  PopupWindow(const Message &message, QWidget *parent = 0);
  ~PopupWindow();
  void setMessage(const Message &message);
  void start(int slot = 1);

signals:
  void aboutToClose(const QString &nick, int slot);
  void openChat(const QString &nick, bool pub, bool open);

public slots:
  void close();
  void flash(const QString &style);
  void freeSlot(int slot);

protected:
  void mouseReleaseEvent(QMouseEvent *event);

private slots:
  void openChat(bool open);

private:
  void moveToSlot(int slot);

  class Private;
  Private *const d;
};

#endif /* POPUPWINDOW_H_ */
