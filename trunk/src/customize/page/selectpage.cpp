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
#include "version.h"
#include "wizardsettings.h"

/*!
 * \brief Конструктор класса SelectPage.
 */
SelectPage::SelectPage(QWidget *parent)
  : QWizardPage(parent)
{
  m_settings = settings;
  setTitle(tr("Выбор необходимых действий"));
  setSubTitle(tr("Выберете действия и укажите необходимые сведения для продолжения работы мастера"));

  m_version = new QLineEdit(m_settings->getString("Version"), this);
  m_version->setValidator(new QRegExpValidator(QRegExp("([0-9]{1,3}[\\.]{1}){3}[0-9]{1,5}"), m_version));
  m_version->setToolTip(tr("Версия приложения, будет использоваться инсталляторе\nдопустимый формат x.x.x.x"));

  QLabel *versionLabel = new QLabel(tr("&Версия:"), this);
  versionLabel->setBuddy(m_version);

  m_suffix = new QLineEdit(m_settings->getString("Suffix"), this);
  m_suffix->setValidator(new QRegExpValidator(QRegExp("[0-9a-zA-Z]{0,20}"), m_suffix));
  m_suffix->setToolTip(tr("Суффикс для имён файлов инсталлятора\nдопускаются только латинские символы и цифры"));

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
  m_core->setChecked(m_settings->getBool("MirrorCore"));
  m_core->setToolTip(tr("Создать обновление для основных компонентов"));
  m_coreLabel = new QLabel(tr("Уровни обновлений:"), this);
  m_coreLevel = new QSpinBox(this);
  m_coreLevel->setRange(1, INT_MAX);
  m_coreLevel->setValue(m_settings->getInt("LevelCore"));
  m_coreLevel->setToolTip(tr("Уровень обновления основных компонентов"));

  m_runtime = new QCheckBox(tr("&Библиотека Qt"), this);
  m_runtime->setChecked(m_settings->getBool("MirrorQt"));
  m_runtime->setToolTip(tr("Создать обновление для библиотеки Qt"));
  m_runtimeLevel = new QSpinBox(this);
  m_runtimeLevel->setRange(1, INT_MAX);
  m_runtimeLevel->setValue(m_settings->getInt("LevelQt"));
  m_runtimeLevel->setToolTip(tr("Уровень обновления библиотеки Qt"));

  m_overrideLevels = new QCheckBox(tr("&Переопределить уровни обновлений"), this);
  m_overrideLevels->setChecked(m_settings->getBool("OverrideLevels"));
  m_overrideLevels->setToolTip(tr("Переопределить жёстко заданные уровни обновлений\nпри помощи default.conf"));

  m_mirror = new QGroupBox(tr("Альтернативное зеркало обновлений"), this);
  m_mirror->setCheckable(true);
  m_mirror->setChecked(m_settings->getBool("Mirror"));

  QGridLayout *updateLay = new QGridLayout(m_mirror);
  updateLay->addWidget(m_core, 0, 0);
  updateLay->addWidget(m_coreLabel, 0, 1);
  updateLay->addWidget(m_coreLevel, 0, 2);
  updateLay->addWidget(m_runtime, 1, 0, 1, 2);
  updateLay->addWidget(m_runtimeLevel, 1, 2);
  updateLay->addWidget(m_overrideLevels, 2, 0, 1, 3);
  updateLay->setColumnStretch(0, 1);

  m_save = new QCheckBox(tr("Со&хранить"), this);
  m_save->setChecked(m_settings->getBool("Save"));
  m_save->setToolTip(tr("Сохранить заданные здесь настройки в качестве\nзначений по умолчанию"));

  m_reset = new QToolButton(this);
  m_reset->setIcon(QIcon(":/images/undo.png"));
  m_reset->setAutoRaise(true);
  m_reset->setToolTip(tr("Вернуть настройки по умолчанию"));
  connect(m_reset, SIGNAL(clicked(bool)), SLOT(reset()));

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(versionGroup, 0, 0, 1, 2);
  mainLay->addWidget(m_mirror, 1, 0, 1, 2);
  mainLay->addWidget(m_save, 2, 0);
  mainLay->addWidget(m_reset, 2, 1);

  connect(m_core, SIGNAL(clicked(bool)), SLOT(clickedCore(bool)));
  connect(m_runtime, SIGNAL(clicked(bool)), m_runtimeLevel, SLOT(setEnabled(bool)));

  if (m_mirror->isChecked())
    clickedCore(m_core->isChecked());
}


bool SelectPage::validatePage()
{
  qDebug() << "SelectPage::validatePage()";

  if (m_version->text().isEmpty())
    return false;

  m_settings->setString("Version",      m_version->text());
  m_settings->setString("Suffix",       m_suffix->text());
  m_settings->setBool("Mirror",         m_mirror->isChecked());
  m_settings->setBool("MirrorCore",     m_core->isChecked());
  m_settings->setBool("MirrorQt",       m_runtime->isChecked());
  m_settings->setBool("OverrideLevels", m_overrideLevels->isChecked());
  m_settings->setBool("Save",           m_save->isChecked());
  m_settings->setInt("LevelCore",       m_coreLevel->value());
  m_settings->setInt("LevelQt",         m_runtimeLevel->value());

  return true;
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


/*!
 * Восстанавливает значения по умолчанию.
 * Вызывается кликом по кнопке \a m_reset.
 */
void SelectPage::reset()
{
  m_version->setText(WizardSettings::version());
  m_suffix->setText("");
  m_mirror->setChecked(false);
  m_core->setChecked(true);
  m_runtime->setChecked(true);
  m_overrideLevels->setChecked(false);
  m_save->setChecked(false);
  m_coreLevel->setValue(UpdateLevelCore);
  m_runtimeLevel->setValue(UpdateLevelQt);
}
