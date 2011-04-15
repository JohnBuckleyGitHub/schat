/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 * idle.h - detect desktop idle time
 * Copyright © 2003 Justin Karneges <justin@affinix.com> (from KVIrc source code)
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

#ifndef IDLE_H_
#define IDLE_H_

#include <QObject>
#include <QCursor>
#include <QDateTime>
#include <QTimer>

class IdlePlatform;

class Idle : public QObject
{
  Q_OBJECT

public:
  Idle();
  ~Idle();

  bool isActive() const;
  bool usingPlatform() const;
  int secondsIdle();
  void start();
  void stop();

signals:
  void secondsIdle(int);

private slots:
  void doCheck();

private:
  class Private;
  Private * const d;
};


class IdlePlatform
{
public:
  IdlePlatform();
  ~IdlePlatform();

  bool init();
  int secondsIdle();

private:
  #ifndef Q_OS_WINCE
    class Private;
    Private * const d;
  #endif
};


class Idle::Private
{
public:
  Private() {}

  QPoint lastMousePos;
  QDateTime idleSince;

  int idleTime;
  QDateTime startTime;
  QTimer checkTimer;
};

#endif
