/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#ifndef DAEMONSETTINGS_H_
#define DAEMONSETTINGS_H_

#include <QObject>

#include "abstractsettings.h"

#define DaemonSettingsInstance (static_cast<DaemonSettings *>(AbstractSettings::instance()))

/*!
 * \brief Класс читает настройки сервера.
 *
 * Читает настройки и предоставляет методы для получения настроек.
 */
class DaemonSettings : public AbstractSettings {
  Q_OBJECT

public:
  DaemonSettings(const QString &filename, QObject *parent = 0);
  void read();
  void reload();

private:
  void mutableKeys();
};

#endif /*DAEMONSETTINGS_H_*/
