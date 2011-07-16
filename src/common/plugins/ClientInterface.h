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

#ifndef CLIENTINTERFACE_H_
#define CLIENTINTERFACE_H_

#include <QObject>

class Settings;
class SimpleClient;

class ClientInterface
{
public:
  virtual ~ClientInterface() {}
  virtual QString name() const = 0;
  virtual void setClient(SimpleClient *client) = 0;
  virtual void setSettings(Settings *settings) = 0;
};

Q_DECLARE_INTERFACE(ClientInterface, "com.impomezia.schat.ClientInterface/1.0");

#endif /* CLIENTINTERFACE_H_ */
