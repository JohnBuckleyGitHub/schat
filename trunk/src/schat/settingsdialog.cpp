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
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui>

#include "abstractprofile.h"
#include "emoticonsreader.h"
#include "icondefreader.h"
#include "profilewidget.h"
#include "schatwindow.h"
#include "settings.h"
#include "settingsdialog.h"
#include "widget/networkwidget.h"

/*!
 * \brief Конструктор класса SettingsDialog.
 */
SettingsDialog::SettingsDialog(AbstractProfile *profile, QWidget *parent)
  : AbstractSettingsDialog(parent)
{
  m_settings       = settings;
  m_profilePage    = new ProfileSettings(profile, this);
  m_networkPage    = new NetworkSettings(this);
  m_interfacePage  = new InterfaceSettings(this);
  m_emoticonsPage  = new EmoticonsSettings(this);
  m_updatePage     = new UpdateSettings(this);

  createPage(QIcon(":/images/profile.png"), tr("Личные данные"), m_profilePage);
  createPage(QIcon(":/images/network.png"), tr("Сеть"), m_networkPage);
  createPage(QIcon(":/images/appearance.png"), tr("Интерфейс"), m_interfacePage);
  createPage(QIcon(":/images/emoticon.png"), tr("Смайлики"), m_emoticonsPage);
  createPage(QIcon(":/images/update.png"), tr("Обновление"), m_updatePage);

  connect(m_profilePage, SIGNAL(validNick(bool)), m_okButton, SLOT(setEnabled(bool)));
  connect(m_networkPage, SIGNAL(validServer(bool)), m_okButton, SLOT(setEnabled(bool)));
}


void SettingsDialog::accept()
{
  emit save();
  m_settings->write();
  close();
}


/*!
 * \brief Конструктор класса ProfileSettings.
 */
ProfileSettings::ProfileSettings(AbstractProfile *profile, QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::ProfilePage, parent)
{
  m_settings = settings;
  m_profile  = profile;
  m_profileWidget = new ProfileWidget(profile, this);
  connect(m_profileWidget, SIGNAL(validNick(bool)), SIGNAL(validNick(bool)));

  QLabel *byeMsgLabel = new QLabel(tr("Сообщение при выходе"), this);
  m_byeMsgEdit = new QLineEdit(profile->byeMsg(), this);
  m_byeMsgEdit->setMaxLength(AbstractProfile::MaxByeMsgLength);
  m_byeMsgEdit->setToolTip(tr("Сообщение которое увидят другие пользователи\nесли вы выйдете из чата"));

  QHBoxLayout *byeMsgLayout = new QHBoxLayout;
  byeMsgLayout->addWidget(byeMsgLabel);
  byeMsgLayout->addWidget(m_byeMsgEdit);

  QGroupBox *profileGroupBox = new QGroupBox(tr("Профиль"), this);
  QVBoxLayout *profileGroupLayout = new QVBoxLayout(profileGroupBox);
  profileGroupLayout->addWidget(m_profileWidget);
  profileGroupLayout->addLayout(byeMsgLayout);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(profileGroupBox);
  mainLayout->addStretch();
}


/** [ProfileSettings/public]
 *
 */
void ProfileSettings::reset(int page)
{
  if (page == m_id) {
    m_profileWidget->reset();
    m_byeMsgEdit->setText("IMPOMEZIA Simple Chat");
  }
}


/** [ProfileSettings/public]
 * Сохраняем настройки
 */
void ProfileSettings::save()
{
  m_profileWidget->save();

  if (m_profileWidget->isModifiled())
    m_settings->notify(Settings::ProfileSettingsChanged);

  if (m_profile->byeMsg() != m_byeMsgEdit->text()) {
    m_profile->setByeMsg(m_byeMsgEdit->text());
    m_settings->notify(Settings::ByeMsgChanged);
  }
}




/*!
 * \brief Конструктор класса NetworkSettings.
 */
