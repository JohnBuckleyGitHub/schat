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

#include <QClipboard>
#include <QContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QScrollBar>
#include <QTextDocument>

#include "abstractprofile.h"
#include "chatwindow/chatview.h"
#include "privatetab.h"
#include "protocol.h"
#include "settings.h"
#include "simplechatapp.h"
#include "userview.h"
#include "userview_p.h"

/*!
 * Конструктор класса UserItem.
 */
UserItem::UserItem(const AbstractProfile &profile, QTabWidget *tabs)
  : QStandardItem(profile.nick()),
  m_upped(false),
  m_profile(profile),
  m_tab(0),
  m_tabs(tabs)
{
  updateIcon();
  updateToolTip();
  setSortData();
}


bool UserItem::isOpenTab() const
{
  if (m_tab)
    return true;

  return false;
}


/*!
 * Возвращает вкладку привата.
 */
PrivateTab* UserItem::privateTab(bool create)
{
  if (create && !m_tab) {
    m_tab = new PrivateTab(icon(), m_tabs);
    m_tab->setChannel(m_profile.nick());
    int index = m_tabs->addTab(m_tab, icon(), m_profile.nick());
    m_tabs->setTabToolTip(index, toolTip());
  }
  return m_tab;
}


/*!
 * Формирует строку для всплывающей подсказки, содержащую информацию о пользователе.
 *
 * \param profile Профиль пользователя.
 */
QString UserItem::userToolTip(const AbstractProfile &profile)
{
  QString html = QString("<h3><img src='%1' align='left'> %2</h3><table>").arg(":/images/" + profile.gender() + ".png").arg(Qt::escape(profile.nick()));
  QString line = "<tr><td style='color:#90a4b3;'>%1</td><td>%2</td></tr>";

  if (!profile.fullName().isEmpty())
    html += QString(line).arg(UserView::tr("Name:")).arg(Qt::escape(profile.fullName()));

  QStringList userAgent = profile.userAgent().split("/");
  if (userAgent.size() == 2) {
    if (userAgent.at(0) == "IMPOMEZIA Simple Chat")
      html += QString(line).arg(UserView::tr("Version:")).arg(Qt::escape(userAgent.at(1)));
    else
      html += QString(line).arg(UserView::tr("Client:")).arg(Qt::escape(userAgent.at(0) + " " + userAgent.at(1)));
  }
  html += QString(line).arg(UserView::tr("Address:")).arg(Qt::escape(profile.host()));

  quint32 status = profile.status();
  html += QString("<tr><td style='color:#90a4b3;'>%1</td><td>").arg(UserView::tr("Status:"));
  if (status == schat::StatusAway || status == schat::StatusAutoAway)
    html += UserView::tr("Away");
  else if (status == schat::StatusDnD)
    html += UserView::tr("DND");
  else if (status == schat::StatusOffline)
    html += UserView::tr("Offline");
  else
    html += UserView::tr("Online");

  html += "</td></tr></table>";

  return html;
}


void UserItem::offline()
{
  if (m_tab) {
    m_profile.setStatus(schat::StatusOffline);
    updateIcon();
    m_tabs->setTabToolTip(m_tabs->indexOf(m_tab), userToolTip(m_profile));
  }
}


void UserItem::pickUp()
{
  m_upped = true;
  setSortData();
}

/*!
 * Установка нового статуса.
 *
 * \param status Новый статус.
 */
void UserItem::setStatus(quint32 status)
{
  if (status == schat::StatusAway || status == schat::StatusAutoAway)
    setForeground(QBrush(QColor(144, 164, 179)));
  else if (status == schat::StatusDnD)
    setForeground(QBrush(QColor(115, 187, 239)));
  else
    setForeground(QPalette().brush(QPalette::WindowText));

  m_profile.setStatus(status);
  updateIcon();
  updateToolTip();
}


void UserItem::setTab(PrivateTab *tab)
{
  m_tab = tab;
}


