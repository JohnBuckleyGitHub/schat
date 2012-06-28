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
#include <QTimer>

#include "plugins/PluginsView.h"
#include "sglobal.h"

PluginsView::PluginsView(QWidget *parent)
  : QWebView(parent)
{
  setAcceptDrops(false);
  page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

  QTimer::singleShot(0, this, SLOT(boot()));
}


void PluginsView::boot()
{
  QString file = QApplication::applicationDirPath() + LS("/styles/test/html/Plugins.html");
  if (QFile::exists(file))
    file = QUrl::fromLocalFile(file).toString();
  else
    file = LS("qrc:/html/Plugins.html");

  setUrl(QUrl(file));
}
