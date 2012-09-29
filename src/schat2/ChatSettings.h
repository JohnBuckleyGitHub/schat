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

#ifndef CHATSETTINGS_H_
#define CHATSETTINGS_H_

#include <QStringList>

#include "Settings.h"

class FeedNotify;
class Notify;

class SCHAT_CORE_EXPORT ChatSettings : public Settings
{
  Q_OBJECT

public:
  ChatSettings(const QString &fileName, QObject *parent = 0);
  QVariant setDefaultAndRead(const QString &key, const QVariant &value);
  void init();
  void setLocalDefault(const QString &key, const QVariant &value);
  void setValue(const QString &key, const QVariant &value, bool notify = true, bool local = false);

private slots:
  void notify(const Notify &notify);
  void ready();

private:
  void set();
  void set(const FeedNotify &notify);
  void set(const QString &key, const QVariant &value);

  QStringList m_local; ///< Список ключей локальных настроек.
};

#endif /* CHATSETTINGS_H_ */