NetworkSettings::NetworkSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::NetworkPage, parent)
{
  m_settings = settings;
  m_welcomeCheckBox = new QCheckBox(tr("Всегда использовать этот сервер"), this);
  m_welcomeCheckBox->setChecked(m_settings->getBool("HideWelcome"));
  m_welcomeCheckBox->setToolTip(tr("Не запрашивать персональную информацию\nи адрес сервера при запуске программы"));

  m_networkWidget = new NetworkWidget(this);
  connect(m_networkWidget, SIGNAL(validServer(bool)), SIGNAL(validServer(bool)));

  QHBoxLayout *networkLayout = new QHBoxLayout;
  networkLayout->addWidget(m_networkWidget);
  networkLayout->setMargin(0);

  QGroupBox *serverGroupBox = new QGroupBox(tr("Подключение"), this);
  QVBoxLayout *serverGroupLayout = new QVBoxLayout(serverGroupBox);
  serverGroupLayout->addLayout(networkLayout);
  serverGroupLayout->addWidget(m_welcomeCheckBox);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(serverGroupBox);
  mainLayout->addStretch();
}


/** [NetworkSettings/public]
 *
 */
void NetworkSettings::reset(int page)
{
  if (page == m_id) {
    m_networkWidget->reset();
    m_welcomeCheckBox->setChecked(true);
  }
}


/** [NetworkSettings/public]
 * Сохраняем настройки
 */
void NetworkSettings::save()
{
  if (m_networkWidget->save())
    m_settings->notify(Settings::NetworkSettingsChanged);

  m_settings->setBool("HideWelcome", m_welcomeCheckBox->isChecked());
}




/*!
 * \brief Конструктор класса InterfaceSettings.
 */
InterfaceSettings::InterfaceSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::InterfacePage, parent)
{
  m_settings = settings;
  m_styleComboBox = new QComboBox(this);
  m_styleComboBox->addItems(QStyleFactory::keys());
  m_styleComboBox->setCurrentIndex(m_styleComboBox->findText(m_settings->getString("Style")));

  QGroupBox *styleGroupBox = new QGroupBox(tr("Внешний вид"), this);
  QHBoxLayout *styleGroupLayout = new QHBoxLayout(styleGroupBox);
  styleGroupLayout->addWidget(m_styleComboBox);
  styleGroupLayout->addStretch();

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(styleGroupBox);
  mainLayout->addStretch();
}


/** [InterfaceSettings/public]
 *
 */
void InterfaceSettings::reset(int page)
{
  if (page == m_id) {
    m_styleComboBox->setCurrentIndex(m_styleComboBox->findText("Plastique"));
  }
}


/** [InterfaceSettings/public]
 * Сохраняем настройки
 */
void InterfaceSettings::save()
{
  if (m_styleComboBox->currentIndex() != -1) {
    m_settings->setString("Style", m_styleComboBox->currentText()) ;
    qApp->setStyle(m_settings->getString("Style"));
  }
}




/*!
 * \brief Конструктор класса EmoticonsSettings.
 */
EmoticonsSettings::EmoticonsSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::EmoticonsPage, parent)
{
  m_settings   = settings;
  m_themeCombo = new QComboBox(this);

  m_themeGroup = new QGroupBox(tr("Тема смайликов"), this);
  QHBoxLayout *themeGroupLayout = new QHBoxLayout(m_themeGroup);
  themeGroupLayout->addWidget(m_themeCombo);
  themeGroupLayout->addStretch();

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(m_themeGroup);

  m_enableCheck = new QCheckBox(tr("Включить смайлики"), this);
  m_enableCheck->setChecked(m_settings->getBool("UseEmoticons"));
  m_enableCheck->setToolTip(tr("Включает использование графических смайликов"));
  connect(m_enableCheck, SIGNAL(clicked(bool)), SLOT(enable(bool)));

  m_animateCheck = new QCheckBox(tr("Разрешить анимацию"), this);
  m_animateCheck->setToolTip(tr("Разрешить поддержку анимации в смайликах,\nможет приводить к повышенной загрузке процессора"));
  m_animateCheck->setChecked(m_settings->getBool("UseAnimatedEmoticons"));

  m_requireSpacesCheck = new QCheckBox(tr("Смайлики отделены пробелами"), this);
  m_requireSpacesCheck->setToolTip(tr("Показывать смайлики только если они\nотделены пробелами от остального сообщения"));
  m_requireSpacesCheck->setChecked(m_settings->getBool("EmoticonsRequireSpaces"));

  mainLayout->addWidget(m_enableCheck);
  mainLayout->addWidget(m_animateCheck);
  mainLayout->addWidget(m_requireSpacesCheck);
  mainLayout->addStretch();

  if (!createThemeList()) {
    m_enableCheck->setEnabled(false);
    m_enableCheck->setChecked(false);
    m_settings->setBool("UseEmoticons", false);
  }

  enable(m_enableCheck->isChecked());
}


