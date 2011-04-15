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

#ifndef SETTINGSPAGE_H_
#define SETTINGSPAGE_H_

#include <QWizardPage>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class WizardSettings;

/*!
 * \brief Страница переопределения основных настроек.
 */
class SettingsPage : public QWizardPage
{
  Q_OBJECT

public:
  SettingsPage(QWidget *parent = 0);
  bool validatePage();

private:
  void emoticonsList();
  void networkList();

  QCheckBox *m_autoDownload;
  QCheckBox *m_overrideEmoticons;
  QCheckBox *m_overrideMirror;
  QComboBox *m_emoticons;
  QComboBox *m_network;
  QLabel *m_overrideNetwork;
  QLineEdit *m_mirror;
  WizardSettings *m_settings;
};

#endif /* SETTINGSPAGE_H_ */
