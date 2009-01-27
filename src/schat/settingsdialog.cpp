/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include "abstractprofile.h"
#include "emoticons/emoticons.h"
#include "emoticonsreader.h"
#include "icondefreader.h"
#include "profilewidget.h"
#include "schatwindow.h"
#include "settings.h"
#include "settingsdialog.h"
#include "widget/networkwidget.h"
#include "widget/soundwidget.h"

/*!
 * \brief Конструктор класса SettingsDialog.
 */
SettingsDialog::SettingsDialog(AbstractProfile *profile, QWidget *parent)
  : AbstractSettingsDialog(parent)
{
  m_settings = settings;
  ProfileSettings *profilePage = new ProfileSettings(profile, this);
  NetworkSettings *networkPage = new NetworkSettings(this);

  createPage(QIcon(":/images/profile.png"),               tr("Личные данные"), profilePage);
  createPage(QIcon(":/images/applications-internet.png"), tr("Сеть"),          networkPage);
  createPage(QIcon(":/images/applications-graphics.png"), tr("Интерфейс"),     new InterfaceSettings(this));
  createPage(QIcon(":/images/emoticon.png"),              tr("Смайлики"),      new EmoticonsSettings(this));
  createPage(QIcon(":/images/sound.png"),                 tr("Звуки"),         new SoundSettings(this));
  createPage(QIcon(":/images/update.png"),                tr("Обновление"),    new UpdateSettings(this));
  createPage(QIcon(":/images/application-x-desktop.png"), tr("Разное"),        new MiscSettings(this));

  connect(profilePage, SIGNAL(validNick(bool)), m_okButton, SLOT(setEnabled(bool)));
  connect(networkPage, SIGNAL(validServer(bool)), m_okButton, SLOT(setEnabled(bool)));
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

  QHBoxLayout *byeMsgLay = new QHBoxLayout;
  byeMsgLay->addWidget(byeMsgLabel);
  byeMsgLay->addWidget(m_byeMsgEdit);

  QGroupBox *profileGroupBox = new QGroupBox(tr("Профиль"), this);
  QVBoxLayout *profileGroupLay = new QVBoxLayout(profileGroupBox);
  profileGroupLay->addWidget(m_profileWidget);
  profileGroupLay->addLayout(byeMsgLay);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(profileGroupBox);
  mainLay->addStretch();
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

  QGroupBox *serverGroupBox = new QGroupBox(tr("Подключение"), this);
  QVBoxLayout *serverGroupLay = new QVBoxLayout(serverGroupBox);
  serverGroupLay->addWidget(m_networkWidget);
  serverGroupLay->addWidget(m_welcomeCheckBox);

  QLabel *url = new QLabel(QString("<a style='text-decoration:none; color:#1a4d82;' href='#'>%1</a>").arg(tr("Файлы сети")), this);
  url->setToolTip(tr("Открыть папку с файлами сети"));
  url->setAlignment(Qt::AlignRight);
  connect(url,  SIGNAL(linkActivated(const QString &)), SLOT(openFolder()));

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(serverGroupBox);
  mainLay->addStretch();
  mainLay->addWidget(url);
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


void NetworkSettings::openFolder()
{
  QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/networks"));
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
  QHBoxLayout *styleGroupLay = new QHBoxLayout(styleGroupBox);
  styleGroupLay->addWidget(m_styleComboBox);
  styleGroupLay->addStretch();

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(styleGroupBox);
  mainLay->addStretch();
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
  QHBoxLayout *themeGroupLay = new QHBoxLayout(m_themeGroup);
  themeGroupLay->addWidget(m_themeCombo);
  themeGroupLay->addStretch();

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_themeGroup);

  m_enableCheck = new QCheckBox(tr("Включить смайлики"), this);
  m_enableCheck->setChecked(m_settings->getBool("UseEmoticons"));
  m_enableCheck->setToolTip(tr("Включает использование графических смайликов"));
  connect(m_enableCheck, SIGNAL(clicked(bool)), SLOT(enable(bool)));

  m_requireSpacesCheck = new QCheckBox(tr("Смайлики отделены пробелами"), this);
  m_requireSpacesCheck->setToolTip(tr("Показывать смайлики только если они\nотделены пробелами от остального сообщения"));
  m_requireSpacesCheck->setChecked(m_settings->getBool("EmoticonsRequireSpaces"));

  QLabel *url = new QLabel(QString("<a style='text-decoration:none; color:#1a4d82;' href='#'>%1</a>").arg(tr("Темы смайликов")), this);
  url->setToolTip(tr("Открыть папку с темами смайликов"));
  url->setAlignment(Qt::AlignRight);
  connect(url,  SIGNAL(linkActivated(const QString &)), SLOT(openFolder()));

  mainLay->addWidget(m_enableCheck);
  mainLay->addWidget(m_requireSpacesCheck);
  mainLay->addStretch();
  mainLay->addWidget(url);

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
    m_requireSpacesCheck->setChecked(true);
    m_themeCombo->setCurrentIndex(m_themeCombo->findText("Kolobok"));
    enable(true);
  }
}


