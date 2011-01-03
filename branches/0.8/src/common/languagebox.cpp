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

#include <QDir>
#include <QTranslator>

#include "languagebox.h"

/*!
 * Конструктор класса LanguageBox.
 */
LanguageBox::LanguageBox(const QString &language, const QString &prefix, const QStringList &search, QWidget *parent)
  : QComboBox(parent),
  m_language(language),
  m_prefix(prefix),
  m_search(search)
{
  setIconSize(QSize(25, 15));
  addItem(QIcon(":/translations/en.png"), "English", ":/translations/" + m_prefix + "en.qm");

  QStringList qmFiles = findQmFiles();
  qmFiles.append(":/translations/" + m_prefix + "ru.qm");
  bool canOverrideEnglish = true;

  for (int i = 0; i < qmFiles.size(); ++i) {
    QString file = qmFiles[i];
    QString langName = languageName(file);

    if (langName.isEmpty())
      continue;

    // Добавляем в список только уникальные языки.
    if (findText(langName) == -1) {
      addItem(QIcon(languageIcon(file)), langName, file);
    }
    else if (canOverrideEnglish && file.endsWith(m_prefix + "en.qm")) {
      setItemIcon(0, QIcon(languageIcon(file)));
      setItemData(0, file);
      canOverrideEnglish = false;
    }
  }

  setCurrentIndex(findText(m_language));
}


QString LanguageBox::qmFile() const
{
  return itemData(currentIndex()).toString();
}


/*!
 * Возвращает путь к файлу изображения флага языка.
 */
QString LanguageBox::languageIcon(const QString &file) const
{
  QFileInfo fi(file);
  QString icon = fi.absolutePath() + "/" + fi.baseName().remove(0, m_prefix.size()) + ".png";

  if (QFile::exists(icon))
    return icon;

  if (icon.endsWith("ru.png"))
    return ":/translations/ru.png";

  if (icon.endsWith("en.png"))
    return ":/translations/en.png";

  return ":/images/lang/unknown.png";
}


QString LanguageBox::languageName(const QString &file) const
{
  QTranslator translator;
  translator.load(file);

  return translator.translate("Translation", "English");
}


/*!
 * Поиск qm файлов.
 */
QStringList LanguageBox::findQmFiles() const
{
  QStringList fileNames;

  foreach (QString d, m_search) {
    QDir dir(d);
    QStringList fn = dir.entryList(QStringList(m_prefix + "*.qm"), QDir::Files, QDir::Name);
    QMutableStringListIterator i(fn);
    while (i.hasNext()) {
        i.next();
        i.setValue(dir.filePath(i.value()));
    }
    fileNames += fn;
  }

  return fileNames;
}