void EmoticonsSettings::reset(int page)
{
  if (page == m_id) {
    m_enableCheck->setChecked(true);
    m_animateCheck->setChecked(false);
    m_requireSpacesCheck->setChecked(true);
    m_themeCombo->setCurrentIndex(m_themeCombo->findText("Kolobok"));
    enable(true);
  }
}


void EmoticonsSettings::save()
{
  m_settings->setBool("UseEmoticons", m_enableCheck->isChecked());
  m_settings->setBool("UseAnimatedEmoticons", m_animateCheck->isChecked());
  m_settings->setBool("EmoticonsRequireSpaces", m_requireSpacesCheck->isChecked());
  m_settings->setString("EmoticonTheme", m_themeCombo->currentText());
  m_settings->createEmoticonsMap();
  m_settings->notify(Settings::EmoticonsChanged);
}


void EmoticonsSettings::enable(bool checked)
{
  m_themeGroup->setEnabled(checked);
  m_animateCheck->setEnabled(checked);
  m_requireSpacesCheck->setEnabled(checked);
}


bool EmoticonsSettings::createThemeList()
{
  QString emoticonsPath = qApp->applicationDirPath() + "/emoticons/";
  QDir dir(emoticonsPath);
  QStringList list = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

  if (list.isEmpty())
    return false;

  foreach (QString theme, list) {
    if (QFile::exists(emoticonsPath + theme + "/icondef.xml")) {
      IconDefReader reader(0);
      if (reader.readFile(emoticonsPath + theme + "/icondef.xml"))
        m_themeCombo->addItem(theme);
    }
    else if (QFile::exists(emoticonsPath + theme + "/emoticons.xml")) {
      EmoticonsReader reader(0);
      if (reader.readFile(emoticonsPath + theme + "/emoticons.xml"))
        m_themeCombo->addItem(theme);
    }
  }
  if (m_themeCombo->count() == -1)
    return false;
  else {
    m_themeCombo->setCurrentIndex(m_themeCombo->findText(m_settings->getString("EmoticonTheme")));
    return true;
  }
}




/*!
 * \brief Конструктор класса UpdateSettings.
 */
