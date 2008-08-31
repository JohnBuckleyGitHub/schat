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

#ifndef EMOTICONSREADER_H_
#define EMOTICONSREADER_H_

class EmoticonsReader : public QXmlStreamReader {
  
public:
  EmoticonsReader(QMap<QString, QStringList> *emoticons);
  bool readFile(const QString &fileName);

private:
  void readEmoticon(const QString &file);
  void readMap();
  void readUnknownElement();

  QMap<QString, QStringList> *m_emoticons;
  QStringList m_text;
};

#endif /*EMOTICONSREADER_H_*/
