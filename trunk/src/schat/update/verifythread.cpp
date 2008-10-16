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
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore>

#include "verifythread.h"

VerifyThread::VerifyThread(QObject *parent)
{
  m_stopped = false;
}


void VerifyThread::run()
{
  forever {
    m_mutex.lock();
    if (m_stopped) {
      m_stopped = false;
      m_mutex.unlock();
      break;
    }
    m_mutex.unlock();

  }
}


void VerifyThread::stop()
{
  m_mutex.lock();
  m_stopped = true;
  m_mutex.unlock();
}
