/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 * class SingleApplication Copyright © 2008, Benjamin C. Meyer <ben@meyerhome.net>
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

#ifndef SINGALAPPLICATION_H
#define SINGALAPPLICATION_H

#include <QApplication>

class QLocalServer;

/*!
 * \brief Обеспечивает запуск единственного экземпляра приложения.
 *
 * QApplication subclass that should be used when you only want one
 * instant of the application to exist at a time.
*/
class SingleApplication : public QApplication
{
  Q_OBJECT

public:
  SingleApplication(int &argc, char **argv);
  bool sendMessage(const QString &message);
  bool startSingleServer();
  bool isRunning() const;

signals:
  void messageRecieved(const QString &message);

private slots:
  void newConnection();

private:
  QString serverName() const;
  const QString m_serverName;
  QLocalServer *m_localServer;
};

#endif

