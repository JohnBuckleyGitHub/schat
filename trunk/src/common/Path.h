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

#ifndef PATH_H_
#define PATH_H_

#include <QStringList>

#include "schat.h"

/*!
 * Предоставляет унифицированный доступ к файловым ресурсам чата.
 */
class SCHAT_EXPORT Path
{
  Path() {}

public:
  enum Scope {
    UserScope,
    SystemScope
  };

  static void init();
  inline static bool isPortable()    { return m_portable; }
  inline static QString app()        { return m_app; }
  inline static QString appDirPath() { return m_appDirPath; }
  static QString cache();
  static QString config();
  static QString data(Scope scope = UserScope);
  static QString file(const QString &path, const QString &name);
  static QStringList plugins();

private:
  static bool m_portable;      ///< \b true если все файлы чат хранит в своей папке.
  static QString m_app;        ///< Имя чата полученное на основе имени исполняемого файла.
  static QString m_appDirPath; ///< Папка чата.
};

#endif /* PATH_H_ */
