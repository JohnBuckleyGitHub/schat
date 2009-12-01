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

#ifndef USERVIEW_H_
#define USERVIEW_H_

#include <QLineEdit>
#include <QListView>
#include <QObject>
#include <QStandardItemModel>

class AbstractProfile;
class QMouseEvent;
class UserView;
class UserViewPrivate;

/*!
 * \brief Виджет для поиска пользователя.
 */
class QuickUserSearch : public QLineEdit
{
  Q_OBJECT

public:
  QuickUserSearch(UserView *parent);

private slots:
  inline void returnPressed()                 { quickSearch(text(), false); }
  inline void textEdited(const QString &text) { quickSearch(text); }

private:
  void quickSearch(const QString &text, bool reset = true);

  int m_pos;        ///< Сохранёная позиция поиска, используется для поиска следующего ника, удовлетворяющему текущему запросу.
  UserView *m_view; ///< Указатель на объект UserView.
};


/*!
 * \brief Список пользователей.
 */
class UserView : public QListView
{
  Q_OBJECT

public:
  enum ItemData {
    ProfileData = Qt::UserRole + 1,
    StatusData
  };

  UserView(const AbstractProfile *profile, QWidget *parent = 0);
  ~UserView();
  AbstractProfile profile(const QString &nick) const;
  bool add(const AbstractProfile &profile);
  bool add(const QStringList &list);
  bool isUser(const QString &nick) const;
  int quickSearch(const QString &nick, int pos = 0);
  static QString userToolTip(const AbstractProfile &profile);
  void clear();
  void remove(const QString &nick);
  void rename(const QString &oldNick, const QString &newNick);
  void setQuickSearch(QuickUserSearch *widget);
  void setStatus(quint32 status, const QStringList &users);
  void update(const QString &nick, const AbstractProfile &profile);

signals:
  void addTab(const QString &nick);
  void insertNick(const QString &nick);
  void showSettings();
  void usersCountChanged(int count);

public slots:
  void nickClicked(const QString &nick);

protected:
  void contextMenuEvent(QContextMenuEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void resizeEvent(QResizeEvent *event);

private slots:
  void addTab(const QModelIndex &index);
  void sort();

private:
  void updateQuickSearchVisible();

  UserViewPrivate * const d;
};

#endif /* USERVIEW_H_ */
