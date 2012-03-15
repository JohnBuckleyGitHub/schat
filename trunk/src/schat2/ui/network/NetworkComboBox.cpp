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

#include <ui/ChatIcons.h>
#include "ui/network/NetworkComboBox.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "ChatCore.h"
#include "NetworkManager.h"

NetworkComboBox::NetworkComboBox(QWidget *parent)
  : QComboBox(parent)
{
  m_tmpId = SimpleID::make("", SimpleID::ServerId);
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

//  updateIndex();
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
    setItemText(0, LS("schat://"));
    return;
  }

  QByteArray id = itemData(index).toByteArray();
  ChatCore::networks()->removeItem(id);
  removeItem(index);
}
