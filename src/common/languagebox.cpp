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
  addItem(QIcon(":/translations/" + m_prefix + "en.png"), "English", ":/translations/" + m_prefix + "en.qm");

  QStringList qmFiles = findQmFiles();
  qmFiles.append(":/translations/schat_ru.qm");
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
    else if (canOverrideEnglish && file.endsWith("schat_en.qm")) {
      setItemIcon(0, QIcon(languageIcon(file)));
      setItemData(0, file);
      canOverrideEnglish = false;
    }
  }

  setCurrentIndex(findText(m_language));
}


/*!
 * Возвращает путь к файлу изображения флага языка.
 * Предполагается, что имя файла с изображением флага языка совпадает с именем языкового файла
 * за исключением расширения, например schat_ru.qm и schat_ru.png.
 * В случае если не будут найдены изображения для русского или английского языка,
 * то для них будут использованы изображения по умолчанию.
 */
QString LanguageBox::languageIcon(const QString &file) const
{
  QString icon = file.left(file.size() - 3) + ".png";
  if (QFile::exists(icon))
    return icon;

  if (icon.endsWith("schat_ru.png"))
    return ":/translations/schat_ru.png";

  if (icon.endsWith("schat_en.png"))
    return ":/translations/schat_en.png";

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