/*!
 * Обновление информации о пользователе.
 *
 * \param profile Новый профиль.
 */
void UserItem::update(const AbstractProfile &profile)
{
  if (m_profile.nick() != profile.nick()) {
    setText(profile.nick());
  }

  m_profile = profile;
  updateIcon();
  updateToolTip();
  setSortData();
}


QIcon UserItem::drawIcon() const
{
  QString file = ":/images/" + m_profile.gender() + ".png";
  quint32 status = m_profile.status();

  if (status == schat::StatusOffline)
    return QIcon(QIcon(file).pixmap(16, 16, QIcon::Disabled));

  if (status == schat::StatusNormal)
    return QIcon(file);

  QPixmap pixmap(file);
  QPainter painter(&pixmap);
  if (status == schat::StatusAway || status == schat::StatusAutoAway)
    painter.drawPixmap(6, 6, QPixmap(":/images/status/small/away.png"));
  else if (status == schat::StatusDnD)
    painter.drawPixmap(6, 6, QPixmap(":/images/status/small/dnd.png"));

  painter.end();
  return QIcon(pixmap);
}


void UserItem::setSortData()
{
  if (m_upped)
    setData("!" + m_profile.nick().toLower());
  else
    setData("5" + m_profile.nick().toLower());
}


/*!
 * Обновление иконки.
 */
void UserItem::updateIcon()
{
  setIcon(drawIcon());

  if (m_tab) {
    m_tab->setIcon(icon());

    if (!m_tab->notice())
      m_tabs->setTabIcon(m_tabs->indexOf(m_tab), icon());
  }
}


/*!
 * Обновление всплывающей подсказки.
 */
void UserItem::updateToolTip()
{
  setToolTip(userToolTip(m_profile));

  if (m_tab)
    m_tabs->setTabToolTip(m_tabs->indexOf(m_tab), toolTip());
}


/*!
 * Конструктор класса QuickUserSearch.
 */
QuickUserSearch::QuickUserSearch(UserView *parent)
  : QLineEdit(parent),
  m_pos(0),
  m_view(parent)
{
  setFrame(false);
  connect(this, SIGNAL(textEdited(const QString &)), SLOT(textEdited(const QString &)));
  connect(this, SIGNAL(returnPressed()), SLOT(returnPressed()));
  setVisible(false);
  retranslateUi();
}


void QuickUserSearch::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QLineEdit::changeEvent(event);
}


/*!
 * Базовая функция быстрого поиска пользователя.
 * В случае неудачного поиска для виджета устанавливается красная палитра.
 *
 * \param text  Поисковый запрос, первые символы ника или ник целиком.
 * \param reset Указывает на необходимость поиска первого ника.
 */
void QuickUserSearch::quickSearch(const QString &text, bool reset)
{
  QPalette pal = palette();
  if (reset)
    m_pos = 0;

  m_pos = m_view->quickSearch(text, m_pos);
  if (m_pos != -1 || text.isEmpty()) {
    pal.setColor(QPalette::Active, QPalette::Base, Qt::white);
    m_pos++;
  }
  else
    pal.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));

  setPalette(pal);
}


void QuickUserSearch::retranslateUi()
{
  #if QT_VERSION >= 0x040700
  setPlaceholderText(tr("Search..."));
  #endif
}


/*!
 * Конструктор класса UserViewPrivate.
 */
UserViewPrivate::UserViewPrivate(const AbstractProfile *profile, QTabWidget *tabs)
  : needSort(false),
  profile(profile),
  tabs(tabs),
  quickUserSearch(0)
{
  sortTimer.setInterval(300);
  model.setSortRole(Qt::UserRole + 1);
}


UserViewPrivate::~UserViewPrivate()
{
}


/*!
 * Выполняет поиск пользователя в модели.
 *
 * \param nick  Ник пользователя.
 * \param flags Опции поиска.
 * \return UserItem в случае успешного поиска, иначе 0.
 */
