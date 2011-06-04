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

#include <QFormLayout>
#include <QLineEdit>

#include "ChatCore.h"
#include "net/SimpleClient.h"
#include "ui/NetworkWidget.h"
#include "ui/fields/NickEdit.h"
#include "ui/tabs/WelcomeTab.h"
#include "User.h"

WelcomeTab::WelcomeTab(SimpleClient *client, TabWidget *parent)
  : AbstractTab(QByteArray(), WelcomeType, parent)
  , m_client(client)
{
  m_networks = new NetworkWidget(this);
  m_nickEdit = new NickEdit(this);

  QFormLayout *mainLay = new QFormLayout(this);
  mainLay->addRow(tr("Network:"), m_networks);
  mainLay->addRow(tr("Nick:"), m_nickEdit);

  m_icon = SCHAT_ICON(SmallLogoIcon);
}
