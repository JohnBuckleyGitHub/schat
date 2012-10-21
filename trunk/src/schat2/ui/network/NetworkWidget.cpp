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
#include "client/ClientFeeds.h"
#include "client/SimpleClient.h"
#include "net/packets/auth.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/network/NetworkComboBox.h"
#include "ui/network/NetworkExtra.h"
#include "ui/network/NetworkWidget.h"

NetworkWidget::NetworkWidget(QWidget *parent, int layout)
  : QWidget(parent)
  , m_layout(layout)
  , m_extra(0)
  , m_manager(ChatCore::networks())
{
  m_combo = new NetworkComboBox(this);
  m_combo->installEventFilter(this);

  createActionsButton();

  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(16, 16));
  m_toolBar->addWidget(m_actions);

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

  m_combo->load();

  connect(m_combo, SIGNAL(currentIndexChanged(int)), SLOT(indexChanged(int)));
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


/*!
 * Добавление дополнительного виджета.
 */
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


/*!
 * Удаление дополнительного виджета.
 */
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

  m_combo->open();
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
 * Обработка изменения текущего индекса.
 */
void NetworkWidget::indexChanged(int index)
{
  if (index == -1)
    return;

  doneExtra();
  reload();
}


void NetworkWidget::menuTriggered(QAction *action)
{
  if (action == m_computers) {
    ChatNotify::start(Notify::OpenInfo, ChatClient::serverId());
  }
  else if (action == m_signOut) {
    signOut();
  }
}


void NetworkWidget::reload()
{
  connectAction();
}


/*!
 * Показ меню.
 */
void NetworkWidget::showMenu()
{
  int index = m_combo->currentIndex();
  if (index == -1 || m_combo->itemData(index).type() != QVariant::ByteArray || m_combo->isEditable())
    m_edit->setVisible(false);
  else
    m_edit->setVisible(true);

  bool computers = ChatClient::state() == ChatClient::Online && !m_combo->isEditable();
  m_computers->setVisible(computers);
  m_signOut->setVisible(computers);
}


void NetworkWidget::createActionsButton()
{
  m_menu = new QMenu(this);

  m_menu->addSeparator();
  m_edit = m_menu->addAction(SCHAT_ICON(TopicEdit), tr("Edit"), m_combo, SLOT(edit()));
  m_menu->addSeparator();
  m_add = m_menu->addAction(SCHAT_ICON(Add), tr("Add"), m_combo, SLOT(add()));
  m_remove = m_menu->addAction(SCHAT_ICON(Remove), tr("Remove"), m_combo, SLOT(remove()));
  m_menu->addSeparator();
  m_signOut = m_menu->addAction(SCHAT_ICON(SignOut), tr("Sign out"));
  m_computers = m_menu->addAction(SCHAT_ICON(Computer), tr("My Computers"));

  m_actions = new QToolButton(this);
  m_actions->setIcon(SCHAT_ICON(Gear));
  m_actions->setMenu(m_menu);
  m_actions->setPopupMode(QToolButton::InstantPopup);
  m_actions->setToolTip(tr("Actions"));

  connect(m_menu, SIGNAL(aboutToShow()), SLOT(showMenu()));
  connect(m_menu, SIGNAL(triggered(QAction *)), SLOT(menuTriggered(QAction *)));
}


void NetworkWidget::retranslateUi()
{
  connectAction();

  m_edit->setText(tr("Edit"));
  m_add->setText(tr("Add"));
  m_remove->setText(tr("Remove"));
  m_signOut->setText(tr("Sign out"));
  m_computers->setText(tr("My Computers"));
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


void NetworkWidget::signOut()
{
  if (ChatClient::state() != ChatClient::Online)
    return;

  QVariantMap json;
  json[LS("id")] = SimpleID::encode(ChatClient::io()->json().value(LS("hostId")).toByteArray());

  /// \bug unlink больше не работает
//  ChatClient::feeds()->query(LS("hosts"), LS("unlink"), json);
  ChatClient::io()->setAuthType(AuthRequest::Discovery);
}
