/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include "colorbutton.h"

/*!
 * Конструктор класса ColorWidget.
 */
ColorWidget::ColorWidget(QWidget *parent)
  : QWidget(parent)
{
  m_image = QImage(":/images/color-map.png");
  setMinimumSize(m_image.size());
}


void ColorWidget::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton && m_image.valid(event->pos()))
    emit newColor(QColor(m_image.pixel(event->pos())));

  QWidget::mouseReleaseEvent(event);
}


void ColorWidget::paintEvent(QPaintEvent * /*event*/)
{
  QPainter painter(this);
  painter.drawImage(0, 0, m_image);
}




/*!
 * Конструктор класса ColorButton.
 */
ColorButton::ColorButton(QWidget *parent)
  : QToolButton(parent),
  m_colorWidget(new ColorWidget(this))
{
  setColor(QColor("#000000"));

  QMenu *menu = new QMenu(this);
  QWidgetAction *action = new QWidgetAction(this);
  action->setDefaultWidget(m_colorWidget);
  menu->addAction(action);

  setMenu(menu);
  setPopupMode(QToolButton::MenuButtonPopup);

  connect(m_colorWidget, SIGNAL(newColor(const QColor &)), SLOT(setColor(const QColor &)));
}


ColorButton::~ColorButton()
{
}


/*!
 * Установка нового цвета.
 */
void ColorButton::setColor(const QColor &color)
{
  QPainter painter;
  QPixmap pix(":/images/color.png");
  painter.begin(&pix);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QBrush(color));
  #ifndef SCHAT_WINCE_VGA
  painter.drawEllipse(QPoint(8, 8), 4, 4);
  #else
  painter.drawEllipse(QPoint(16, 16), 8, 8);
  #endif
  painter.end();
  setIcon(pix);
}
