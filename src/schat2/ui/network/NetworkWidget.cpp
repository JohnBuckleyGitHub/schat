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

#include <QDebug>

#include <QComboBox>
#include <QEvent>
#include <QGridLayout>
#include <QKeyEvent>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>

#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "ui/network/NetworkWidget.h"

NetworkWidget::NetworkWidget(QWidget *parent)
  : QWidget(parent)
  , m_manager(ChatCore::i()->networks())
  , m_client(ChatCore::i()->client())
{
  m_combo = new QComboBox(this);
  m_combo->installEventFilter(this);

  m_menu = new QMenu(this);
  connect(m_menu, SIGNAL(aboutToShow()), SLOT(showMenu()));
  m_connectAction = m_menu->addAction("", this, SLOT(open()));
  m_menu->addSeparator();
  m_edit = m_menu->addAction(SCHAT_ICON(TopicEdit), tr("Edit"), this, SLOT(edit()));
  m_menu->addSeparator();
  m_addAction = m_menu->addAction(SCHAT_ICON(AddIcon), tr("Add"), this, SLOT(add()));
  m_removeAction = m_menu->addAction(SCHAT_ICON(RemoveIcon), tr("Remove"), this, SLOT(remove()));

  m_config = new QToolButton(this);
  m_config->setIcon(SCHAT_ICON(GearIcon));
  m_config->setMenu(m_menu);
  m_config->setPopupMode(QToolButton::InstantPopup);

  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(16, 16));
  m_toolBar->addWidget(m_config);
  m_toolBar->setStyleSheet(QLatin1String("QToolBar { margin:0px; border:0px; }"));

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(m_combo, 0, 0);
  mainLay->addWidget(m_toolBar, 0, 1);
  mainLay->setColumnStretch(0, 1);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  load();

  connect(m_combo, SIGNAL(currentIndexChanged(int)), SLOT(indexChanged(int)));
  connect(ChatCore::i(), SIGNAL(notify(int, const QVariant &)), SLOT(notify(int, const QVariant &)));
}


QAction *NetworkWidget::connectAction()
{
  int state = m_manager->isSelectedActive();

  if (state == 1) {
    m_connectAction->setIcon(SCHAT_ICON(DisconnectIcon));
    m_connectAction->setText(tr("Disconnect"));
  }
  else if (state == 2) {
    m_connectAction->setIcon(SCHAT_ICON(DisconnectIcon));
    m_connectAction->setText(tr("Abort"));
  }
  else {
    m_connectAction->setIcon(SCHAT_ICON(ConnectIcon));
    m_connectAction->setText(tr("Connect"));
  }

  return m_connectAction;
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
    m_client->leave();
    return;
  }

  NetworkItem& item = m_manager->edit(m_manager->selected());
  if (item.id() == m_manager->tmpId()) {
    item.setUrl(m_combo->currentText());
    m_combo->setItemText(index, item.url());
  }

  m_manager->open(item.id());
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
  NetworkItem &item = m_manager->edit(m_manager->tmpId());

  int index = m_combo->findData(item.id());
  if (index == -1) {
    if (!url.isEmpty()) {
      item.setUrl(url);
      item.setAccount(QString());
      item.setPassword(QString());
    }

    m_combo->insertItem(0, SCHAT_ICON(GlobeIcon), item.url(), item.id());
    index = 0;
  }

  if (m_combo->currentIndex() != index)
    m_combo->setCurrentIndex(index);

  m_combo->setFocus();
  m_combo->setEditable(true);
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

  QByteArray id = m_combo->itemData(index).toByteArray();
  if (SimpleID::typeOf(id) != SimpleID::ServerId)
    return;

  NetworkItem item = m_manager->item(id);
  if (!item.isValid())
    return;

  m_combo->setItemText(index, item.url());
  m_combo->setEditable(true);
  m_combo->setFocus();
}


/*!
 * Обработка изменения индекса, запрещается редактирование сохранённых сетей.
 */
void NetworkWidget::indexChanged(int index)
{
  QByteArray id = m_combo->itemData(index).toByteArray();
  m_combo->setEditable(id == m_manager->tmpId());

  m_manager->setSelected(id);
}


void NetworkWidget::notify(int notice, const QVariant &data)
{
  if (notice == ChatCore::NetworkChangedNotice) {
    int index = m_combo->findData(m_manager->tmpId());
    if (index != -1)
      m_combo->removeItem(index);

    QByteArray id = data.toByteArray();
    NetworkItem item = m_manager->item(id);

    if (!item.isValid())
      return;

    m_combo->insertItem(0, SCHAT_ICON(GlobeIcon), item.name(), item.id());
    m_combo->setCurrentIndex(0);
  }
  else if (notice == ChatCore::NetworkSelectedNotice) {
    updateIndex();
  }
}


/*!
 * Удаление сервера.
 */
void NetworkWidget::remove()
{
  int index = m_combo->currentIndex();
  qDebug() << "@@@@@@" << index;
  if (index == -1)
    return;

  QByteArray id = m_combo->itemData(index).toByteArray();
  m_manager->removeItem(id);
  m_combo->removeItem(index);

  qDebug() << m_combo->count();
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

  QByteArray id = m_combo->itemData(index).toByteArray();
  connectAction();
}


/*!
 * Загрузка списка серверов в виджет.
 */
void NetworkWidget::load()
{
  QList<NetworkItem> items = m_manager->items();

  for (int i = 0; i < items.size(); ++i) {
    m_combo->addItem(SCHAT_ICON(GlobeIcon), items.at(i).name(), items.at(i).id());
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
  m_addAction->setText(tr("Add"));
  m_removeAction->setText(tr("Remove"));
}


/*!
 * Обновление выбора текущей сети.
 * Необходимо для синхронизации выбора сети во всех виджетах.
 */
void NetworkWidget::updateIndex()
{
  NetworkItem item = m_manager->item(m_manager->selected());

  int index = m_combo->findData(item.id());
  if (index == -1) {
    if (m_manager->tmpId() == item.id())
      index = add(QString());
    else
      return;
  }

  if (m_combo->currentIndex() != index)
    m_combo->setCurrentIndex(index);
}