void EmoticonsSettings::save()
{
  m_settings->setBool("UseEmoticons", m_enableCheck->isChecked());
  m_settings->setBool("EmoticonsRequireSpaces", m_requireSpacesCheck->isChecked());
  m_settings->setString("EmoticonTheme", m_themeCombo->currentText());
  m_settings->notify(Settings::EmoticonsChanged);
}


void EmoticonsSettings::enable(bool checked)
{
  m_themeGroup->setEnabled(checked);
  m_requireSpacesCheck->setEnabled(checked);
}


void EmoticonsSettings::openFolder()
{
  QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/emoticons"));
}


bool EmoticonsSettings::createThemeList()
{
  m_themeCombo->addItems(Emoticons::themeList());

  if (m_themeCombo->count() == -1)
    return false;
  else {
    m_themeCombo->setCurrentIndex(m_themeCombo->findText(m_settings->getString("EmoticonTheme")));
    return true;
  }
}




/*!
 * \brief Конструктор класса SoundSettings.
 */
SoundSettings::SoundSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::SoundPage, parent)
{
  m_settings = settings;

  m_enable = new QGroupBox(tr("&Звуковые уведомления"), this);
  m_enable->setCheckable(true);
  m_enable->setChecked(m_settings->getBool("Sound"));

  QDir dir(QApplication::applicationDirPath() + "/sounds");
  QStringList list = dir.entryList(m_settings->getList("Sound/NameFilter"), QDir::Files);

  m_message = new SoundWidget("Message", tr("Сообщение"), tr("Сообщение в основной канал"), list, this);
  connect(m_message, SIGNAL(play(const QString &)), SLOT(play(const QString &)));
  m_private = new SoundWidget("PrivateMessage", tr("Приватное сообщение"), tr("Сообщение в приват от другого пользователя"), list, this);
  connect(m_private, SIGNAL(play(const QString &)), SLOT(play(const QString &)));

  QVBoxLayout *soundLay = new QVBoxLayout(m_enable);
  soundLay->addWidget(m_message);
  soundLay->addWidget(m_private);
  soundLay->setSpacing(0);

  #ifdef Q_WS_X11
    m_useCmd = new QGroupBox(tr("Внешняя команда для воспроизведения звука"), this);
    m_useCmd->setCheckable(true);
    m_useCmd->setChecked(m_settings->getBool("Sound/UseExternalCmd"));
    m_cmd = new QLineEdit(m_settings->getString("Sound/ExternalCmd"), this);
    m_cmd->setToolTip(tr("Внешняя команда, Вместо %1 подставляется\nимя звукового файла"));
    QVBoxLayout *cmdLay = new QVBoxLayout(m_useCmd);
    cmdLay->addWidget(m_cmd);
  #endif

  QLabel *url = new QLabel(QString("<a style='text-decoration:none; color:#1a4d82;' href='#'>%1</a>").arg(tr("Звуки")), this);
  url->setToolTip(tr("Открыть папку со звуками"));
  url->setAlignment(Qt::AlignRight);
  connect(url, SIGNAL(linkActivated(const QString &)), SLOT(openFolder()));

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_enable);
  #ifdef Q_WS_X11
    mainLay->addWidget(m_useCmd);
  #endif
  mainLay->addStretch();
  mainLay->addWidget(url);
}


void SoundSettings::reset(int page)
{
  if (page == m_id) {
    m_message->reset(true, "Received.wav");
    m_private->reset(true, "Received.wav");

    #ifdef Q_WS_X11
      m_useCmd->setChecked(true);
      m_cmd->setText("aplay -q -N %1");
    #endif
  }
}


void SoundSettings::save()
{
  m_settings->setBool("Sound", m_enable->isChecked());
  m_message->save();
  m_private->save();
  #ifdef Q_WS_X11
    m_settings->setBool("Sound/UseExternalCmd", m_useCmd->isChecked());
    m_settings->setString("Sound/ExternalCmd", m_cmd->text());
  #endif
  m_settings->notify(Settings::SoundChanged);
}


