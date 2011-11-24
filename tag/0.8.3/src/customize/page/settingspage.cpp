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

  setTitle(tr("Overriding default settings"));
  setSubTitle(tr("Select options to override the default settings"));

  m_overrideNetwork = new QLabel(tr("Network:"), this);
  m_network = new QComboBox(this);

  m_overrideEmoticons = new QCheckBox(tr("Emoticons:"), this);
  m_overrideEmoticons->setChecked(m_settings->getBool("OverrideEmoticons"));
  m_emoticons = new QComboBox(this);

  m_overrideMirror = new QCheckBox(tr("Mirror updates:"), this);
  m_overrideMirror->setChecked(m_settings->getBool("OverrideMirror"));
  m_mirror = new QLineEdit(m_settings->getString("MirrorUrl"), this);

  m_autoDownload = new QCheckBox(tr("Automatically download updates"), this);
  m_autoDownload->setChecked(m_settings->getBool("AutoDownloadUpdates"));

  QGroupBox *group = new QGroupBox(tr("Main settings"), this);
  QGridLayout *groupLay = new QGridLayout(group);
  groupLay->addWidget(m_overrideNetwork, 0, 0);
  groupLay->addWidget(m_network, 0, 1);
  groupLay->addWidget(m_overrideEmoticons, 1, 0);
  groupLay->addWidget(m_emoticons, 1, 1);
  groupLay->addWidget(m_overrideMirror, 2, 0);
  groupLay->addWidget(m_mirror, 2, 1, 1, 2);
  groupLay->addWidget(m_autoDownload, 3, 0, 1, 3);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(group);
  mainLay->addStretch();

  connect(m_overrideNetwork,   SIGNAL(clicked(bool)), m_network, SLOT(setEnabled(bool)));
  connect(m_overrideEmoticons, SIGNAL(clicked(bool)), m_emoticons, SLOT(setEnabled(bool)));
  connect(m_overrideMirror,    SIGNAL(clicked(bool)), m_mirror, SLOT(setEnabled(bool)));

  m_emoticons->setEnabled(m_overrideEmoticons->isChecked());
  m_mirror->setEnabled(m_overrideMirror->isChecked());

  networkList();
  emoticonsList();
}


/*!
 * Проверка страницы и в случае успеха сохранение настроек.
 */
bool SettingsPage::validatePage()
{
  if (m_network->currentIndex() == -1 || m_network->currentText() == "SimpleNet.xml") {
    QMessageBox::critical(this, tr("No network is selected"), tr("Need to create your network file and select it on this page of the wizard.") +
        "<br /><a href='http://impomezia.com/node/142'>" + tr("More information") + "</a>.");
    return false;
  }

  if (m_overrideEmoticons->isChecked() && m_emoticons->currentIndex() == -1)
    return false;

  if (m_overrideMirror->isChecked() && m_mirror->text().isEmpty())
    return false;

  m_settings->setBool("OverrideNetwork",     true);
  m_settings->setBool("OverrideEmoticons",   m_overrideEmoticons->isChecked());
  m_settings->setBool("OverrideMirror",      m_overrideMirror->isChecked());
  m_settings->setBool("AutoDownloadUpdates", m_autoDownload->isChecked());
  m_settings->setString("Network",           m_network->currentText());
  m_settings->setString("Emoticons",         m_emoticons->currentText());
  m_settings->setString("MirrorUrl",         m_mirror->text());

  return true;
}


/*!
 * Создаёт список тем смайликов.
 */
void SettingsPage::emoticonsList()
{
  QString emoticonsPath = QApplication::applicationDirPath() + "/emoticons/";
  QDir dir(emoticonsPath);
  QStringList list = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

  if (!list.isEmpty()) {

    foreach (QString theme, list) {
      if (QFile::exists(emoticonsPath + theme + "/icondef.xml") || QFile::exists(emoticonsPath + theme + "/emoticons.xml"))
        m_emoticons->addItem(theme);
    }
  }

  if (m_emoticons->count())
    m_emoticons->setCurrentIndex(m_emoticons->findText(m_settings->getString("Emoticons")));
}


/*!
 * Заполняет список файлов сети.
 */
void SettingsPage::networkList()
{
  QDir dir(QApplication::applicationDirPath() + "/networks");
  QStringList list = dir.entryList(QStringList("*.xml"), QDir::Files);
  if (!list.isEmpty())
    m_network->addItems(list);

  if (m_network->count())
    m_network->setCurrentIndex(m_network->findText(m_settings->getString("Network")));
}
