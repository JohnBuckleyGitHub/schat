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

#include <QAction>
#include <QEvent>

#include "ui/tabs/AbstractTab.h"
#include "ui/TabWidget.h"

AbstractTab::AbstractTab(const QByteArray &id, const QString &type, TabWidget *parent)
  : QWidget(parent)
  , m_online(false)
  , m_options(NoOptions)
  , m_tabs(parent)
  , m_id(id)
  , m_type(type)
{
  m_action = new QAction(this);
  m_action->setCheckable(true);

  connect(m_action, SIGNAL(triggered(bool)), SIGNAL(actionTriggered(bool)));
}


void AbstractTab::pin()
{
  setText(QString());
}


void AbstractTab::setOnline(bool online)
{
  m_online = online;

  int index = m_tabs->indexOf(this);
  if (index == -1)
    return;

  if (m_online) {
    m_tabs->setTabIcon(index, m_icon);
    m_action->setIcon(m_icon);
  }
  else {
    QIcon offline = QIcon(m_icon.pixmap(16, 16, QIcon::Disabled));
    m_tabs->setTabIcon(index, offline);
    m_action->setIcon(offline);
  }
}


/*!
 * Установка иконки вкладки.
 */
void AbstractTab::setIcon(const QIcon &icon)
{
  m_icon = icon;
  m_action->setIcon(icon);

  int index = m_tabs->indexOf(this);
  if (index != -1)
    m_tabs->setTabIcon(index, m_icon);
}


/*!
 * Установка текста вкладки.
 */
void AbstractTab::setText(const QString &text)
{
  m_text = text;
  m_action->setText(text);

  int index = m_tabs->indexOf(this);
  if (index != -1)
    m_tabs->setTabText(index, text);
}


void AbstractTab::retranslateUi()
{
}


void AbstractTab::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}
