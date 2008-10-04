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

#ifndef USERVIEW_H_
#define USERVIEW_H_

#include <QListView>
#include <QObject>
#include <QStandardItemModel>

class AbstractProfile;
class QMouseEvent;

/*!
 * \brief Список пользователей.
 */
class UserView : public QListView {
  Q_OBJECT

public:
  UserView(const AbstractProfile *profile, QWidget *parent = 0);
  bool add(const AbstractProfile &profile);
  bool add(const QStringList &list);
  inline bool isUser(const QString &nick) const { return (bool) findItem(nick); }
  inline void clear()                           { m_model.clear(); }
  QStringList profile(const QString &nick) const;
  static QString userToolTip(const AbstractProfile &profile);
  void remove(const QString &nick);
  void rename(const QString &oldNick, const QString &newNick);
  void update(const QString &nick, const AbstractProfile &profile);

signals:
  void addTab(const QString &nick);
  void insertNick(const QString &nick);

public slots:
  void nickClicked(const QString &hex);

protected:
  void mouseReleaseEvent(QMouseEvent *event);

private slots:
  void addTab(const QModelIndex &index);

private:
  QStandardItem* findItem(const QString &nick) const;
  void prepareInsertNick(const QString &nick);

  const AbstractProfile *m_profile;
  QStandardItemModel m_model;
};

#endif /* USERVIEW_H_ */
