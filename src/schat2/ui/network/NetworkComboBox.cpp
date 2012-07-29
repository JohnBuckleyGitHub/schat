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

#include <QTimer>

#include "ChatCore.h"
#include "ChatNotify.h"
#include "client/ChatClient.h"
#include "client/SimpleClient.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/network/NetworkComboBox.h"
#include "ui/network/NetworkWidget.h"

NetworkComboBox::NetworkComboBox(NetworkWidget *parent)
  : QComboBox(parent)
  , m_network(parent)
{
  m_tmpId = SimpleID::make("", SimpleID::ServerId);

  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
}


/*!
 * Загрузка списка серверов в виджет.
 */
void NetworkComboBox::load()
{
  addItem(SCHAT_ICON(Add), tr("Add"), m_tmpId);

  QList<Network> items = ChatCore::networks()->items();
  for (int i = 0; i < items.size(); ++i) {
    addItem(SCHAT_ICON(Globe), items.at(i)->name(), items.at(i)->id());
  }

  if (count() == 1) {
    ChatCore::networks()->setSelected(m_tmpId);
    setItemText(0, LS("schat://schat.me"));
    setEditable(true);
  }

  connect(this, SIGNAL(currentIndexChanged(int)), SLOT(indexChanged(int)));
  updateIndex();
}


void NetworkComboBox::open()
{
  Network item = ChatCore::networks()->item(ChatCore::networks()->selected());
  if (item->id() == m_tmpId) {
    item->setUrl(currentText());
    setItemText(currentIndex(), item->url());
  }
  else if (!m_editing.isEmpty() && item->id() == m_editing) {
    item->setUrl(currentText());
    setItemText(currentIndex(), item->name());
    m_editing.clear();
    setEditable(false);
  }

  ChatClient::open(item->id());
}


/*!
 * Добавление пользователем нового подключения.
 */
void NetworkComboBox::add(const QString &url)
{
  if (currentIndex() != 0) {
    setCurrentIndex(0);
    ChatCore::networks()->setSelected(m_tmpId);
  }

  setItemText(0, url);
  setEditable(true);
  QTimer::singleShot(0, this, SLOT(setFocus()));
}


/*!
 * Редактирование адреса сети.
 */
void NetworkComboBox::edit()
{
  int index = currentIndex();
  if (index < 1)
    return;

  Network item = ChatCore::networks()->item(itemData(index).toByteArray());
  if (!item->isValid())
    return;

//  doneExtra();

  m_editing = item->id();
  setItemText(index, item->url());
  setEditable(true);
  QTimer::singleShot(0, this, SLOT(setFocus()));
}


/*!
 * Удаление сервера.
 */
void NetworkComboBox::remove()
{
  int index = currentIndex();
  if (index == -1)
    return;

  if (index == 0) {
    add();
    return;
  }

  QByteArray id = itemData(index).toByteArray();
  ChatCore::networks()->removeItem(id);
  removeItem(index);
}


/*!
 * Обработка изменения выбранного индекса.
 */
void NetworkComboBox::indexChanged(int index)
{
  if (index == -1)
    return;

  if (!m_editing.isEmpty()) {
    int index = findData(m_editing);
    if (index != -1) {
      Network item = ChatCore::networks()->item(m_editing);
      setItemText(index, item->name());
    }

    m_editing.clear();
  }

  ChatCore::networks()->setSelected(itemData(index).toByteArray());

  if (index == 0) {
    add();
    return;
  }

  setEditable(false);
  setItemText(0, tr("Add"));
}


void NetworkComboBox::notify(const Notify &notify)
{
  if (notify.type() == Notify::NetworkSelected) {
    updateIndex();
  }
  else if (notify.type() == Notify::NetworkChanged) {
    QByteArray id = notify.data().toByteArray();
    Network item = ChatCore::networks()->item(id);

    if (!item->isValid())
      return;

    setCurrentIndex(-1);

    int index = findData(item->id());
    if (index != -1)
      removeItem(index);

    insertItem(1, SCHAT_ICON(Globe), item->name(), item->id());
    setCurrentIndex(1);
  }
  else if (notify.type() == Notify::ServerRenamed) {
    int index = findData(ChatClient::serverId());
    if (index != -1)
      setItemText(index, ChatClient::serverName());
  }
}


/*!
 * Обновление выбора текущей сети.
 * Необходимо для синхронизации выбора сети во всех виджетах.
 */
void NetworkComboBox::updateIndex()
{
  if (itemData(currentIndex()) == ChatCore::networks()->selected())
    return;

  int index = findData(ChatCore::networks()->selected());
  if (index == -1)
    return;

  if (currentIndex() != index)
    setCurrentIndex(index);
}
