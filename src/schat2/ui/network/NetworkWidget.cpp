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

#include <QDebug>

#include <QComboBox>
#include <QEvent>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>

#include "Account.h"
#include "ChatCore.h"
#include "ChatNotify.h"
#include "client/ChatClient.h"
#include "client/SimpleClient.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/network/AccountButton.h"
#include "ui/network/NetworkWidget.h"

#include "ui/network/SignUpWidget.h"

NetworkWidget::NetworkWidget(QWidget *parent, bool compact)
  : QWidget(parent)
  , m_account(0)
  , m_extra(0)
  , m_manager(ChatCore::networks())
{
  m_combo = new QComboBox(this);
  m_combo->installEventFilter(this);

  createActionsButton();

  if (!compact)
    m_account = new AccountButton(this);

  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(16, 16));
  m_toolBar->addWidget(m_actions);

  if (m_account)
    m_toolBar->addWidget(m_account);

  m_connect = m_toolBar->addAction(QString(), this, SLOT(open()));
  m_toolBar->setStyleSheet(LS("QToolBar { margin:0px; border:0px; }"));

  m_title = new QLabel(this);
  m_title->setVisible(false);

  QGridLayout *mainLay = new QGridLayout();
  mainLay->addWidget(m_combo, 0, 0);
  mainLay->addWidget(m_toolBar, 0, 1);
  mainLay->setColumnStretch(0, 1);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  m_mainLayout = new QVBoxLayout(this);
  m_mainLayout->addItem(mainLay);
  m_mainLayout->addWidget(m_title);
  m_mainLayout->setMargin(0);

  load();

  connect(m_combo, SIGNAL(currentIndexChanged(int)), SLOT(indexChanged(int)));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
  connect(ChatClient::io(), SIGNAL(clientStateChanged(int, int)), SLOT(reload()));

  connectAction();
}


QAction *NetworkWidget::connectAction()
{
  int state = m_manager->isSelectedActive();

  if (state == 1) {
    m_connect->setIcon(SCHAT_ICON(Disconnect));
    m_connect->setText(tr("Disconnect"));
  }
  else if (state == 2) {
    m_connect->setIcon(SCHAT_ICON(Disconnect));
    m_connect->setText(tr("Abort"));
  }
  else {
    m_connect->setIcon(SCHAT_ICON(Connect));
    m_connect->setText(tr("Connect"));
  }

  return m_connect;
}


void NetworkWidget::add(NetworkExtra *extra)
{
  if (m_extra) {
    m_mainLayout->removeWidget(m_extra);
    delete m_extra;
  }

  m_extra = extra;
  setTitle(m_extra->title());
  m_mainLayout->addWidget(m_extra);

  connect(m_extra, SIGNAL(done()), SLOT(doneExtra()));
  QTimer::singleShot(0, m_extra, SLOT(setFocus()));
}


void NetworkWidget::doneExtra()
{
  if (!m_extra)
    return;

  m_mainLayout->removeWidget(m_extra);
  delete m_extra;
  m_extra = 0;
  m_title->setVisible(false);
}


/*!
 * Обработка действия для \p m_connectAction.
 */
void NetworkWidget::open()
{
  QMenu *popup = qobject_cast<QMenu *>(parentWidget());
  if (popup && isVisible())
    popup->close();

  int index = m_combo->currentIndex();
  if (index == -1)
    return;

  if (m_manager->isSelectedActive()) {
    ChatClient::io()->leave();
    return;
  }

  Network item = m_manager->item(m_manager->selected());
  if (item->id() == m_manager->tmpId()) {
    item->setUrl(m_combo->currentText());
    m_combo->setItemText(index, item->url());
  }
  else if (!m_editing.isEmpty() && item->id() == m_editing) {
    item->setUrl(m_combo->currentText());
    m_combo->setItemText(index, item->name());
    m_editing.clear();
    m_combo->setEditable(false);
  }

  ChatClient::open(item->id());
}


bool NetworkWidget::eventFilter(QObject *watched, QEvent *event)
{
  if (watched == m_combo && event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Return) {
      open();
      return true;
    }
  }

  return QWidget::eventFilter(watched, event);
}


void NetworkWidget::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


/*!
 * Добавление нового подключения.
 */
int NetworkWidget::add(const QString &url)
{
  Network item = m_manager->item(m_manager->tmpId());

  int index = m_combo->findData(item->id());
  if (index == -1) {
    if (!url.isEmpty())
      item->setUrl(url);

    m_combo->insertItem(0, SCHAT_ICON(Globe), item->url(), item->id());
    index = 0;
  }

  if (m_combo->currentIndex() != index)
    m_combo->setCurrentIndex(index);

  m_combo->setEditable(true);
  QTimer::singleShot(0, m_combo, SLOT(setFocus()));
  return index;
}


/*!
 * Редактирование адреса сети.
 */
