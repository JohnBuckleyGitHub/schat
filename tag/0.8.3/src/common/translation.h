/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#ifndef TRANSLATION_H_
#define TRANSLATION_H_

#include <QObject>
#include <QStringList>

class QTranslator;

#define CURRENT_LANG SimpleChatApp::instance()->translation()->language()

class Translation : public QObject
{
  Q_OBJECT

public:
  Translation(QObject *parent = 0);
  inline QString language() const              { return m_language; }
  inline QString name() const                  { return m_name; }
  inline QString prefix() const                { return m_prefix; }
  inline QStringList search() const            { return m_search; }
  inline void setPrefix(const QString &prefix) { m_prefix = prefix; }
  void load(const QString &name);
  void setSearch(const QString &search);
  void setSearch(const QStringList &search);

private:
  void loadQt();

  bool m_empty;         ///< true если не было загружено ни одного перевода.
  QString m_language;   ///< Человеко-понятное переведённое имя языка.
  QString m_name;       ///< Имя языка, например ru, en или auto.
  QString m_prefix;     ///< Префикс файлов перевода.
  QStringList m_search; ///< Список директорий для поиска переводов.
  QTranslator *m_core;  ///< Основной загрузчик переводов.
  QTranslator *m_qt;    ///< Загрузчки перевода Qt.
};

#endif /* TRANSLATION_H_ */
