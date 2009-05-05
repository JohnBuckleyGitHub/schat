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
#include "protocol.h"
#include "settings.h"
#include "userview.h"
#include "userview_p.h"


/*!
 * Конструктор класса UserViewPrivate.
 */
UserViewPrivate::UserViewPrivate(const AbstractProfile *prof)
  : needSort(false), profile(prof)
{
  sortTimer.setInterval(300);
}


UserViewPrivate::~UserViewPrivate()
{
}


/*!
 * Выполняет поиск пользователя в модели.
 *
 * \param nick Ник пользователя.
 * \return QStandardItem в случае успешного поиска, иначе 0.
 */
QStandardItem* UserViewPrivate::item(const QString &nick) const
{
  QList<QStandardItem *> items;

  items = model.findItems(nick);
  if (items.size() > 0)
    return items[0];
  else
    return 0;
}


/*!
 * Сортировка списка пользователей, отложенная если
 * их больше 100, либо немедленная.
 */
void UserViewPrivate::sort()
{
  if (model.rowCount() > 100) {
    if (!needSort && !sortTimer.isActive()) {
      needSort = true;
      sortTimer.start();
    }
  }
  else
    sortNow();
}


/*!
 * Немедленная сортировка списка пользователей.
 */
void UserViewPrivate::sortNow()
{
  if (sortTimer.isActive())
    sortTimer.stop();

  needSort = false;
  model.sort(0);
}


/*!
 * Конструктор класса UserView.
 */
UserView::UserView(const AbstractProfile *profile, QWidget *parent)
  : QListView(parent), d(new UserViewPrivate(profile))
{
  setModel(&d->model);
  setFocusPolicy(Qt::NoFocus);
  setEditTriggers(QListView::NoEditTriggers);
  setSpacing(1);

  QPalette p = palette();
  if (p.color(QPalette::Base) == Qt::white) {
    setAlternatingRowColors(true);
    p.setColor(QPalette::AlternateBase, QColor("#f7faff"));
    setPalette(p);
  }

  connect(this, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(addTab(const QModelIndex &)));
  connect(&d->sortTimer, SIGNAL(timeout()), SLOT(sort()));
}


UserView::~UserView()
{
  delete d;
}


/*!
 * Добавление пользователя в список.
 */
bool UserView::add(const AbstractProfile &profile)
{
  QString nick = profile.nick();

  if (isUser(nick))
    return false;

  QStandardItem *item = new QStandardItem(QIcon(":/images/" + profile.gender() + ".png"), nick);
  item->setData(profile.pack(), ProfileData);
  item->setData(profile.status(), StatusData);
  item->setToolTip(userToolTip(profile));

  if (nick == d->profile->nick()) {
    QFont font;
    font.setBold(true);
    item->setFont(font);
  }

  d->model.appendRow(item);
  d->sort();

  return true;
}


bool UserView::add(const QStringList &list)
{
  AbstractProfile profile(list);
  return add(profile);
}


/*!
 * Проверка на наличие пользователя в списке.
 *
 * \return \a true если пользователь найден.
 */
bool UserView::isUser(const QString &nick) const
{
  return (bool) d->item(nick);
}


void UserView::clear()
{
  d->model.clear();
}


/*!
 * Возвращает профиль пользователя.
 * Перед вызовом функции необходимо убедится, что пользователь имеется в списке.
 */
AbstractProfile UserView::profile(const QString &nick) const
{
  QStandardItem *item = d->item(nick);
  AbstractProfile profile(item->data(ProfileData).toStringList());
  profile.setStatus(item->data(StatusData).toUInt());
  return profile;
}


/*!
 * Формирует строку для всплывающей подсказки, содержащую информацию о пользователе.
 *
 * \param profile Профиль пользователя.
 */
QString UserView::userToolTip(const AbstractProfile &profile)
{
  QString html = QString("<h3><img src='%1' align='left'> %2</h3><table>").arg(":/images/" + profile.gender() + ".png").arg(Qt::escape(profile.nick()));
  if (!profile.fullName().isEmpty())
    html += QString("<tr><td style='color:#90a4b3;'>%1</td><td>%2</td></tr>").arg(tr("ФИО:")).arg(Qt::escape(profile.fullName()));
  html += QString("<tr><td style='color:#90a4b3;'>%1</td><td>%2</td></tr>").arg(tr("Клиент:")).arg(Qt::escape(profile.userAgent().replace('/', ' ')));
  html += QString("<tr><td style='color:#90a4b3;'>%1</td><td>%2</td></tr>").arg(tr("Адрес:")).arg(Qt::escape(profile.host()));

  quint32 status = profile.status();
  html += QString("<tr><td style='color:#90a4b3;'>%1</td><td>").arg(tr("Статус:"));
  if (status == schat::StatusAway || status == schat::StatusAutoAway)
    html += tr("Отсутствую");
  else if (status == schat::StatusDnD)
    html += tr("Не беспокоить");
  else
    html += tr("В сети");

  html += "</td></tr></table>";

  return html;
}


