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
 * Конструктор класса ColorButton.
 */
ColorButton::ColorButton(QWidget *parent)
  : QToolButton(parent)
{
  setColor(QColor("#000000"));
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
