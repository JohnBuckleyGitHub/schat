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

#ifndef TOPICMESSAGE_H_
#define TOPICMESSAGE_H_

#include "Channel.h"
#include "messages/AbstractMessage.h"

class SCHAT_CORE_EXPORT TopicMessage : public AbstractMessage
{
public:
  TopicMessage(const Topic &topic);
  QString js(bool add = true) const;

private:
  void buttons(QString &html) const;
};

#endif /* TOPICMESSAGE_H_ */
