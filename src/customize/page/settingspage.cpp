/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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

#include <QtGui>

#include "settingspage.h"
#include "wizardsettings.h"

/*!
 * \brief Конструктор класса SettingsPage.
 */
SettingsPage::SettingsPage(QWidget *parent)
  : QWizardPage(parent)
{
  m_settings = settings;

  setTitle(tr("Переопределение настроек по умолчанию"));
  setSubTitle(tr("Выберите какие основные настройки нужно переопределить"));

  m_overrideNetwork = new QCheckBox(tr("&Файл сети"), this);
  m_network = new QComboBox(this);

  m_overrideEmoticons = new QCheckBox(tr("&Тема смайликов"), this);
  m_emoticons = new QComboBox(this);

  m_overrideMirror = new QCheckBox(tr("&Зеркало обновлений"), this);
  m_mirror = new QLineEdit(this);

  QGroupBox *group = new QGroupBox(tr("Основные настройки"), this);
  QGridLayout *groupLay = new QGridLayout(group);
  groupLay->addWidget(m_overrideNetwork, 0, 0);
  groupLay->addWidget(m_network, 0, 1);
  groupLay->addWidget(m_overrideEmoticons, 1, 0);
  groupLay->addWidget(m_emoticons, 1, 1);
  groupLay->addWidget(m_overrideMirror, 2, 0);
  groupLay->addWidget(m_mirror, 2, 1, 1, 2);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(group);
  mainLay->addStretch();

  connect(m_overrideNetwork,   SIGNAL(clicked(bool)), m_network, SLOT(setEnabled(bool)));
  connect(m_overrideEmoticons, SIGNAL(clicked(bool)), m_emoticons, SLOT(setEnabled(bool)));
  connect(m_overrideMirror,    SIGNAL(clicked(bool)), m_mirror, SLOT(setEnabled(bool)));

  m_network->setEnabled(m_overrideNetwork->isChecked());
  m_emoticons->setEnabled(m_overrideEmoticons->isChecked());
  m_mirror->setEnabled(m_overrideMirror->isChecked());
}