UpdateSettings::UpdateSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::UpdatePage, parent)
{
  m_settings = settings;
  m_last = -1;

  // Уведомление о новых версиях.
  m_versionGroup = new QGroupBox(tr("&Уведомление о новых версиях"), this);
  m_versionGroup->setCheckable(true);
  m_versionGroup->setChecked(m_settings->getBool("Updates/Enable"));

  m_checkOnStartup = new QCheckBox(tr("Проверять при &запуске"), this);
  m_checkOnStartup->setToolTip(tr("Проверять обновления при запуске программы"));
  m_checkOnStartup->setChecked(m_settings->getBool("Updates/CheckOnStartup"));

  QLabel *interval = new QLabel(tr("&Интервал проверки:"), this);
  interval->setToolTip(tr("Временной интервал для периодической проверки\nобновлений"));
  QHBoxLayout *intervalLay = new QHBoxLayout;

  m_interval = new QSpinBox(this);
  m_interval->setRange(0, 60);
  connect(m_interval, SIGNAL(valueChanged(int)), SLOT(intervalChanged(int)));
  interval->setBuddy(interval);

  m_factor = new QComboBox(this);
  intervalChanged(m_settings->getInt("Updates/CheckInterval"));
  connect(m_factor, SIGNAL(activated(int)), SLOT(factorChanged(int)));

  intervalLay->addWidget(interval);
  intervalLay->addWidget(m_interval);
  intervalLay->addWidget(m_factor);
  intervalLay->addStretch();

  QVBoxLayout *versionLay = new QVBoxLayout(m_versionGroup);
  versionLay->addWidget(m_checkOnStartup);
  versionLay->addLayout(intervalLay);

  // Автоматическое обновление
  #ifndef SCHAT_NO_UPDATE
    QGroupBox *updateGroup = new QGroupBox(tr("&Автоматическое обновление"), this);

    m_autoDownload = new QCheckBox(tr("Автоматически &загружать"), this);
    m_autoDownload->setToolTip(tr("Автоматически загружать обновления"));
    m_autoDownload->setChecked(m_settings->getBool("Updates/AutoDownload"));

    m_autoClean = new QCheckBox(tr("&Удалять после установки"), this);
    m_autoClean->setToolTip(tr("Удалять обновления после установки"));
    m_autoClean->setChecked(m_settings->getBool("Updates/AutoClean"));

    QVBoxLayout *updateLay = new QVBoxLayout(updateGroup);
    updateLay->addWidget(m_autoDownload);
    updateLay->addWidget(m_autoClean);

    connect(m_versionGroup, SIGNAL(toggled(bool)), updateGroup, SLOT(setEnabled(bool)));
    updateGroup->setEnabled(m_versionGroup->isChecked());
  #endif

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_versionGroup);
  #ifndef SCHAT_NO_UPDATE
    mainLay->addWidget(updateGroup);
  #endif
  mainLay->addStretch();
}


void UpdateSettings::reset(int page)
{
  if (page == m_id) {
    m_versionGroup->setChecked(true);
    m_checkOnStartup->setChecked(true);
    m_interval->setValue(m_factor->currentIndex() ? 1 : 60);

    #ifndef SCHAT_NO_UPDATE
      m_autoDownload->setChecked(false);
      m_autoClean->setChecked(true);
    #endif
  }
}


void UpdateSettings::save()
{
  m_settings->setBool("Updates/Enable",         m_versionGroup->isChecked());
  m_settings->setBool("Updates/CheckOnStartup", m_checkOnStartup->isChecked());
  m_settings->setInt("Updates/CheckInterval",   m_factor->currentIndex() ? m_interval->value() * 60 : m_interval->value());
  m_settings->notify(Settings::UpdateSettingsChanged);

  #ifndef SCHAT_NO_UPDATE
    m_settings->setBool("Updates/AutoDownload",   m_autoDownload->isChecked());
    m_settings->setBool("Updates/AutoClean",      m_autoClean->isChecked());
  #endif
}


/*!
 * Изменение отсчёта времени (минуты/часы).
 */
void UpdateSettings::factorChanged(int index)
{
  if (m_last != index)
    if (index)
      m_interval->setValue(1);
    else
      m_interval->setValue(59);

  m_last = index;
}


/*!
 * Обработка изменения значения в \a m_interval.
 */
void UpdateSettings::intervalChanged(int i)
{
  int value = i;
  int index = 0;

  if (m_factor->count() == 0) {
    if (value > 59) {
      value /= 60;
      index = 1;
    }
  }
  else {
    index = m_factor->currentIndex();

    if (index == 0 && value == 60) {
      value = 1;
      index = 1;
    }
    else if (index == 1 && value == 0) {
      value = 59;
      index = 0;
    }
    else if (index == 0 && value < 5)
      value = 5;
    else if (index == 1 && value > 24)
      value = 24;
  }

  QString minute = tr("%n Минуты", "", value);
  QString hour   = tr("%n Часы", "", value);

  m_factor->clear();
  m_factor->addItem(minute);
  m_factor->addItem(hour);
  m_factor->setCurrentIndex(index);
  m_last = index;

  m_interval->setValue(value);
}
