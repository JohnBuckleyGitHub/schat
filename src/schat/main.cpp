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

#include <QApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>

#include "settings.h"
#include "simplechatapp.h"

#ifdef SCHAT_STATIC
  Q_IMPORT_PLUGIN(qgif)
#endif

int main(int argc, char *argv[])
{
  SimpleChatApp app(argc, argv);
  if (app.isRunning())
    return 0;

  #ifndef SCHAT_NO_UPDATE
    if (Settings::install())
      return 0;
  #endif

  return app.run();
}
