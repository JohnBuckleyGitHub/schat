/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#ifndef CHATWINDOWSTYLEOUTPUT_H_
#define CHATWINDOWSTYLEOUTPUT_H_

#include <QDateTime>
#include <QObject>

#include "chatwindowstyle.h"

/*!
 * \brief Служит для формирования HTML на основе выбраного стиля.
 */
class ChatWindowStyleOutput
{
public:
  ChatWindowStyleOutput(const QString &style, const QString &variant);
  ~ChatWindowStyleOutput();
  QString makeStylesheet(const QString &variant) const;

  QString makeSkeleton(const QString &chatName = "IMPOMEZIA Simple Chat",
                       const QString &ownerName = "%sourceName%",
                       const QString &partnerName = "%destinationName%",
                       const QString &ownerIconPath = "",
                       const QString &partnerIconPath = "",
                       const QString &time = QDateTime::currentDateTime().toString());

  QString makeMessage(const QString &sender = "%sender%",
                      const QString &message = "%message%",
                      bool direction = true,
                      bool sameSender = false,
                      const QString &avatarPath = "",
                      bool action = false,
                      const QString &time = QDateTime::currentDateTime().toString("hh:mm:ss"),
                      bool aligment = true);

  QString makeStatus(const QString &message = "%message%",
                     const QString &time = QDateTime::currentDateTime().toString("hh:mm:ss"));

private:
  void commonReplace(QString &html, const QString &msg, const QString &time);

  ChatWindowStyle *m_style;
  QString m_variant;
};

#endif /*CHATWINDOWSTYLEOUTPUT_H_*/
