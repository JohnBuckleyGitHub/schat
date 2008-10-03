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

#include <QtCore>

#include "abstractprofile.h"
#include "userview.h"

/*!
 * \brief Конструктор класса UserView.
 */
UserView::UserView(const AbstractProfile *profile, QWidget *parent)
  : QListView(parent), m_profile(profile)
{
  setModel(&m_model);
  setFocusPolicy(Qt::NoFocus);
  setEditTriggers(QAbstractItemView::NoEditTriggers);

  connect(this, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(addTab(const QModelIndex &)));
}


bool UserView::add(const AbstractProfile &profile)
{
  QString nick = profile.nick();

  if (findItem(nick))
    return false;

  QStandardItem *item = new QStandardItem(QIcon(":/images/" + profile.gender() + ".png"), nick);
  item->setData(profile.pack(), Qt::UserRole + 1);
  item->setToolTip(userToolTip(profile));

  if (nick == m_profile->nick()) {
    QFont font;
    font.setBold(true);
    item->setFont(font);
  }

  m_model.appendRow(item);
  m_model.sort(0);

  return true;
}


bool UserView::add(const QStringList &list)
{
  AbstractProfile profile(list);
  return add(profile);
}


QStringList UserView::profile(const QString &nick) const
{
  QStandardItem *item = findItem(nick);
  return item->data(Qt::UserRole + 1).toStringList();
}


QString UserView::userToolTip(const AbstractProfile &profile)
{
  QString p_agent = profile.userAgent();
  p_agent.replace(QChar('/'), QChar(' '));
  QString p_name;
  profile.fullName().isEmpty() ? p_name = tr("&lt;не указано&gt;") : p_name = profile.fullName();

  return tr("<h3><img src='%1' align='left'> %2</h3>"
            "<table><tr><td>Настоящее имя:</td><td>%3</td></tr>"
            "<tr><td>Клиент:</td><td>%4</td></tr>"
            "<tr><td>IP-адрес:</td><td>%5</td></tr></table>")
            .arg(":/images/" + profile.gender() + ".png").arg(profile.nick()).arg(p_name).arg(p_agent).arg(profile.host());
}


void UserView::remove(const QString &nick)
{
  QStandardItem *item = findItem(nick);

  if (item)
    m_model.removeRow(m_model.indexFromItem(item).row());
}


void UserView::rename(const QString &oldNick, const QString &newNick)
{
  QStandardItem *item = findItem(oldNick);

  if (item) {
    item->setText(newNick);
    m_model.sort(0);
  }
}


void UserView::update(const QString &nick, const AbstractProfile &profile)
{
  QStandardItem *item = findItem(nick);

  if (item) {
    item->setIcon(QIcon(":/images/" + profile.gender() + ".png"));
    item->setToolTip(userToolTip(profile));
    item->setData(profile.pack(), Qt::UserRole + 1);
  }
}


void UserView::addTab(const QModelIndex &index)
{
  QStandardItem *item = m_model.itemFromIndex(index);
  QString nick = item->text();
  if (nick == m_profile->nick())
    return;

  emit addTab(nick);
}


QStandardItem* UserView::findItem(const QString &nick) const
{
  QList<QStandardItem *> items;

  items = m_model.findItems(nick, Qt::MatchCaseSensitive);
  if (items.size() == 1)
    return items[0];
  else
    return 0;
}
