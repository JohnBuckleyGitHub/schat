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

#include <QFileDialog>
#include <QMenu>
#include <QWidgetAction>

#include "ChatCore.h"
#include "sglobal.h"
#include "ShareButton.h"
#include "SharePlugin_p.h"
#include "ShareWidget.h"
#include "ui/TabWidget.h"

ShareButton::ShareButton(Share *share, QWidget *parent)
  : QToolButton(parent)
  , m_share(share)
{
  m_menu = new QMenu(this);

  setAutoRaise(true);
  setPopupMode(QToolButton::InstantPopup);
  setMenu(m_menu);
  setStyleSheet(LS("QToolButton::menu-indicator {image:none}"));

  retranslateUi();

  connect(m_menu, SIGNAL(aboutToHide()), SLOT(menuAboutToHide()));
  connect(m_menu, SIGNAL(aboutToShow()), SLOT(menuAboutToShow()));
}


void ShareButton::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QToolButton::changeEvent(event);
}


void ShareButton::addFromDisk()
{
  const QStringList files = QFileDialog::getOpenFileNames(TabWidget::i(), tr("Open images"), QDir::currentPath(), tr("Images (*.jpg *.jpeg *.png *.gif *.JPG *.PNG)"));
  if (files.isEmpty())
    return;

  QList<QUrl> urls;
  urls.reserve(files.size());

  foreach (const QString &name, files)
    urls.append(QUrl::fromLocalFile(name));

  ChatId id(ChatCore::currentId());
  m_share->upload(id, urls, true);
}


void ShareButton::menuAboutToHide()
{
  QList<QAction *> actions = m_menu->actions();
  foreach (QAction *action, actions)
    action->deleteLater();

  m_menu->clear();
}


void ShareButton::menuAboutToShow()
{
  ShareWidget *widget = new ShareWidget(this);
  connect(widget, SIGNAL(addFromDisk()), SLOT(addFromDisk()));

  QWidgetAction *action = new QWidgetAction(this);
  action->setDefaultWidget(widget);
  m_menu->addAction(action);
}


void ShareButton::retranslateUi()
{
  setToolTip(tr("Share images"));
}


ShareAction::ShareAction(Share *share)
  : ToolBarActionCreator(1150, LS("share"), WidgetType | AutoShow | AutoDelete)
  , m_icon(LS(":/images/Share/button.png"))
  , m_share(share)
{
}


QWidget* ShareAction::createWidget(QWidget *parent) const
{
  ShareButton *button = new ShareButton(m_share, parent);
  button->setIcon(m_icon);
  return button;
}


QString ShareAction::title() const
{
  return tr("Share images");
}