void NetworkWidget::edit()
{
  int index = m_combo->currentIndex();
  if (index == -1)
    return;

  Network item = m_manager->item(m_combo->itemData(index).toByteArray());
  if (!item->isValid())
    return;

  doneExtra();

  m_editing = item->id();
  m_combo->setItemText(index, item->url());
  m_combo->setEditable(true);
  m_combo->setFocus();
}


/*!
 * Обработка изменения текущего индекса.
 */
void NetworkWidget::indexChanged(int index)
{
  QByteArray id = m_combo->itemData(index).toByteArray();
  m_combo->setEditable(id == m_manager->tmpId());

  if (!m_editing.isEmpty()) {
    int index = m_combo->findData(m_editing);
    if (index != -1) {
      Network item = m_manager->item(m_editing);
      m_combo->setItemText(index, item->name());
    }

    m_editing.clear();
  }

  m_manager->setSelected(id);
  doneExtra();
  reload();
}


void NetworkWidget::notify(const Notify &notify)
{
  if (notify.type() == Notify::NetworkChanged) {
    int index = m_combo->findData(m_manager->tmpId());
    if (index != -1)
      m_combo->removeItem(index);

    QByteArray id = notify.data().toByteArray();
    Network item = m_manager->item(id);

    if (!item->isValid())
      return;

    index = m_combo->findData(item->id());
    if (index != -1)
      m_combo->removeItem(index);

    m_combo->insertItem(0, SCHAT_ICON(Globe), item->name(), item->id());
    m_combo->setCurrentIndex(0);
  }
  else if (notify.type() == Notify::NetworkSelected) {
    updateIndex();
  }
  else if (notify.type() == Notify::ServerRenamed) {
    int index = m_combo->findData(ChatClient::serverId());
    if (index != -1)
      m_combo->setItemText(index, ChatClient::serverName());
  }
}


void NetworkWidget::reload()
{
  connectAction();

  if (!m_account)
    return;

  m_account->setEnabled(ChatClient::state() == ChatClient::Online);
  if (m_account->isEnabled() && m_manager->selected() != ChatClient::serverId())
    m_account->setEnabled(false);
}


/*!
 * Удаление сервера.
 */
void NetworkWidget::remove()
{
  int index = m_combo->currentIndex();
  if (index == -1)
    return;

  QByteArray id = m_combo->itemData(index).toByteArray();
  m_manager->removeItem(id);
  m_combo->removeItem(index);

  if (!m_combo->count())
    add();
}


void NetworkWidget::showMenu()
{
  int index = m_combo->currentIndex();
  if (index == -1 || m_combo->itemData(index).type() != QVariant::ByteArray || m_combo->isEditable())
    m_edit->setVisible(false);
  else
    m_edit->setVisible(true);
}


void NetworkWidget::createActionsButton()
{
  m_menu = new QMenu(this);

  m_menu->addSeparator();
  m_edit = m_menu->addAction(SCHAT_ICON(TopicEdit), tr("Edit"), this, SLOT(edit()));
  m_menu->addSeparator();
  m_add = m_menu->addAction(SCHAT_ICON(Add), tr("Add"), this, SLOT(add()));
  m_remove = m_menu->addAction(SCHAT_ICON(Remove), tr("Remove"), this, SLOT(remove()));

  m_actions = new QToolButton(this);
  m_actions->setIcon(SCHAT_ICON(Gear));
  m_actions->setMenu(m_menu);
  m_actions->setPopupMode(QToolButton::InstantPopup);
  m_actions->setToolTip(tr("Actions"));

  connect(m_menu, SIGNAL(aboutToShow()), SLOT(showMenu()));
}


/*!
 * Загрузка списка серверов в виджет.
 */
void NetworkWidget::load()
{
  QList<Network> items = m_manager->items();

  for (int i = 0; i < items.size(); ++i) {
    m_combo->addItem(SCHAT_ICON(Globe), items.at(i)->name(), items.at(i)->id());
  }

  if (m_combo->count() == 0) {
    add("schat://schat.me");
    m_manager->setSelected(m_manager->tmpId());
  }

  updateIndex();
}


void NetworkWidget::retranslateUi()
{
  m_edit->setText(tr("Edit"));
  m_add->setText(tr("Add"));
  m_remove->setText(tr("Remove"));
  m_actions->setToolTip(tr("Actions"));

  if (m_extra)
    setTitle(m_extra->title());
}


void NetworkWidget::setTitle(const QString &title)
{
  if (!title.isEmpty()) {
    m_title->setText(LS("<b>") + title + LS("</b>"));
    m_title->setVisible(true);
  }
  else
    m_title->setVisible(false);
}


/*!
 * Обновление выбора текущей сети.
 * Необходимо для синхронизации выбора сети во всех виджетах.
 */
void NetworkWidget::updateIndex()
{
  Network item = m_manager->item(m_manager->selected());

  int index = m_combo->findData(item->id());
  if (index == -1) {
    if (m_manager->tmpId() == item->id())
      index = add(QString());
    else
      return;
  }

  if (m_combo->currentIndex() != index)
    m_combo->setCurrentIndex(index);
}
