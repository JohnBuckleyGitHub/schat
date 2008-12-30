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

#include "nsispage.h"
#include "wizardsettings.h"

/*!
 * \brief Конструктор класса NsisPage.
 */
NsisPage::NsisPage(QWidget *parent)
  : QWizardPage(parent)
{
  m_settings = settings;

  setTitle(tr("Дополнительные настройки инсталлятора"));
  setSubTitle(tr("Переопределение дополнительных настроек инсталлятора"));
  setCommitPage(true);

  m_desktop = new QCheckBox(tr("&Рабочий стол"), this);
  m_desktop->setChecked(m_settings->getBool("NsisDesktop"));
  m_desktop->setToolTip(tr("Создать ярлык на рабочем столе"));

  m_quickLaunch = new QCheckBox(tr("&Быстрый запуск"), this);
  m_quickLaunch->setChecked(m_settings->getBool("NsisQuickLaunch"));
  m_quickLaunch->setToolTip(tr("Создать ярлык в быстром запуске"));

  m_allPrograms = new QCheckBox(tr("&Меню \"Все программы\""), this);
  m_allPrograms->setChecked(m_settings->getBool("NsisAllPrograms"));
  m_allPrograms->setToolTip(tr("Создать группу в меню \"Все программы\""));

  m_autostart = new QCheckBox(tr("Добавить в &автозагрузку"), this);
  m_autostart->setChecked(m_settings->getBool("NsisAutostart"));
  m_autostart->setToolTip(tr("Добавить чат в автозагрузку"));

  m_autostartDaemon = new QCheckBox(tr("Добавить &сервер в автозагрузку"), this);
  m_autostartDaemon->setChecked(m_settings->getBool("NsisAutostartDaemon"));
  m_autostartDaemon->setToolTip(tr("Добавить сервер в автозагрузку,\nбудет доступно только если при\nустановке выбран сервер"));

  QGroupBox *group = new QGroupBox(tr("Опции инсталлятора"), this);
  QVBoxLayout *groupLay = new QVBoxLayout(group);
  groupLay->addWidget(m_desktop);
  groupLay->addWidget(m_quickLaunch);
  groupLay->addWidget(m_allPrograms);
  groupLay->addWidget(m_autostart);
  groupLay->addWidget(m_autostartDaemon);

  m_nsis = new QLineEdit(m_settings->getString("MakensisFile"), this);
  m_button = new QPushButton(tr("Обзор..."), this);

  QGroupBox *nsisGroup = new QGroupBox(tr("Путь к NSIS"), this);
  QHBoxLayout *nsisLay = new QHBoxLayout(nsisGroup);
  nsisLay->addWidget(m_nsis);
  nsisLay->addWidget(m_button);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(group);
  mainLay->addWidget(nsisGroup);
  mainLay->addStretch();

  connect(m_button, SIGNAL(clicked(bool)), SLOT(getOpenFileName()));
}


/*!
 * Проверка страницы и в случае успеха сохранение настроек.
 */
bool NsisPage::validatePage()
{
  if (!QFile::exists(m_nsis->text()))
    return false;

  m_settings->setString("MakensisFile",      m_nsis->text());
  m_settings->setBool("NsisDesktop",         m_desktop->isChecked());
  m_settings->setBool("NsisQuickLaunch",     m_quickLaunch->isChecked());
  m_settings->setBool("NsisAllPrograms",     m_allPrograms->isChecked());
  m_settings->setBool("NsisAutostart",       m_autostart->isChecked());
  m_settings->setBool("NsisAutostartDaemon", m_autostartDaemon->isChecked());

  return true;
}


/*!
 * Открывает диалог открытия файла, для указания пути к \b makensis.exe.
 */
void NsisPage::getOpenFileName()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Путь к компилятору NSIS"), m_nsis->text(), tr("Исполняемые файлы (*.exe)"));

  if (!fileName.isEmpty())
    m_nsis->setText(QDir::toNativeSeparators(fileName));
}
