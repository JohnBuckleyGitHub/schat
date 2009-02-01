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

#include <QtGui>

#include "abstractprofile.h"
#include "settings.h"
#include "userview.h"

/*!
 * Конструктор класса UserView.
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


/*!
 * Формирует строку для всплывающей подсказки, содержащую информацию о пользователе.
 *
 * \param profile Профиль пользователя.
 */
QString UserView::userToolTip(const AbstractProfile &profile)
{
  QString p_agent = profile.userAgent();
  p_agent.replace(QChar('/'), QChar(' '));
  QString p_name;
  profile.fullName().isEmpty() ? p_name = tr("<не указано>") : p_name = profile.fullName();

  return QString("<h3><img src='%1' align='left'> %2</h3>"
                 "<table><tr><td>%3</td><td>%4</td></tr>"
                 "<tr><td>%5</td><td>%6</td></tr>"
                 "<tr><td>%7</td><td>%8</td></tr></table>")
                 .arg(":/images/" + profile.gender() + ".png")
                 .arg(Qt::escape(profile.nick()))
                 .arg(tr("ФИО:"))
                 .arg(Qt::escape(p_name))
                 .arg(tr("Клиент:"))
                 .arg(Qt::escape(p_agent))
                 .arg(tr("Адрес:"))
                 .arg(Qt::escape(profile.host()));
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


void UserView::nickClicked(const QString &nick)
{
  emit insertNick(" <b>" + Qt::escape(nick) + "</b> ");
}


void UserView::mouseReleaseEvent(QMouseEvent *event)
{
  QModelIndex index = indexAt(event->pos());

  if (event->modifiers() == Qt::ControlModifier && event->button() == Qt::LeftButton && index.isValid()) {
    QStandardItem *item = m_model.itemFromIndex(index);

    nickClicked(item->text());
  }
  else
    QListView::mouseReleaseEvent(event);
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
  return Settings::findItem(&m_model, nick);
}
