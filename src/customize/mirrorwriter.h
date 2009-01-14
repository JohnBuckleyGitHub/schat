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

#ifndef MIRRORWRITER_H_
#define MIRRORWRITER_H_

#include <QXmlStreamWriter>

#include "updatexmlreader.h"

/*!
 * \brief Низкоуровневый класс для записи xml-файла обновлений.
 */
class MirrorWriter : public QXmlStreamWriter {

public:
  MirrorWriter(const QList<VersionInfo> &version, const QMultiMap<int, FileInfo> &files, const QString &baseUrl = QString());
  bool writeFile(const QString &fileName);
  inline void comparable(bool enable) { m_comparable = enable; }

private:
  bool m_comparable;
  const QList<VersionInfo> m_version;
  const QMultiMap<int, FileInfo> m_files;
  const QString m_baseUrl;
};

#endif /* MIRRORWRITER_H_ */
