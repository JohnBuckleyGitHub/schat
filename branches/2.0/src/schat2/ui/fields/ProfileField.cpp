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

#include "ChatCore.h"
#include "ChatSettings.h"
#include "ui/fields/ProfileField.h"

ProfileField::ProfileField(int key, QWidget *parent)
  : LineEdit(ChatCore::i()->settings()->value(key).toString(), parent)
  , m_settings(ChatCore::i()->settings())
  , m_key(key)
{
  init();
}


ProfileField::ProfileField(int key, const QString &contents, QWidget *parent)
  : LineEdit(contents, parent)
  , m_settings(ChatCore::i()->settings())
  , m_key(key)
{
  init();
}


void ProfileField::updateData()
{
  m_settings->updateValue(m_key, text());
}


void ProfileField::editingFinished()
{
  updateData();
}


void ProfileField::settingsChanged(const QList<int> &keys)
{
  if (!keys.contains(m_key))
    return;

  setText(m_settings->value(m_key).toString());
}


void ProfileField::init()
{
  connect(m_settings, SIGNAL(changed(const QList<int> &)), SLOT(settingsChanged(const QList<int> &)));
  connect(this, SIGNAL(editingFinished()), SLOT(editingFinished()));
}

