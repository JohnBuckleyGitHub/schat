/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
 * Copyright © 2012 Alexey Ivanov <alexey.ivanes@gmail.com>
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

#ifndef SENDFILEPAGE_H_
#define SENDFILEPAGE_H_

#include <QList>
#include <QTableWidget>
#include "SpellBackend.h"
#include "SpellChecker.h"

#include "ui/tabs/SettingsTabHook.h"

class QLabel;
class SpellCheckerPluginImpl;
class QTableWidget;
class Spellchecker;

class SpellCheckerPage : public SettingsPage
{
  Q_OBJECT

public:
  SpellCheckerPage(QWidget *parent = 0);
  void retranslateUi();

protected slots:
  void modified(QTableWidgetItem *item);

private:
  QLabel *m_label;              ///< Надпись вверху страницы.
  QTableWidget *m_table;
};


class SpellCheckerPageCreator : public SettingsPageCreator
{
public:
  SpellCheckerPageCreator()
  : SettingsPageCreator(5100)
  {}

  SettingsPage* page(QWidget *parent = 0);

};

#endif /* SENDFILEPAGE_H_ */
