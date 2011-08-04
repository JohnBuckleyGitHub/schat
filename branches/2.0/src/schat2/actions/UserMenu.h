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

#ifndef USERMENU_H_
#define USERMENU_H_

#include "actions/MenuBuilder.h"
#include "User.h"

class QUrl;

class SCHAT_CORE_EXPORT UserMenu : public MenuBuilder
{
  Q_OBJECT

public:
  UserMenu(ClientUser user, QObject *parent = 0);
  static void insertNick(const QString &nick);
  static void insertNick(const QUrl &url);
  void bind(QMenu *menu);

public slots:
  void triggered(QAction *action);

private:
  void init();

  bool m_self;
  ClientUser m_user;
  QAction *m_ignore;
  QAction *m_insert;
  QAction *m_talk;
};

#endif /* USERMENU_H_ */
