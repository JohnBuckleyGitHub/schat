/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 - 2009 IMPOMEZIA <schat@impomezia.com>
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

#include <QtCore>

#include "mirrorwriter.h"

/*!
 * \brief Конструктор класса MirrorWriter.
 *
 * \param version Список версий.
 * \param files   Список файлов.
 */
MirrorWriter::MirrorWriter(const QList<VersionInfo> &version, const QMultiMap<int, FileInfo> &files)
  : m_version(version), m_files(files)
{
  setAutoFormatting(true);
  setAutoFormattingIndent(2);
}


/*!
 * Запись XML файла.
 *
 * \param fileName Имя файла.
 * \return         Возвращает \a true в случае успешной записи.
 */
bool MirrorWriter::writeFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    return false;
  }

  if (m_version.isEmpty())
    return false;

  if (m_files.isEmpty())
    return false;

  setDevice(&file);

  writeStartDocument();
  writeStartElement("updates");
  writeAttribute("version", "1.1");

  writeStartElement("meta");
   foreach (VersionInfo info, m_version) {
     writeStartElement("version");
      writeAttribute("level", QString::number(info.level));
      writeAttribute("type", info.type);
      writeCharacters(info.version);
     writeEndElement();
   }
  writeEndElement();

  writeStartElement("files");
  writeAttribute("platform", "win32"); /// \todo Запись данного атрибута больше не нужна.
   writeStartElement("cumulative");
    QMapIterator<int, FileInfo> i(m_files);
    while (i.hasNext()) {
      i.next();
      FileInfo info = i.value();
      writeStartElement("file");
      writeAttribute("size", QString::number(info.size));
      writeAttribute("md5", info.md5);
      writeAttribute("type", info.type);
      writeAttribute("level", QString::number(i.key()));
      writeCharacters(info.name);
      writeEndElement();
    }
   writeEndElement();
  writeEndElement();

  writeEndDocument();
  return true;
}
