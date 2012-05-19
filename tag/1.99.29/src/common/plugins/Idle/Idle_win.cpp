/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include "Idle.h"

#include <QLibrary>
#include <qt_windows.h>

class IdlePlatform::Private
{
public:
  Private()
  : GetLastInputInfo(0)
  , IdleUIGetLastInputTime(0)
  , lib(0)
  {
  }

  BOOL (__stdcall * GetLastInputInfo)(PLASTINPUTINFO);
  DWORD (__stdcall * IdleUIGetLastInputTime)(void);
  QLibrary *lib;
};


IdlePlatform::IdlePlatform()
{
  d = new Private;
}


IdlePlatform::~IdlePlatform()
{
  if (d->lib)
    delete d->lib;

  delete d;
}


bool IdlePlatform::init()
{
  if (d->lib)
    return true;

  void *p;

  // try to find the built-in Windows 2000 function
  d->lib = new QLibrary("user32");
  if (d->lib->load() && (p = d->lib->resolve("GetLastInputInfo"))) {
    d->GetLastInputInfo = (BOOL(__stdcall *)(PLASTINPUTINFO)) p;
    return true;
  }
  else {
    delete d->lib;
    d->lib = 0;
  }

  // fall back on idleui
  d->lib = new QLibrary("idleui");
  if (d->lib->load() && (p = d->lib->resolve("IdleUIGetLastInputTime"))) {
    d->IdleUIGetLastInputTime = (DWORD(__stdcall *)(void)) p;
    return true;
  }
  else {
    delete d->lib;
    d->lib = 0;
  }

  return false;
}


int IdlePlatform::secondsIdle()
{
  int i;
  if (d->GetLastInputInfo) {
    LASTINPUTINFO li;
    li.cbSize = sizeof(LASTINPUTINFO);
    bool ok = d->GetLastInputInfo(&li);
    if (!ok)
      return 0;
    i = li.dwTime;
  }
  else if (d->IdleUIGetLastInputTime) {
    i = d->IdleUIGetLastInputTime();
  }
  else
    return 0;

  return (GetTickCount() - i) / 1000;
}
