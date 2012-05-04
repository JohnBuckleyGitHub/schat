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

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/IOKitLib.h>

IdlePlatform::IdlePlatform()
{
}


IdlePlatform::~IdlePlatform()
{
}


bool IdlePlatform::init()
{
  if (secondsIdle() == -1)
    return false;

  return true;
}


int IdlePlatform::secondsIdle()
{
  mach_port_t masterPort;
  io_iterator_t iter;
  io_registry_entry_t curObj;

  IOMasterPort(MACH_PORT_NULL, &masterPort);
  IOServiceGetMatchingServices(masterPort, IOServiceMatching("IOHIDSystem"), &iter);
  if (iter == 0)
    return -1;

  curObj = IOIteratorNext(iter);
  if (curObj == 0)
    return -1;

  CFMutableDictionaryRef properties = 0;
  CFTypeRef obj;
  int result = -1;

  if (IORegistryEntryCreateCFProperties(curObj, &properties, kCFAllocatorDefault, 0) == KERN_SUCCESS && properties != NULL) {
    obj = CFDictionaryGetValue(properties, CFSTR("HIDIdleTime"));
    CFRetain(obj);
  } else
    obj = NULL;

  if (obj) {
    uint64_t tHandle;

    CFTypeID type = CFGetTypeID(obj);

    if (type == CFDataGetTypeID())
      CFDataGetBytes((CFDataRef) obj, CFRangeMake(0, sizeof(tHandle)), (UInt8*) &tHandle);
    else if (type == CFNumberGetTypeID())
      CFNumberGetValue((CFNumberRef)obj, kCFNumberSInt64Type, &tHandle);
    else
      return -1;

    CFRelease(obj);

    // essentially divides by 10^9
    tHandle >>= 30;
    result = tHandle;
  }

  /* Release our resources */
  IOObjectRelease(curObj);
  IOObjectRelease(iter);
  CFRelease((CFTypeRef)properties);
  return result;
}
