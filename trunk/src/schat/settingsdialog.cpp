/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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

#include "networkwidget.h"
#include "profilewidget.h"
#include "schatwindow.h"
#include "settings.h"
#include "settingsdialog.h"
#include "abstractprofile.h"

/*!
 * \brief Конструктор класса SettingsDialog.
 */
SettingsDialog::SettingsDialog(AbstractProfile *profile, Settings *settings, QWidget *parent)
  : QDialog(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

  m_okButton       = new QPushButton(QIcon(":/images/ok.png"), tr("OK"), this);
  m_cancelButton   = new QPushButton(QIcon(":/images/cancel.png"), tr("Отмена"), this);
  m_resetButton    = new QPushButton(QIcon(":/images/undo.png"), "", this);
  m_resetButton->setToolTip(tr("Вернуть настройки по умолчанию"));
  m_contentsWidget = new QListWidget(this);
  m_pagesWidget    = new QStackedWidget(this);

  m_profilePage    = new ProfileSettings(settings, profile, this); 
  m_networkPage    = new NetworkSettings(settings, this);
  m_interfacePage  = new InterfaceSettings(settings, this);
  m_emoticonsPage  = new EmoticonsSettings(settings, this);

  createPage(QIcon(":/images/profile.png"), tr("Личные данные"), m_profilePage);
  createPage(QIcon(":/images/network.png"), tr("Сеть"), m_networkPage);
  createPage(QIcon(":/images/appearance.png"), tr("Интерфейс"), m_interfacePage);
  createPage(QIcon(":/images/emoticon.png"), tr("Смайлики"), m_emoticonsPage);

  #ifdef SCHAT_UPDATE
  m_updatePage = new UpdateSettings(settings, this);
  createPage(QIcon(":/images/update.png"), tr("Обновления"), m_updatePage);
  #endif

  QFrame *line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  connect(m_contentsWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
  connect(m_okButton, SIGNAL(clicked()), SLOT(accept()));
  connect(m_cancelButton, SIGNAL(clicked()), SLOT(close()));
  connect(m_resetButton, SIGNAL(clicked()), SLOT(reset()));
  connect(m_profilePage, SIGNAL(validNick(bool)), SLOT(validNick(bool)));

  m_contentsWidget->setCurrentRow(ProfilePage);

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch();
  buttonLayout->addWidget(m_resetButton);
  buttonLayout->addWidget(m_okButton);
  buttonLayout->addWidget(m_cancelButton);
  buttonLayout->setSpacing(3);

  QGridLayout *mainLayout = new QGridLayout(this);
  mainLayout->setColumnStretch(0, 1);
  mainLayout->setColumnStretch(1, 3);
  mainLayout->addWidget(m_contentsWidget, 0, 0);
  mainLayout->addWidget(m_pagesWidget, 0, 1);
  mainLayout->addWidget(line, 1, 0, 1, 2);
  mainLayout->addLayout(buttonLayout, 2, 0, 1, 2);
  mainLayout->setMargin(3);
  mainLayout->setSpacing(3);

  setWindowTitle(tr("Настройка"));
}


void SettingsDialog::setPage(int page)
{
  m_contentsWidget->setCurrentRow(page);
  m_pagesWidget->setCurrentIndex(page);
}


void SettingsDialog::accept()
{
  emit save();
  close();
}


void SettingsDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
  if (!current)
    current = previous;

  m_pagesWidget->setCurrentIndex(m_contentsWidget->row(current));
}


void SettingsDialog::reset()
{
  emit reset(m_pagesWidget->currentIndex());
}


void SettingsDialog::createPage(const QIcon &icon, const QString &text, AbstractSettingsPage *page)
{
  m_pagesWidget->addWidget(page);
  new QListWidgetItem(icon, text, m_contentsWidget);
  connect(this, SIGNAL(save()), page, SLOT(save()));
  connect(this, SIGNAL(reset(int)), page, SLOT(reset(int)));
}



/*!
 * \brief Конструктор класса AbstractSettingsPage.
 */
AbstractSettingsPage::AbstractSettingsPage(SettingsDialog::Page id, Settings *settings, QWidget *parent)
  : QWidget(parent)
{
  m_id = id;
  m_settings = settings;
}



/*!
 * \brief Конструктор класса ProfileSettings.
 */
