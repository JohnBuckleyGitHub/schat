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

#include "messages/TopicMessage.h"
#include "net/packets/message.h"


TopicMessage::TopicMessage(const Topic &topic)
  : AbstractMessage(QLatin1String("user-type"), topic.topic, topic.channel)
{
  m_senderId = topic.author;
  m_timestamp = topic.timestamp;
  m_template = QLatin1String("topic");
}


QString TopicMessage::js(bool add) const
{
  if (m_text.isEmpty()) {
    QString html;
    return appendMessage(html, QLatin1String("setTopic"));
  }

  QString html = tpl(m_template);
  time(html);
  nick(html);
  text(html);
  return appendMessage(html, QLatin1String("setTopic"));
}
