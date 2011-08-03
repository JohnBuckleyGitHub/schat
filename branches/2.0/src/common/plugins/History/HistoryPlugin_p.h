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

#ifndef HISTORYPLUGIN_P_H_
#define HISTORYPLUGIN_P_H_

#include "plugins/AbstractHistory.h"

class HistoryDB;
class RawUserMessageHook;

class History : public AbstractHistory
{
  Q_OBJECT

public:
  History(ChatCore *core);
  QList<HookData::Type> hooks() const;
  void hook(const HookData &data);
  void notify(int notice, const QVariant &data);

private:
  void add(const RawUserMessageHook &data);
  void openDb();

  HistoryDB *m_db;
};


#endif /* HISTORYPLUGIN_P_H_ */
