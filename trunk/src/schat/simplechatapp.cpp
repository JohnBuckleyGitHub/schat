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

#include <QtGui>
#include <QtNetwork>

#include "simplechatapp.h"
#include "version.h"

SimpleChatApp::SimpleChatApp(int &argc, char **argv)
  : SingleApplication(argc, argv)
{
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

  QString appPath = applicationDirPath();

  setApplicationName(SCHAT_NAME);
  setApplicationVersion(SCHAT_VERSION);
  setOrganizationName(SCHAT_ORGANIZATION);
  setOrganizationDomain(SCHAT_DOMAIN);
  setQuitOnLastWindowClosed(false);
  addLibraryPath(appPath + "/plugins");
  setStyle(new QPlastiqueStyle);

  QStringList args = arguments();
  args.takeFirst();
  QString message;

  if (!args.isEmpty())
    message = args.join(", ");

  if (sendMessage(message))
    return;

  if (args.contains("-exit"))
    return;

  if (!startSingleServer())
    return;
}


SimpleChatApp::~SimpleChatApp()
{
}


SimpleChatApp *SimpleChatApp::instance()
{
  return (static_cast<SimpleChatApp *>(QCoreApplication::instance()));
}
