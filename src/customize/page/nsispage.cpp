/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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

#include "nsispage.h"
#include "wizardsettings.h"

/*!
 * \brief Конструктор класса NsisPage.
 */
NsisPage::NsisPage(QWidget *parent)
  : QWizardPage(parent)
{
  m_settings = settings;

  setTitle(tr("Заглушко"));
  setSubTitle(tr("Заглушко"));
  setCommitPage(true);
}
