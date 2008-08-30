/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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

#ifndef ICONDEFREADER_H_
#define ICONDEFREADER_H_

#include <QXmlStreamReader>

class IconDefReader : public QXmlStreamReader {
  
public:
  IconDefReader(QHash<QString, QStringList> *emoticons);
  bool readFile(const QString &fileName);

private:
  void readIcon();
  void readIcondef();
  void readUnknownElement();

  QHash<QString, QStringList> *m_emoticons;
  QStringList m_text;
};

#endif /*ICONDEFREADER_H_*/
