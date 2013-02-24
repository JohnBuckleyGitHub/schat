/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include <QVBoxLayout>

#include "privatetab.h"
#include "settings.h"

/*!
 * \brief Конструктор класса PrivateTab.
 */
PrivateTab::PrivateTab(const QIcon &icon, QWidget *parent)
  : AbstractTab(Private, icon, parent)
{
  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_view);
  mainLay->setMargin(0);

  m_view->log(SimpleSettings->getBool("LogPrivate"));
  connect(SimpleSettings, SIGNAL(changed(int)), SLOT(notify(int)));
}


void PrivateTab::notify(int code)
{
  if (code == Settings::MiscSettingsChanged)
    m_view->log(SimpleSettings->getBool("LogPrivate"));
}
