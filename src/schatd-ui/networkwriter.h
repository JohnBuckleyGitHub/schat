/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#ifndef NETWORKWRITER_H_
#define NETWORKWRITER_H_

#include <QStringList>
#include <QXmlStreamWriter>

/*!
 * \brief Низкоуровневый класс для записи xml-файла сети.
 */
class NetworkWriter : public QXmlStreamWriter {

public:
  NetworkWriter(const QMap<QString, QString> &meta, const QStringList &servers);
  bool writeFile(const QString &fileName);

private:
  const QMap<QString, QString> m_meta;
  const QStringList m_servers;
};

#endif /* NETWORKWRITER_H_ */
