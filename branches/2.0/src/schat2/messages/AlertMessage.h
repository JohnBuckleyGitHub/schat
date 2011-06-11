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

#ifndef ALERTMESSAGE_H_
#define ALERTMESSAGE_H_

#include "messages/AbstractMessage.h"

class AlertMessage : public AbstractMessage
{
public:
  enum AlertType {
    Information,
    Exclamation
  };

  AlertMessage(AlertType alertType, const QString &text, const QByteArray &destId = QByteArray());
  QString js() const;

private:
  AlertType m_alertType;
};

#endif /* ALERTMESSAGE_H_ */
