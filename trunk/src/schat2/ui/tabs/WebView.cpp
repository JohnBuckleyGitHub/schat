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

#include <QEvent>
#include <QAction>

#include "ui/tabs/WebView.h"
#include "ui/ChatIcons.h"

WebView::WebView(QWidget *parent)
  : QWebView(parent)
{
  pageAction(QWebPage::Cut)->setIcon(SCHAT_ICON(EditCut));
  pageAction(QWebPage::Copy)->setIcon(SCHAT_ICON(EditCopy));
  pageAction(QWebPage::CopyLinkToClipboard)->setIcon(SCHAT_ICON(EditCopy));
  pageAction(QWebPage::Paste)->setIcon(SCHAT_ICON(EditPaste));
  pageAction(QWebPage::SelectAll)->setIcon(SCHAT_ICON(EditSelectAll));
}


void WebView::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWebView::changeEvent(event);
}


void WebView::retranslateUi()
{
  pageAction(QWebPage::Cut)->setText(tr("Cut"));
  pageAction(QWebPage::Copy)->setText(tr("Copy"));
  pageAction(QWebPage::CopyLinkToClipboard)->setText(tr("Copy Link"));
  pageAction(QWebPage::Paste)->setText(tr("Paste"));
  pageAction(QWebPage::SelectAll)->setText(tr("Select All"));
}
