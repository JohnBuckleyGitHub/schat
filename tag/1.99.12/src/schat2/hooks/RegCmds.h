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

#ifndef REGCMDS_H_
#define REGCMDS_H_

#include "client/ClientHooks.h"

class SCHAT_CORE_EXPORT RegCmds : public Hooks::Messages
{
  Q_OBJECT

public:
  RegCmds(QObject *parent = 0);
  bool command(const QByteArray &dest, const ClientCmd &cmd);

  static QVariantMap request(const QString &action, const QString &name, const QString &password);
  static void signOut();
};

#endif /* REGCMDS_H_ */