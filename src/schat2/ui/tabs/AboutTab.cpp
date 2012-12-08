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

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QLibraryInfo>
#include <QVBoxLayout>

#include <QWebView>
#include <QApplication>
#include <QWebFrame>

# if QT_VERSION >= 0x050000
# include <QWebPage>
#else
# include <qwebkitversion.h>
#endif

#include "ChatCore.h"
#include "ChatSettings.h"
#include "Path.h"
#include "sglobal.h"
#include "Tr.h"
#include "ui/ChatIcons.h"
#include "ui/tabs/AboutTab.h"
#include "version.h"
#include "WebBridge.h"

class AboutTr : public Tr
{
  Q_DECLARE_TR_FUNCTIONS(AboutTr)

public:
  AboutTr() : Tr() {}

protected:
  QString valueImpl(const QString &key) const
  {
    if (key == LS("paths"))                    return tr("Paths");
    else if (key == LS("third_parties"))       return tr("Third parties");
    else if (key == LS("gnu_gpl"))             return tr("This software is released under the terms of the <a href=\"http://www.gnu.org/licenses/gpl-3.0-standalone.html\">GNU General Public License</a> version 3.");
    else if (key == LS("all_rights_reserved")) return tr("All rights reserved.");
    else if (key == LS("preferences"))         return tr("Preferences");
    return QString();
  }
};


AboutTab::AboutTab(TabWidget *parent)
  : AbstractTab("about", LS("about"), parent)
{
  m_tr = new AboutTr();

  m_view = new QWebView(this);
  m_view->setAcceptDrops(false);
  m_view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_view);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  connect(m_view, SIGNAL(linkClicked(const QUrl &)), SLOT(linkClicked(const QUrl &)));

  QString file = QApplication::applicationDirPath() + LS("/styles/test/html/about.html");
  if (QFile::exists(file))
    file = QUrl::fromLocalFile(file).toString();
  else
    file = LS("qrc:/html/about.html");

  m_view->setUrl(QUrl(file));
  connect(m_view->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(populateJavaScriptWindowObject()));

  setIcon(SCHAT_ICON(SmallLogo));
  retranslateUi();
}


AboutTab::~AboutTab()
{
  delete m_tr;
}


QString AboutTab::path(const QString &type) const
{
  if (type == LS("preferences"))
    return fileUrl(Path::config());

  return QString();
}


/*!
 * Получение версии из JavaScript.
 */
QString AboutTab::version(const QString &type) const
{
  if (type == LS("app"))
    return SCHAT_VERSION;

  if (type == LS("qt"))
    return qVersion() + (QSysInfo::WordSize == 32 ? tr(" (32 bit)") : tr(" (64 bit)"));

  if (type == LS("webkit"))
    return qWebKitVersion();

  return QString();
}


void AboutTab::linkClicked(const QUrl &url)
{
  QDesktopServices::openUrl(url);
}


void AboutTab::populateJavaScriptWindowObject()
{
  m_view->page()->mainFrame()->addToJavaScriptWindowObject(LS("SimpleChat"), WebBridge::i());
  m_view->page()->mainFrame()->addToJavaScriptWindowObject(LS("About"), this);
}


QString AboutTab::fileUrl(const QString &fileName) const
{
  QString out = LS("<a href=\"");
  out += QUrl::fromLocalFile(fileName).toEncoded();
  out += LS("\">");
  out += QDir::toNativeSeparators(fileName);
  out += LS("</a>");

  return out;
}


void AboutTab::retranslateUi()
{
  setText(tr("About"));
}
