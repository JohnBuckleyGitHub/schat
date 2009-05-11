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

#ifndef COLORBUTTON_H_
#define COLORBUTTON_H_

#include <QWidget>
#include <QToolButton>

/*!
 * \brief Кнопка для выбора цвета текста.
 */
class ColorButton : public QToolButton
{
  Q_OBJECT

public:
  ColorButton(QWidget *parent = 0);
  ~ColorButton();

private:
  void setColor(const QColor &color);
};


#endif /* COLORBUTTON_H_ */
