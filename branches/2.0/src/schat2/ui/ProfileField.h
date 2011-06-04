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

#ifndef PROFILEFIELD_H_
#define PROFILEFIELD_H_

#include "arora/lineedit.h"

class ChatSettings;

class ProfileField : public LineEdit
{
  Q_OBJECT

public:
  ProfileField(int key, QWidget *parent = 0);
  ProfileField(int key, const QString &contents, QWidget *parent = 0);

protected:
  virtual void updateData();

  ChatSettings *m_settings;
  int m_key;

private slots:
  void editingFinished();
  void settingsChanged(const QList<int> &keys);

private:
  void init();
};

#endif /* PROFILEFIELD_H_ */
