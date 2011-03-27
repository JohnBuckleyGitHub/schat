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

#ifndef WELCOMETAB_H_
#define WELCOMETAB_H_

#include "ui/tabs/AbstractTab.h"

class QLineEdit;
class SimpleClient;

class WelcomeTab : public AbstractTab
{
  Q_OBJECT

public:
  WelcomeTab(SimpleClient *client, TabWidget *parent);

private slots:
  void nickChanged(const QString &text);

private:
  QLineEdit *m_nickEdit;
  SimpleClient *m_client;
};

#endif /* WELCOMETAB_H_ */