void UserView::remove(const QString &nick)
{
  QStandardItem *item = d->item(nick);

  if (item) {
    d->model.removeRow(d->model.indexFromItem(item).row());
  }
}


void UserView::rename(const QString &oldNick, const QString &newNick)
{
  QStandardItem *item = d->item(oldNick);

  if (item) {
    item->setText(newNick);
    sort();
  }
}


/*!
 * Установка статуса пользователей.
 *
 * \param status Статус.
 * \param users  Список ников пользователей с заданным статусом.
 */
void UserView::setStatus(quint32 status, const QStringList &users)
{
  if (users.isEmpty())
    return;

  foreach (QString user, users) {
    QStandardItem *item = d->item(user);
    if (item) {
      if (status == schat::StatusAway || status == schat::StatusAutoAway)
        item->setForeground(QBrush(QColor("#90a4b3")));
      else if (status == schat::StatusDnD)
        item->setForeground(QBrush(QColor("#73bbef")));
      else
        item->setForeground(QPalette().brush(QPalette::WindowText));

      item->setData(status, StatusData);
      item->setToolTip(userToolTip(profile(user)));
    }
  }

  setCurrentIndex(QModelIndex());
}


void UserView::update(const QString &nick, const AbstractProfile &profile)
{
  QStandardItem *item = d->item(nick);

  if (item) {
    item->setIcon(QIcon(":/images/" + profile.gender() + ".png"));
    item->setToolTip(userToolTip(profile));
    item->setData(profile.pack(), ProfileData);
  }
}


void UserView::nickClicked(const QString &nick)
{
  emit insertNick(" <b>" + Qt::escape(nick) + "</b> ");
}


/*!
 * Контекстное меню.
 */
void UserView::contextMenuEvent(QContextMenuEvent *event)
{
  QModelIndex index = indexAt(event->pos());

  if (index.isValid()) {
    QStandardItem *item = d->model.itemFromIndex(index);
    QString nick = item->text();

    QAction *profileAction     = 0;
    QAction *privateMsgAction  = 0;

    QMenu menu(this);
    if (nick == d->profile->nick()) {
      profileAction = menu.addAction(QIcon(":/images/profile.png"), tr("Личные данные..."));
      profileAction->setShortcut(tr("Ctrl+F12"));
    }
    else
      privateMsgAction = menu.addAction(QIcon(":/images/im-status-message-edit.png"), tr("Приватное сообщение"));

    QMenu copyMenu(tr("Копировать"), this);
    copyMenu.setIcon(QIcon(":/images/edit-copy.png"));
    menu.addMenu(&copyMenu);

    AbstractProfile profile(item->data(ProfileData).toStringList(), this);
    QAction *copyNick = copyMenu.addAction(QIcon(":/images/profile.png"), tr("Ник"));
    QAction *copyFullName = 0;
    if (!profile.fullName().isEmpty())
      copyFullName = copyMenu.addAction(QIcon(":/images/profile.png"), tr("ФИО"));

    QAction *copyUserAgent = copyMenu.addAction(QIcon(":/images/logo16.png"), tr("Клиент"));
    QAction *copyHost = copyMenu.addAction(QIcon(":/images/applications-internet.png"), tr("Адрес"));

    menu.addSeparator();
    QAction *nickClickedAction = menu.addAction(tr("Вставить ник"));

    QAction *action = menu.exec(event->globalPos());
    if (action) {
      if (action == profileAction)
        emit showSettings();
      else if (action == privateMsgAction)
        addTab(index);
      else if (action == copyNick)
        QApplication::clipboard()->setText(nick);
      else if (action == copyFullName)
        QApplication::clipboard()->setText(profile.fullName());
      else if (action == copyUserAgent) {
        QString p_agent = profile.userAgent();
        p_agent.replace('/', ' ');
        QApplication::clipboard()->setText(p_agent);
      }
      else if (action == copyHost)
        QApplication::clipboard()->setText(profile.host());
      else if (action == nickClickedAction)
        nickClicked(nick);
    }
  }
}


/*!
 * Обработка событий от мыши.
 * - Если нажата кнопка Ctrl и левая кнопка мыши и индекс валидный
 * вызываем вставку ника.
 * - Для левой кнопки и не валидного индекса, снимаем выделение со всех итемов.
 * - Для всех остальных событий вызываем стандартный обработчик.
 */
void UserView::mouseReleaseEvent(QMouseEvent *event)
{
  QModelIndex index = indexAt(event->pos());

  if (event->modifiers() == Qt::ControlModifier && event->button() == Qt::LeftButton && index.isValid()) {
    QStandardItem *item = d->model.itemFromIndex(index);

    nickClicked(item->text());
  }
  else if (event->button() == Qt::LeftButton && !index.isValid())
    setCurrentIndex(QModelIndex());
  else
    QListView::mouseReleaseEvent(event);
}


void UserView::addTab(const QModelIndex &index)
{
  QStandardItem *item = d->model.itemFromIndex(index);
  QString nick = item->text();
  if (nick == d->profile->nick())
    return;

  emit addTab(nick);
}


void UserView::sort()
{
  d->sortNow();
}
