/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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

#ifndef WIZARDSETTINGS_H_
#define WIZARDSETTINGS_H_

#include <QObject>

#include "abstractsettings.h"

#define settings (static_cast<WizardSettings *>(AbstractSettings::instance()))

/*!
 * \brief Класс читает и записывает настройки мастера.
 */
class WizardSettings : public AbstractSettings {
  Q_OBJECT

public:
  WizardSettings(const QString &filename, QObject *parent = 0);
  void read();

private:
  void setVersions();
};

#endif /* WIZARDSETTINGS_H_ */
