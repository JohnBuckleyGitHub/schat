/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include <QDebug>

#include <QFile>

#include "EmoticonData.h"
#include "Emoticons.h"
#include "Extension.h"
#include "JSON.h"
#include "sglobal.h"

Emoticons::Emoticons(QObject *parent)
  : QObject(parent)
{
}


/*!
 * Загрузка смайлов.
 */
bool Emoticons::load(Extension *extension)
{
  if (!extension)
    return false;

  QFile file(extension->root() + LS("/emoticons.json"));
  if (!file.exists())
    return false;

  if (!file.open(QIODevice::ReadOnly))
    return false;

  QVariantMap data = JSON::parse(file.readAll()).toMap().value(LS("emoticons")).toMap();
  if (data.isEmpty())
    return false;

  QMapIterator<QString, QVariant> i(data);
  while (i.hasNext()) {
    i.next();
    Emoticon emoticon = Emoticon(new EmoticonData(extension->root() + LC('/') + i.key(), extension->id(), i.value().toList()));
    qDebug() << emoticon->isValid() << emoticon->file() << emoticon->width() << emoticon->height() << emoticon->texts();
  }

  qDebug() << "Emoticons::load()" << extension;
  return false;
}