ProfileSettings::ProfileSettings(Settings *settings, AbstractProfile *profile, QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::ProfilePage, settings, parent)
{
  m_profile  = profile;
  m_profileWidget = new ProfileWidget(profile, this);
  connect(m_profileWidget, SIGNAL(validNick(bool)), this, SIGNAL(validNick(bool)));

  QLabel *byeMsgLabel = new QLabel(tr("Сообщение при выходе"), this);
  m_byeMsgEdit = new QLineEdit(profile->byeMsg(), this);
  m_byeMsgEdit->setMaxLength(AbstractProfile::MaxByeMsgLength);
  m_byeMsgEdit->setToolTip(tr("Сообщение которое увидят другие пользователи если вы выйдете из чата"));

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
NetworkSettings::NetworkSettings(Settings *settings, QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::NetworkPage, settings, parent)
{  
  m_welcomeCheckBox = new QCheckBox(tr("Всегда использовать этот сервер"), this);
  m_welcomeCheckBox->setChecked(m_settings->getBool("HideWelcome"));
  m_welcomeCheckBox->setToolTip(tr("Не запрашивать персональную информацию и адрес сервера при запуске программы"));

  m_networkWidget = new NetworkWidget(m_settings, this);

  QHBoxLayout *networkLayout = new QHBoxLayout;
  networkLayout->addWidget(m_networkWidget);
  networkLayout->setMargin(0);

  QGroupBox *serverGroupBox = new QGroupBox(tr("Сервер"), this);
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
InterfaceSettings::InterfaceSettings(Settings *settings, QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::InterfacePage, settings, parent)
{  
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
EmoticonsSettings::EmoticonsSettings(Settings *settings, QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::EmoticonsPage, settings, parent)
{
  m_themeCombo = new QComboBox(this);

  m_themeGroup = new QGroupBox(tr("Тема смайликов"), this);
  QHBoxLayout *themeGroupLayout = new QHBoxLayout(m_themeGroup);
  themeGroupLayout->addWidget(m_themeCombo);
  themeGroupLayout->addStretch();

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(m_themeGroup);

  m_enableCheck = new QCheckBox(tr("Включить смайлики"), this);
  m_enableCheck->setChecked(m_settings->getBool("UseEmoticons"));
  connect(m_enableCheck, SIGNAL(clicked(bool)), SLOT(enable(bool)));

  m_animateCheck = new QCheckBox(tr("Разрешить анимацию"), this);
  m_animateCheck->setChecked(m_settings->getBool("UseAnimatedEmoticons"));

  m_requireSpacesCheck = new QCheckBox(tr("Смайлики отделены пробелами"), this);
  m_requireSpacesCheck->setChecked(m_settings->getBool("EmoticonsRequireSpaces"));

  mainLayout->addWidget(m_enableCheck);
  mainLayout->addWidget(m_animateCheck);
  mainLayout->addWidget(m_requireSpacesCheck);
  mainLayout->addStretch();

  enable(m_enableCheck->isChecked());
}


void EmoticonsSettings::reset(int page)
{
  if (page == m_id) {
    m_enableCheck->setChecked(true);
    m_animateCheck->setChecked(true);
    m_requireSpacesCheck->setChecked(true);
    enable(true);
  }
}


void EmoticonsSettings::save()
{
  m_settings->setBool("UseEmoticons", m_enableCheck->isChecked());
  m_settings->setBool("UseAnimatedEmoticons", m_animateCheck->isChecked());
  m_settings->setBool("EmoticonsRequireSpaces", m_requireSpacesCheck->isChecked());
}


void EmoticonsSettings::enable(bool checked)
{
  m_themeGroup->setEnabled(checked);
  m_animateCheck->setEnabled(checked);
  m_requireSpacesCheck->setEnabled(checked);
}




/*!
 * \brief Конструктор класса UpdateSettings.
 */
#ifdef SCHAT_UPDATE
UpdateSettings::UpdateSettings(Settings *settings, QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::UpdatePage, settings, parent)
{  
  m_autoDownload = new QCheckBox(tr("Автоматически загружать обновления"), this);
  m_autoDownload->setChecked(m_settings->getBool("Updates/AutoDownload"));
  m_autoDownload->setEnabled(false);

  m_autoClean = new QCheckBox(tr("Удалять обновления после установки"), this);
  m_autoClean->setChecked(m_settings->getBool("Updates/AutoClean"));

  QLabel *interval = new QLabel(tr("Интервал проверки обновлений:"), this);
  QHBoxLayout *intervalLayout = new QHBoxLayout;

  m_interval = new QSpinBox(this);
  m_interval->setValue(m_settings->getInt("Updates/CheckInterval"));
  m_interval->setRange(5, 1440);
  m_interval->setSuffix(tr(" мин"));

  QGroupBox *styleGroupBox = new QGroupBox(tr("Автоматические обновления"), this);
  QVBoxLayout *styleGroupLayout = new QVBoxLayout(styleGroupBox);
  styleGroupLayout->addWidget(m_autoDownload);
  styleGroupLayout->addWidget(m_autoClean);
  intervalLayout->addWidget(interval);
  intervalLayout->addWidget(m_interval);
  intervalLayout->addStretch();
  styleGroupLayout->addLayout(intervalLayout);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(styleGroupBox);
  mainLayout->addStretch();
}


/** [UpdateSettings/public]
 * 
 */
void UpdateSettings::reset(int page)
{
  if (page == m_id) {
    m_autoDownload->setChecked(true);
    m_autoClean->setChecked(true);
    m_interval->setValue(60);
  }
}




/** [UpdateSettings/public]
 * Сохраняем настройки
 */
void UpdateSettings::save()
{
  m_settings->setBool("Updates/AutoDownload", m_autoDownload->isChecked());
  m_settings->setBool("Updates/AutoClean", m_autoClean->isChecked());
  m_settings->setInt("Updates/CheckInterval", m_interval->value());
  m_settings->notify(Settings::UpdateSettingsChanged);
}
#endif
