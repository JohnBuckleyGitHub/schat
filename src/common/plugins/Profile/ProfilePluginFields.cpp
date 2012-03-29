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

#include "ProfilePluginFields.h"
#include "sglobal.h"

ProfilePluginFields::ProfilePluginFields(QObject *parent)
  : Profile(parent)
{
  addField(LS("city"), 2000);
  addField(LS("site"), 4000);
  addField(LS("email"), 4050);
}


QString ProfilePluginFields::translateImpl(const QString &field) const
{
  if (field == LS("city"))
    return tr("City");
  else if (field == LS("site"))
    return tr("Site");
  else if (field == LS("email"))
    return tr("E-Mail");

  return QString();
}
