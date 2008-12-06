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
#include <limits.h>

#include "selectpage.h"

SelectPage::SelectPage(QWidget *parent)
  : QWizardPage(parent)
{
  setTitle(tr("Выбор необходимых действий"));
  setSubTitle(tr("Выберете действия и укажите необходимые сведения для продолжения работы мастера"));

  m_version = new QLineEdit(this);
  m_version->setValidator(new QRegExpValidator(QRegExp("([0-9]{1,3}[\\.]{1}){3}[0-9]{1,5}"), m_version));
  m_version->setToolTip(tr("Версия приложения, будет использоваться инсталляторе\nдопустимый формат x.x.x.x"));

  QLabel *versionLabel = new QLabel(tr("&Версия:"), this);
  versionLabel->setBuddy(m_version);

  m_suffix = new QLineEdit(this);
  QLabel *suffixLabel = new QLabel(tr("&Суффикс файлов:"), this);
  suffixLabel->setBuddy(m_suffix);

  QGroupBox *versionGroup = new QGroupBox(tr("Основные сведения"));
  QGridLayout *versionLay = new QGridLayout(versionGroup);
  versionLay->addWidget(versionLabel, 0, 0);
  versionLay->addWidget(m_version, 0, 1);
  versionLay->addWidget(suffixLabel, 0, 2);
  versionLay->addWidget(m_suffix, 0, 3);
  versionLay->setColumnStretch(1, 2);
  versionLay->setColumnStretch(3, 1);

  m_core = new QCheckBox(tr("&Ядро"), this);
  m_coreLabel = new QLabel(tr("Уровни обновлений:"), this);
  m_coreLevel = new QSpinBox(this);
  m_coreLevel->setRange(1, INT_MAX);

  m_runtime = new QCheckBox(tr("&Библиотека Qt"), this);
  m_runtimeLevel = new QSpinBox(this);
  m_runtimeLevel->setRange(1, INT_MAX);

  m_overrideLevels = new QCheckBox(tr("&Переопределить уровни обновлений"), this);
  QGroupBox *updateGroup = new QGroupBox(tr("Альтернативное зеркало обновлений"), this);
  updateGroup->setCheckable(true);
  QGridLayout *updateLay = new QGridLayout(updateGroup);
  updateLay->addWidget(m_core, 0, 0);
  updateLay->addWidget(m_coreLabel, 0, 1);
  updateLay->addWidget(m_coreLevel, 0, 2);
  updateLay->addWidget(m_runtime, 1, 0, 1, 2);
  updateLay->addWidget(m_runtimeLevel, 1, 2);
  updateLay->addWidget(m_overrideLevels, 2, 0, 1, 3);
  updateLay->setColumnStretch(0, 1);

  m_save = new QCheckBox(tr("Со&хранить"), this);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(versionGroup);
  mainLay->addWidget(updateGroup);
  mainLay->addWidget(m_save);
  mainLay->addStretch();

  connect(m_core, SIGNAL(clicked(bool)), SLOT(clickedCore(bool)));
  connect(m_runtime, SIGNAL(clicked(bool)), m_runtimeLevel, SLOT(setEnabled(bool)));

  clickedCore(m_core->isChecked());
}


void SelectPage::clickedCore(bool checked)
{
  m_coreLabel->setEnabled(checked);
  m_coreLevel->setEnabled(checked);
  m_overrideLevels->setEnabled(checked);
  m_runtime->setEnabled(checked);

  if (checked)
    m_runtimeLevel->setEnabled(m_runtime->isChecked());
  else
    m_runtimeLevel->setEnabled(checked);
}
