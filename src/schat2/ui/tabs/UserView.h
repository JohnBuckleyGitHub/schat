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

#ifndef USERVIEW_H_
#define USERVIEW_H_

#include <QListView>
#include <QStandardItemModel>

#include "User.h"

/*!
 * Итем в списке пользователей.
 */
class UserItem : public QStandardItem
{
public:
  UserItem(ClientUser user, int option);
  inline ClientUser user() { return m_user; }
  bool update();

private:
  void setColor();
  void setSortData();

  bool m_self;     ///< true это данные текущего пользователя.
  ClientUser m_user; ///< Пользователь.
};


/*!
 * Отображает список пользователей.
 */
class UserView : public QListView
{
  Q_OBJECT

public:
  enum AddOptions {
    NoOptions = 0,
    NoSort    = 1,
    SelfNick  = 2
  };

  UserView(QWidget *parent = 0);
  bool add(ClientUser user);
  bool remove(const QByteArray &id);
  bool update(ClientUser user);
  inline bool isSortable() const { return m_sortable; }
  inline QList<QByteArray> users() const { return m_users.keys(); }
  void clear();
  void sort();

protected:
  void contextMenuEvent(QContextMenuEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

private slots:
  void addTab(const QModelIndex &index);

private:
  bool m_sortable;                      ///< true если список пользователей нужно сортировать при добавлении пользователя.
  QHash<QByteArray, UserItem*> m_users; ///< Таблица для ускоренного поиска пользователей.
  QStandardItemModel m_model;           ///< Модель для отображения списка пользователей.
};

#endif /* USERVIEW_H_ */
