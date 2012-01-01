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

#ifndef NODEFEEDSTORAGE_H_
#define NODEFEEDSTORAGE_H_

#include "feeds/FeedStorage.h"

class SCHAT_EXPORT NodeFeedStorage : public FeedStorage
{
  Q_OBJECT

public:
  NodeFeedStorage(QObject *parent = 0);

protected:
  int saveImpl(FeedPtr feed);

private:
  qint64 save(FeedPtr feed, const QByteArray &json);
  void start();
};

#endif /* NODEFEEDSTORAGE_H_ */