void SoundSettings::openFolder()
{
  QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/sounds"));
}


void SoundSettings::play(const QString &file)
{
  #ifdef Q_WS_X11
  if (m_useCmd->isChecked() && !m_cmd->text().isEmpty())
    QProcess::startDetached(m_cmd->text().arg(file));
  else
  #endif
    QSound::play(file);
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


/*!
 * \brief Конструктор класса MiscSettings.
 */
MiscSettings::MiscSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::MiscPage, parent)
{
  m_settings = settings;

  #ifdef Q_WS_WIN
    QGroupBox *integration = new QGroupBox(tr("Интеграция"), this);

    m_autostart = new QCheckBox(tr("&Автозапуск"), this);
    m_autostart->setToolTip(tr("Автозапуск программы при старте системы"));
    m_autostart->setTristate();
    m_autostart->setCheckState(Qt::PartiallyChecked);

    m_autostartDaemon = new QCheckBox(tr("Автозапуск &сервера"), this);
    m_autostartDaemon->setToolTip(tr("Автозапуск сервера при старте системы"));
    m_autostartDaemon->setTristate();
    m_autostartDaemon->setCheckState(Qt::PartiallyChecked);

    QVBoxLayout *integrationLay = new QVBoxLayout(integration);
    integrationLay->addWidget(m_autostart);
    integrationLay->addWidget(m_autostartDaemon);
  #endif

  QGroupBox *logGroup = new QGroupBox(tr("&Журналирование"), this);

  m_log = new QCheckBox(tr("Журнал &основного канала"), this);
  m_log->setToolTip(tr("Вести журнал основного канала"));
  m_log->setChecked(m_settings->getBool("Log"));

  m_logPrivate = new QCheckBox(tr("Журналы &приватов"), this);
  m_logPrivate->setToolTip(tr("Вести журналы приватных сообщений"));
  m_logPrivate->setChecked(m_settings->getBool("LogPrivate"));

  QVBoxLayout *logLay = new QVBoxLayout(logGroup);
  logLay->addWidget(m_log);
  logLay->addWidget(m_logPrivate);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  #ifdef Q_WS_WIN
    mainLay->addWidget(integration);
  #endif
  mainLay->addWidget(logGroup);
  mainLay->addStretch();

  #ifdef Q_WS_WIN
    autostart();
  #endif
}


void MiscSettings::reset(int page)
{
  if (page == m_id) {
    m_log->setChecked(true);
    m_logPrivate->setChecked(true);
  }
}


void MiscSettings::save()
{
  #ifdef Q_WS_WIN
    writeAutostart();
  #endif

  m_settings->setBool("Log", m_log->isChecked());
  m_settings->setBool("LogPrivate", m_logPrivate->isChecked());
  m_settings->notify(Settings::MiscSettingsChanged);
}


/*!
 * Устанавливает состояние флажков добавления в автозагрузку.
 * Если файл программы управления сервером не найден, то скрываем флажок сервера.
 * Если ключа в реестре не найдено, снимаем флажок.
 */
#ifdef Q_WS_WIN
void MiscSettings::autostart()
{
  QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

  if (QFile::exists(QApplication::applicationDirPath() + "/schatd-ui.exe")) {
    QString value = reg.value(QApplication::applicationName() + " Daemon", "").toString();
    if (value.isEmpty())
      m_autostartDaemon->setChecked(false);
  }
  else
    m_autostartDaemon->setVisible(false);

  QString value = reg.value(QApplication::applicationName(), "").toString();
  if (value.isEmpty())
    m_autostart->setChecked(false);
}


/*!
 * Добавляет/удаляет чат и при необходимости сервер из автозагрузки.
 */
void MiscSettings::writeAutostart()
{
  QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

  if (m_autostart->checkState() == Qt::Checked)
    reg.setValue(QApplication::applicationName(), QDir::toNativeSeparators(QApplication::applicationFilePath()) + " -hide");
  else if (m_autostart->checkState() == Qt::Unchecked)
    reg.remove(QApplication::applicationName());

  if (m_autostartDaemon->isVisible()) {
    if (m_autostart->checkState() == Qt::Checked)
      reg.setValue(QApplication::applicationName() + " Daemon", QDir::toNativeSeparators(QApplication::applicationDirPath() + "/schatd-ui.exe") + " -start");
    else if (m_autostart->checkState() == Qt::Unchecked)
      reg.remove(QApplication::applicationName() + " Daemon");
  }
}
#endif
