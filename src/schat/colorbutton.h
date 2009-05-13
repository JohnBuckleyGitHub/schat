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

#include <QLabel>
#include <QToolButton>
#include <QWidget>

/*!
 * \brief Виджет для выбора цветов.
 */
class ColorWidget : public QWidget
{
  Q_OBJECT

public:
  ColorWidget(QWidget *parent = 0);

signals:
  void newColor(const QColor &color);

protected:
  void mouseReleaseEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event);

private:
  QImage m_image;
};

/*!
 * \brief Кнопка для выбора цвета текста.
 */
class ColorButton : public QToolButton
{
  Q_OBJECT

public:
  ColorButton(QWidget *parent = 0);
  ~ColorButton();

private slots:
  void setColor(const QColor &color);

private:
  ColorWidget *m_colorWidget;
};


#endif /* COLORBUTTON_H_ */
