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

#include <QUuid>

#include "messages/MessageBox.h"

MessageBox::MessageBox(const QString &tpl, const QString &text, const QByteArray &destId)
  : AbstractMessage(QLatin1String("message-box-type"), text, destId)
{
  m_id = QUuid::createUuid().toString().mid(1, 36);
  m_template = tpl;
  m_bodyTpl = QLatin1String("content-small");
}


QString MessageBox::js(bool add) const
{
  QString html = tpl(m_template);
  type(html);
  id(html);
  text(html);

  html.replace(QLatin1String("%yes%"), QObject::tr("Yes"));
  html.replace(QLatin1String("%no%"), QObject::tr("No"));

  if (add)
    return appendMessage(html) + QLatin1String(" rebindButtonHandlers();");

  return html;
}
