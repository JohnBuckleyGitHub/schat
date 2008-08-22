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

#ifndef SENDWIDGET_H_
#define SENDWIDGET_H_

#include <QWidget>

#include "widget/inputwidget.h"

class InputWidget;

class SendWidget : public QWidget
{
  Q_OBJECT

public:
  SendWidget(QWidget *parent = 0);
  void clear() { m_input->clear(); }

signals:
  void sendMsg(const QString &message);

private:
  InputWidget *m_input;
};

#endif /*SENDWIDGET_H_*/
