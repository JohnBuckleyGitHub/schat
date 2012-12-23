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

#include <QApplication>
#include <QFile>
#include <QVBoxLayout>

#include "sglobal.h"
#include "ui/ChannelsView.h"
#include "ui/ListTab.h"

ListTab::ListTab(TabWidget *parent)
  : AbstractTab("list", LS("list"), parent)
{
  QString url = QApplication::applicationDirPath() + LS("/styles/Channels/index.html");
  if (QFile::exists(url))
    url = QUrl::fromLocalFile(url).toString();
  else
    url = LS("qrc:/html/Channels/index.html");

  m_view = new ChannelsView(this);
  m_view->setUrl(url);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_view);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  setIcon(QIcon(LS(":/images/Channels/list.png")));
  retranslateUi();
}


void ListTab::retranslateUi()
{
  setText(tr("Channels"));
}