UserItem* UserViewPrivate::item(const QString &nick, Qt::MatchFlags flags) const
{
  QList<QStandardItem *> items;

  items = model.findItems(nick, flags);
  if (items.size() > 0)
    return static_cast<UserItem *>(items[0]);
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
UserView::UserView(const AbstractProfile *profile, QTabWidget *tabs, QWidget *parent)
  : QListView(parent), d(new UserViewPrivate(profile, tabs))
{
  setModel(&d->model);
  setFocusPolicy(Qt::TabFocus);
  setEditTriggers(QListView::NoEditTriggers);
  setSpacing(1);
  setFrameShape(QFrame::NoFrame);

  QPalette p = palette();
  if (p.color(QPalette::Base) == Qt::white) {
    setAlternatingRowColors(true);
    p.setColor(QPalette::AlternateBase, QColor(247, 250, 255));
    setPalette(p);
  }

  setTextElideMode(Qt::ElideRight);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setStyleSheet("QScrollBar:vertical{background:transparent;width:6px}QScrollBar::handle:vertical{background:#ccc;border-radius:3px}QScrollBar::sub-line:vertical,QScrollBar::add-line:vertical{height:0}");

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
bool UserView::add(const AbstractProfile &profile, quint8 echo)
{
  QString nick = profile.nick();

  if (isUser(nick))
    return false;

  UserItem *item = new UserItem(profile, d->tabs);

  // Свой ник выделяем жирным и отключаем эхо.
  if (nick == d->profile->nick()) {
    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);
    item->pickUp();
    echo = 0;
  }

  // Ищем открытый приват и если находим обновляем его.
  PrivateTab *tab = tabFromName(nick);
  if (tab) {
    item->setTab(tab);
    item->update(profile);

    if (echo == 1 && SimpleSettings->getBool("ServiceMessages"))
      tab->msg(ChatView::statusNewUser(profile.genderNum(), nick));
  }

  d->model.appendRow(item);
  d->sort();
  emit usersCountChanged(d->model.rowCount(QModelIndex()));

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


/*!
 * Выполняет быстрый поиск ника по первым символам.
 *
 * \param nick Поисковый запрос, первые символы ника или ник целиком.
 * \param pos  Позиция поиска после, которой необходимо найти ник.
 *
 * \return -1 в случае неуспешного поиска, или позиция ника.
 */
int UserView::quickSearch(const QString &nick, int pos)
{
  if (nick.isEmpty())
    return -1;

  QList<QStandardItem *> items = d->model.findItems(nick, Qt::MatchStartsWith | Qt::MatchWrap);
  if (!items.size())
    return -1;

  if (items.size() <= pos)
    pos = 0;

  QStandardItem *item = items[pos];

  QModelIndex index = d->model.indexFromItem(item);
  setCurrentIndex(index);
  scrollTo(index);

  return pos;
}


PrivateTab* UserView::privateTab(const QString &nick, bool create)
{
  UserItem *item = d->item(nick);
  PrivateTab *tab = tabFromName(nick);
  if (!item)
    return tab;

  if (tab) {
    item->setTab(tab);
  }
  else {
    bool isOpen = item->isOpenTab();
    tab = item->privateTab(create);
    if (tab && !isOpen) {
      connect(tab, SIGNAL(nickClicked(const QString &)), SLOT(nickClicked(const QString &)));
      connect(tab, SIGNAL(emoticonsClicked(const QString &)), SIGNAL(emoticonsClicked(const QString &)));
      connect(tab, SIGNAL(popupMsg(const QString &, const QString &, const QString &, bool)), SIGNAL(popupMsg(const QString &, const QString &, const QString &, bool)));
    }
  }

  return tab;
}


/*!
 * Поиск открытого привата, по нику.
 */
PrivateTab* UserView::tabFromName(const QString &text) const
{
  int count = d->tabs->count();

  if (count > 0) {
    for (int i = 0; i < count; ++i)
      if (d->tabs->tabText(i) == text) {
        AbstractTab *tab = static_cast<AbstractTab *>(d->tabs->widget(i));
        if (tab->type() == AbstractTab::Private)
          return static_cast<PrivateTab *>(tab);
      }
  }

  return 0;
}


void UserView::clear()
{
  d->model.clear();
  emit usersCountChanged(0);
}


/*!
 * Возвращает профиль пользователя.
 * Перед вызовом функции необходимо убедится, что пользователь имеется в списке.
 */
AbstractProfile UserView::profile(const QString &nick) const
{
  UserItem *item = d->item(nick);
  return item->profile();
}


void UserView::remove(const QString &nick)
{
  UserItem *item = d->item(nick);

  if (item) {
    item->offline();
    d->model.removeRow(d->model.indexFromItem(item).row());
    emit usersCountChanged(d->model.rowCount(QModelIndex()));
  }
}


void UserView::setQuickSearch(QuickUserSearch *widget)
{
  d->quickUserSearch = widget;
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
    UserItem *item = d->item(user);
    if (item)
      item->setStatus(status);
  }

  setCurrentIndex(QModelIndex());
}


/*!
 * Обновление информации о пользователе.
 */
void UserView::update(const QString &nick, const AbstractProfile &profile)
{
  UserItem *item = d->item(nick);
  if (!item)
    return;

  if (nick != profile.nick()) {
    PrivateTab *oldTab = tabFromName(nick);
    PrivateTab *newTab = tabFromName(profile.nick());
    QString newNick = profile.nick();
    QString html = ChatView::statusChangedNick(profile.genderNum(), nick, profile.nick());

    if (oldTab) {
      if (!newTab) {
        d->tabs->setTabText(d->tabs->indexOf(oldTab), newNick);
        oldTab->setChannel(newNick);
        item->setTab(oldTab);
      }
      else {
        newTab->msg(html);
        item->setTab(newTab);
        if (d->tabs->currentIndex() == d->tabs->indexOf(oldTab))
          d->tabs->setCurrentIndex(d->tabs->indexOf(newTab));
      }
      oldTab->msg(html);
    }

    item->update(profile);
    sort();
  }
  else
    item->update(profile);
}


void UserView::update(const QString &nick, const QString &newNick, const QString &name, quint8 gender)
{
  AbstractProfile p(profile(nick));
  p.setNick(newNick);
  p.setGender(gender);
  p.setFullName(name);
  update(nick, p);
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
    UserItem *item = static_cast<UserItem *>(d->model.itemFromIndex(index));
    QString nick = item->text();

    QAction *profileAction     = 0;
    QAction *privateMsgAction  = 0;

    QMenu menu(this);
    if (nick == d->profile->nick())
      profileAction = menu.addAction(QIcon(":/images/profile.png"), tr("Personal data..."));
    else
      privateMsgAction = menu.addAction(QIcon(":/images/balloon.png"), tr("Private message"));

    QMenu copyMenu(tr("Copy"), this);
    copyMenu.setIcon(SimpleChatApp::iconFromTheme("edit-copy"));
    menu.addMenu(&copyMenu);

    AbstractProfile profile = item->profile();
    QAction *copyNick = copyMenu.addAction(QIcon(":/images/profile.png"), tr("Nick"));
    QAction *copyFullName = 0;
    if (!profile.fullName().isEmpty())
      copyFullName = copyMenu.addAction(QIcon(":/images/profile.png"), tr("Name"));

    QAction *copyUserAgent = copyMenu.addAction(QIcon(":/images/schat16.png"), tr("Client"));
    QAction *copyHost = copyMenu.addAction(QIcon(":/images/computer.png"), tr("Address"));

    menu.addSeparator();
    QAction *nickClickedAction = menu.addAction(tr("Insert nick"));

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


void UserView::resizeEvent(QResizeEvent *event)
{
  updateQuickSearchVisible();
  QListView::resizeEvent(event);
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


void UserView::updateQuickSearchVisible()
{
  if (d->quickUserSearch)
    d->quickUserSearch->setVisible(verticalScrollBar()->isVisible());
}
