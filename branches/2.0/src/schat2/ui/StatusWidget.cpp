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

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>

#include "ChatCore.h"
#include "ui/StatusMenu.h"
#include "ui/StatusWidget.h"

StatusWidget::StatusWidget(QWidget *parent)
  : QWidget(parent)
  , m_menu(ChatCore::i()->statusMenu())
{
  m_icon = new QLabel(this);
  m_label = new QLabel(this);

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->setMargin(0);
  mainLay->setSpacing(3);
  mainLay->addWidget(m_icon);
  mainLay->addWidget(m_label);
  mainLay->addStretch();

  update();

  connect(m_menu, SIGNAL(updated()), SLOT(update()));
}


/*!
 * Обработка нажатий кнопок мыши для показа меню.
 */
void StatusWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
    m_menu->exec(event->globalPos());
  }
  else {
    QWidget::mouseReleaseEvent(event);
  }
}


void StatusWidget::update()
{
  m_icon->setPixmap(m_menu->icon().pixmap(16));
  m_label->setText(m_menu->title());
  adjustSize();
}