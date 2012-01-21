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

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QLibraryInfo>
#include <QVBoxLayout>
#include <qwebkitversion.h>
#include <QWebView>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "FileLocations.h"
#include "ui/tabs/AboutTab.h"
#include "version.h"
#include "ui/ChatIcons.h"

AboutTab::AboutTab(TabWidget *parent)
  : AbstractTab(QByteArray(), AboutType, parent)
{
  m_view = new QWebView(this);
  m_view->setAcceptDrops(false);
  m_view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_view);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  connect(m_view, SIGNAL(linkClicked(const QUrl &)), SLOT(linkClicked(const QUrl &)));

  setIcon(SCHAT_ICON(SmallLogo));
  retranslateUi();
}


void AboutTab::linkClicked(const QUrl &url)
{
  QDesktopServices::openUrl(url);
}


QString AboutTab::fileUrl(const QString &fileName) const
{
  QString out = "<a href=\"";
  out += QUrl::fromLocalFile(fileName).toEncoded();
  out += "\">";
  out += QDir::toNativeSeparators(fileName);
  out += "</a>";

  return out;
}


void AboutTab::retranslateUi()
{
  setText(tr("About"));

  QFile file(":/html/about.html");
  if (file.open(QIODevice::ReadOnly)) {
    QString page = file.readAll();
    page.replace("%version%", SCHAT_VERSION);
    page.replace("%copyright%", QString(SCHAT_COPYRIGHT) + ". " + tr("All rights reserved."));
    page.replace("%license%", tr("License"));
    page.replace("%site%", tr("Site"));
    page.replace("%paths%", tr("Paths"));
    page.replace("%3rdparty%", tr("Third parties"));
    page.replace("%preferences%", tr("Preferences"));
    page.replace("%preferences-file%", fileUrl(ChatCore::i()->locations()->path(FileLocations::ConfigFile)));

    page.replace("%edition%", QLibraryInfo::licensee());
    page.replace("%qt-version%", qVersion() + (QSysInfo::WordSize == 32 ? tr(" (32 bit)") : tr(" (64 bit)")));
    page.replace("%webkit-version%", qWebKitVersion());
    m_view->setHtml(page);
  }
}
