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

#ifndef TOOLBARACTIONS_H_
#define TOOLBARACTIONS_H_

#include "ui/ToolBarAction.h"

/*!
 * Действия для форматирования текста в поле ввода.
 */
class TextEditAction : public ToolBarActionCreator
{
public:
  TextEditAction(int action);
  QAction* createAction(QObject *parent) const;

private:
  int m_inputAction;
};


/*!
 * Действие для установки цвета текста.
 */
class ColorAction : public ToolBarActionCreator
{
public:
  ColorAction();
  QWidget* createWidget(QWidget *parent) const;
};


/*!
 * Растяжка, делящая панель инструментов на 2 половины.
 */
class StretchAction : public ToolBarActionCreator
{
public:
  StretchAction();
  QWidget* createWidget(QWidget *parent) const;
};

#endif /* TOOLBARACTIONS_H_ */
