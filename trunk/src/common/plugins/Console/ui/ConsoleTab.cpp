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

#include <QVBoxLayout>
#include <QFile>
#include <QApplication>

#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/ConsoleTab.h"
#include "ui/ConsoleView.h"

ConsoleTab::ConsoleTab(TabWidget *parent)
  : AbstractTab("console", LS("console"), parent)
{
  QString url = QApplication::applicationDirPath() + "/styles/Console/index.html";
  if (QFile::exists(url))
    url = QUrl::fromLocalFile(url).toString();
  else
    url = "qrc:/html/Console/index.html";

  m_view = new ConsoleView(this);
  m_view->setUrl(url);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_view);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  setIcon(SCHAT_ICON(Globe));
  retranslateUi();
}


void ConsoleTab::retranslateUi()
{
  setText(tr("Console"));
}
