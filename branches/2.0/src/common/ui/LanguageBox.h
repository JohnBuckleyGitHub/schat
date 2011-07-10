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

#ifndef LANGUAGEBOX_H_
#define LANGUAGEBOX_H_

#include <QComboBox>

class Translation;

/*!
 * Виджет выбора языка.
 */
class LanguageBox : public QComboBox
{
  Q_OBJECT

public:
  LanguageBox(Translation *translation, QWidget *parent = 0);
  QString qmFile() const;

private:
  QString languageIcon(const QString &file) const;
  QString languageName(const QString &file) const;
  QStringList findQmFiles() const;

  const QString m_language;   ///< Человеко-понятное переведённое имя языка.
  const QString m_prefix;     ///< Префикс файлов перевода.
  const QStringList m_search; ///< Список директорий для поиска переводов.
  Translation *m_translation;
};

#endif /* LANGUAGEBOX_H_ */
