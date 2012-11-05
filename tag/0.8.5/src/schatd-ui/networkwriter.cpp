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

#include "networkwriter.h"

/*!
 * \brief Конструктор класса NetworkWriter.
 */
NetworkWriter::NetworkWriter(const QMap<QString, QString> &meta, const QStringList &servers)
  : m_meta(meta), m_servers(servers)
{
  setAutoFormatting(true);
  setAutoFormattingIndent(2);
}


bool NetworkWriter::writeFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    return false;
  }

  if (m_meta.isEmpty())
    return false;

  setDevice(&file);

  writeStartDocument();
  writeStartElement("network");
  writeAttribute("version", "1.0");

  writeStartElement("meta");
    QMapIterator<QString, QString> i(m_meta);
    while (i.hasNext()) {
      i.next();
      if (!i.value().isEmpty())
        writeTextElement(i.key(), i.value());
    }
  writeEndElement();

  if (!m_servers.isEmpty()) {
    writeStartElement("servers");
    foreach (QString server, m_servers)
      if (!server.isEmpty())
        writeTextElement("host", server);
    writeEndElement();
  }

  writeEndDocument();
  return true;
}
