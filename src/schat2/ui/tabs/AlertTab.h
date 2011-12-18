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

#ifndef ALERTTAB_H_
#define ALERTTAB_H_

#include "ui/tabs/AbstractTab.h"

class Alert;
class ChatView;

class AlertTab : public AbstractTab
{
  Q_OBJECT

public:
  AlertTab(TabWidget *parent);

private slots:
  void alert(const Alert &alert);

private:
  void retranslateUi();

  ChatView *m_chatView; ///< Виджет отображающий текст чата.
};

#endif /* ALERTTAB_H_ */
