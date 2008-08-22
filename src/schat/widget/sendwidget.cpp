/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui>

#include "widget/sendwidget.h"

/*!
 * \class SendWidget
 * \brief Виджет полностью берущий на себя ввода текста.
 */

/*!
 * \brief Конструктор класса SendWidget.
 */
SendWidget::SendWidget(QWidget *parent)
  : QWidget(parent)
{
  m_input = new InputWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(m_input);  
  mainLayout->setMargin(0);

  connect(m_input, SIGNAL(sendMsg(const QString &)), SIGNAL(sendMsg(const QString &)));
}
