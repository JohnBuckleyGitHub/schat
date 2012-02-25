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

#include "arora/lineedit.h"

class NickEdit : public LineEdit
{
  Q_OBJECT

public:
  NickEdit(QWidget *parent = 0);

private slots:
  void editingFinished();
  void settingsChanged(const QString &key, const QVariant &value);
  void textChanged();
};

#endif /* NICKEDIT_H_ */