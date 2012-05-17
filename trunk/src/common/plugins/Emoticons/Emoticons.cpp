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
#include <QFileInfo>

#include <QFile>

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
    add(emoticon);
    qDebug() << emoticon->isValid() << QFileInfo(emoticon->file()).fileName() << emoticon->width() << emoticon->height() << emoticon->texts();
  }


  makeIndex();
  return true;
}


Emoticon Emoticons::get(const QString &key)
{
  return m_emoticons.value(EmoticonKey(key));
}


QString Emoticons::find(const QString &text, int pos)
{
  QMapIterator<EmoticonKey, Emoticon> i(m_emoticons);
  while (i.hasNext()) {
    i.next();
    if (text.indexOf(i.key().text(), pos) != -1)
      return i.key().text();
  }
  return QString();
}


void Emoticons::add(Emoticon emoticon)
{
  if (!emoticon->isValid())
    return;

  foreach (const QString &text, emoticon->texts()) {
    m_emoticons[EmoticonKey(text)] = emoticon;
  }
}


void Emoticons::makeIndex()
{
  m_index.clear();
  QMapIterator<EmoticonKey, Emoticon> i(m_emoticons);
  while (i.hasNext()) {
    i.next();
    if (!m_index.contains(i.key().text().at(0)))
      m_index.append(i.key().text().at(0));
  }

  qDebug() << m_index;
}
