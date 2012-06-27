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

#include <QPainter>

#include "sglobal.h"
#include "ui/AlertsPixmap.h"

/*!
 * Определение ширины текста.
 */
int AlertsPixmap::widthHint(int count)
{
  if (count < 10) {
    if (count == 7)
      return 3;

    return 4;
  }

  if (count < 100)
    return width(count / 10) + 1 + width(count % 10);

  return width(count / 100) + 1 + width(count / 10 % 10) + 1 + width(count % 10);
}


QIcon AlertsPixmap::icon(const QIcon &icon, int count)
{
  QPixmap pixmap = icon.pixmap(16, 16);
  QPixmap overlay = draw(count);

  QPainter painter(&pixmap);
  painter.drawPixmap(16 - overlay.width(), 7, overlay);
  painter.end();

  return QIcon(pixmap);
}


/*!
 * Отрисовка картинки.
 */
QPixmap AlertsPixmap::draw(int count)
{
  if (count > 999)
    return draw(999);

  bool wide = false;
  int width = widthHint(count);
  if (width > 10) {
    wide = true;
    width += 2;
    if (width > 16)
      width = 16;
  }
  else
    width += 6;

  QPixmap pixmap(width, 9);
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QBrush(QColor(204, 60, 41)));

  painter.setRenderHint(QPainter::Antialiasing);
  if (wide)
    painter.drawRoundedRect(pixmap.rect(), 2, 2);
  else
    painter.drawRoundedRect(pixmap.rect(), 4.5, 4.5);

  painter.setRenderHint(QPainter::Antialiasing, false);
  int offset = wide ? 1 : 3;
  if (count < 10) {
    if (count == 1)
      offset++;

    drawNumber(offset, &painter, count);
  }
  else if (count < 100) {
    drawNumber(offset, &painter, count / 10);
    drawNumber(offset, &painter, count % 10);
  }
  else {
    drawNumber(offset, &painter, count / 100);
    drawNumber(offset, &painter, count / 10 % 10);
    drawNumber(offset, &painter, count % 10);
  }

  painter.end();

  return pixmap;
}


/*!
 * Возвращает позицию размещения изображения цифры.
 */
int AlertsPixmap::pos(int number)
{
  if (number > 9)
    number = 9;

  if (number == 0)
    return number;

  if (number == 1)
    return 4;

  if (number > 1 && number < 8)
    return 4 * number - 2;

  return 4 * number - 3;
}


/*!
 * Возвращает ширину цифры в пикселях.
 */
int AlertsPixmap::width(int number)
{
  if (number == 1)
    return 2;

  if (number == 7)
    return 3;

  return 4;
}


void AlertsPixmap::drawNumber(int &offset, QPainter *painter, int number)
{
  if (number > 9)
    number = 9;

  painter->drawPixmap(offset, 2, QPixmap(LS(":/images/numbers.png")), pos(number), 0, width(number), 5);
  offset += width(number) + 1;
}
