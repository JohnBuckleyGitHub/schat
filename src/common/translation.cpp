/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#include <QCoreApplication>
#include <QFileInfo>
#include <QLocale>
#include <QTranslator>
//#include <QDebug>

#include "translation.h"

/*!
 * Конструктор класса Translation.
 */
Translation::Translation(QObject *parent)
  : QObject(parent),
  m_empty(true)
{
  m_language = tr("English");
  m_name = "en";
  m_prefix = "schat_";
  m_core = new QTranslator(this);
}


/*!
 * Загрузка языкового файла.
 * Допустимые значения для параметра \p name:
 * - auto или пустая строка, будет произведена попытка автоматически определить язык
 * в случае неудачи будет установлен английский язык.
 *
 * - Полный путь к qm файла, в случае неудачи будет попытка загрузить язык полученный из имени qm файла.
 *
 * - Код языка.
 */
void Translation::load(const QString &name)
{
  if (!m_empty) {
    QCoreApplication::removeTranslator(m_core);
  }
  else
    m_empty = false;

  if (name == "auto" || name.isEmpty()) {
    QLocale locale = QLocale::system();
    if (locale.language() == QLocale::C)
      m_name = "en";
    else
      m_name = locale.name();
  }
  else if (name.endsWith(".qm")) {
    QFileInfo fileInfo = QFileInfo(name);
    m_name = fileInfo.baseName().mid(m_prefix.size());
    if (m_core->load(name)) {
      m_language = m_core->translate("Translation", "English");
      QCoreApplication::installTranslator(m_core);
      return;
    }
    else {
      load(m_name);
    }
  }
  else {
    m_name = name;
  }

  bool loaded = false;

  for (int i = 0; i < m_search.size(); ++i) {
    loaded = m_core->load(m_prefix + m_name, m_search.at(i));
    if (loaded)
      break;
  }

  if (loaded) {
    m_language = m_core->translate("Translation", "English");
    QCoreApplication::installTranslator(m_core);
  }
  else if (m_name != "en")
    load("en");
}


void Translation::setSearch(const QStringList &search)
{
  m_search = search;
  m_search.append(":/translations");
}
