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

#ifndef SENDFILEPAGE_H_
#define SENDFILEPAGE_H_

#include "ui/tabs/SettingsTabHook.h"

class QLabel;

class SendFilePage : public SettingsPage
{
  Q_OBJECT

public:
  SendFilePage(QWidget *parent = 0);
  void retranslateUi();

private:
  QLabel *m_label;
};


class SendFilePageCreator : public SettingsPageCreator
{
public:
  SendFilePageCreator()
  : SettingsPageCreator(5100)
  {}

  SettingsPage* page(QWidget *parent = 0);
};

#endif /* SENDFILEPAGE_H_ */
