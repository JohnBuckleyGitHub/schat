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

#ifndef NICKEDIT_H_
#define NICKEDIT_H_

#include "ui/fields/ProfileField.h"

class QToolBar;
class QToolButton;

class NickEdit : public ProfileField
{
  Q_OBJECT

public:
  NickEdit(QWidget *parent = 0);
  NickEdit(const QString &contents, QWidget *parent = 0);

private:
  void init();

  QToolBar *m_toolBar;
  QToolButton *m_color;
  QToolButton *m_config;
};

#endif /* NICKEDIT_H_ */
