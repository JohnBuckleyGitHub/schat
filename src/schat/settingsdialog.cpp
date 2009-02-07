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
#include "chatwindow/chatwindowstyle.h"
#include "emoticons/emoticons.h"
#include "profilewidget.h"
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
  SimpleSettings->write();
  close();
}




class ProfileSettings::Private
{
public:
  Private() {}

  AbstractProfile *profile;
  ProfileWidget *profileWidget;
  QLineEdit *byeMsgEdit;
};


/*!
 * \brief Конструктор класса ProfileSettings.
 */
ProfileSettings::ProfileSettings(AbstractProfile *profile, QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::ProfilePage, parent), d(new Private)
{
  d->profile  = profile;
  d->profileWidget = new ProfileWidget(profile, this);
  connect(d->profileWidget, SIGNAL(validNick(bool)), SIGNAL(validNick(bool)));

  QLabel *byeMsgLabel = new QLabel(tr("Сообщение при выходе"), this);
  d->byeMsgEdit = new QLineEdit(profile->byeMsg(), this);
  d->byeMsgEdit->setMaxLength(AbstractProfile::MaxByeMsgLength);
  d->byeMsgEdit->setToolTip(tr("Сообщение которое увидят другие пользователи\nесли вы выйдете из чата"));

  QHBoxLayout *byeMsgLay = new QHBoxLayout;
  byeMsgLay->addWidget(byeMsgLabel);
  byeMsgLay->addWidget(d->byeMsgEdit);

  QGroupBox *profileGroupBox = new QGroupBox(tr("Профиль"), this);
  QVBoxLayout *profileGroupLay = new QVBoxLayout(profileGroupBox);
  profileGroupLay->addWidget(d->profileWidget);
  profileGroupLay->addLayout(byeMsgLay);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(profileGroupBox);
  mainLay->addStretch();
}

ProfileSettings::~ProfileSettings() { delete d; }


void ProfileSettings::reset(int page)
{
  if (page == m_id) {
    d->profileWidget->reset();
    d->byeMsgEdit->setText(QApplication::applicationName());
  }
}


void ProfileSettings::save()
{
  d->profileWidget->save();

  if (d->profileWidget->isModifiled())
    SimpleSettings->notify(Settings::ProfileSettingsChanged);

  if (d->profile->byeMsg() != d->byeMsgEdit->text()) {
    d->profile->setByeMsg(d->byeMsgEdit->text());
    SimpleSettings->notify(Settings::ByeMsgChanged);
  }
}




class NetworkSettings::Private
{
public:
  Private() {}

  NetworkWidget *network;
  QCheckBox *welcome;
};

/*!
 * \brief Конструктор класса NetworkSettings.
 */
NetworkSettings::NetworkSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::NetworkPage, parent), d(new Private)
{
  d->welcome = new QCheckBox(tr("Всегда использовать этот сервер"), this);
  d->welcome->setChecked(SimpleSettings->getBool("HideWelcome"));
  d->welcome->setToolTip(tr("Не запрашивать персональную информацию\nи адрес сервера при запуске программы"));

  d->network = new NetworkWidget(this);
  connect(d->network, SIGNAL(validServer(bool)), SIGNAL(validServer(bool)));

  QGroupBox *serverGroupBox = new QGroupBox(tr("Подключение"), this);
  QVBoxLayout *serverGroupLay = new QVBoxLayout(serverGroupBox);
  serverGroupLay->addWidget(d->network);
  serverGroupLay->addWidget(d->welcome);

  QLabel *url = new QLabel(QString("<a style='text-decoration:none; color:#1a4d82;' href='#'>%1</a>").arg(tr("Файлы сети")), this);
  url->setToolTip(tr("Открыть папку с файлами сети"));
  url->setAlignment(Qt::AlignRight);
  connect(url, SIGNAL(linkActivated(const QString &)), SLOT(openFolder()));

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(serverGroupBox);
  mainLay->addStretch();
  mainLay->addWidget(url);
}


NetworkSettings::~NetworkSettings() { delete d; }


void NetworkSettings::reset(int page)
{
  if (page == m_id) {
    d->network->reset();
    d->welcome->setChecked(true);
  }
}


void NetworkSettings::save()
{
  if (d->network->save())
    SimpleSettings->notify(Settings::NetworkSettingsChanged);

  SimpleSettings->setBool("HideWelcome", d->welcome->isChecked());
}


void NetworkSettings::openFolder()
{
  QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/networks"));
}




class InterfaceSettings::Private
{
public:
  Private() {}
  void createStylesList();
  void reloadVariants(int index);
  void setStyle();

  QCheckBox *grouping;
  QComboBox *chatStyle;
  QComboBox *chatStyleVariant;
  QComboBox *mainStyle;
};


void InterfaceSettings::Private::createStylesList()
{
  QStringList stylesDirs;
  stylesDirs << (QApplication::applicationDirPath() + "/styles/");
  int index = 0;

  for (int i = 0; i < stylesDirs.count(); ++i) {
    QString dir = stylesDirs.at(i);
    QDir qdir(dir);
    qdir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    qdir.setSorting(QDir::Name);

    QStringList entryList = qdir.entryList();
    if (entryList.contains("Default"))
      entryList.removeAll("Default");

    foreach (QString style, entryList) {
      QString styleDirPath = dir + style + '/';
      if (ChatWindowStyle::isValid(styleDirPath)) {
        index++;
        chatStyle->addItem(style);
        chatStyle->setItemData(index, QStringList(ChatWindowStyle::variants(styleDirPath).keys()), Qt::UserRole + 1);
      }
    }
  }
}


void InterfaceSettings::Private::reloadVariants(int index)
{
  chatStyleVariant->clear();
  chatStyleVariant->addItem(tr("(без вариантов)"));

  if (index > 0) {
    chatStyleVariant->addItems(chatStyle->itemData(index, Qt::UserRole + 1).toStringList());
  }
}


void InterfaceSettings::Private::setStyle()
{
  QString style = SimpleSettings->getString("ChatStyle");
  int index = chatStyle->findText(style);
  if (index != -1) {
    chatStyle->setCurrentIndex(index);
    reloadVariants(index);
    QString styleVariant = SimpleSettings->getString("ChatStyleVariant");
    int variant = chatStyleVariant->findText(styleVariant);
    if (variant != -1)
      chatStyleVariant->setCurrentIndex(variant);
  }
  else
    reloadVariants(0);
}


/*!
 * \brief Конструктор класса InterfaceSettings.
 */
InterfaceSettings::InterfaceSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::InterfacePage, parent), d(new Private)
{
  d->mainStyle = new QComboBox(this);
  d->mainStyle->addItems(QStyleFactory::keys());
  d->mainStyle->setCurrentIndex(d->mainStyle->findText(SimpleSettings->getString("Style")));

  QGroupBox *mainStyleGroup = new QGroupBox(tr("&Глобальный стиль"), this);
  QHBoxLayout *mainStyleLay = new QHBoxLayout(mainStyleGroup);
  mainStyleLay->addWidget(d->mainStyle);
  mainStyleLay->addStretch();

  d->chatStyle = new QComboBox(this);
  d->chatStyle->addItem("Default");
  QLabel *name = new QLabel(tr("&Имя стиля:"), this);
  name->setBuddy(d->chatStyle);

  d->chatStyleVariant = new QComboBox(this);
  QLabel *variant = new QLabel(tr("&Вариант:"), this);
  variant->setBuddy(d->chatStyleVariant);

  d->grouping = new QCheckBox(tr("Группировать идущие &подряд сообщения"), this);
  d->grouping->setToolTip(tr("Группировать идущие подряд сообщения\nот одного пользователя если это\nподдерживается выбранным стилем"));
  d->grouping->setChecked(SimpleSettings->getBool("MessageGrouping"));

  QGroupBox *chatStyleGroup = new QGroupBox(tr("Стиль &текста"), this);
  QGridLayout *chatStyleLay = new QGridLayout(chatStyleGroup);
  chatStyleLay->addWidget(name, 0, 0);
  chatStyleLay->addWidget(d->chatStyle, 0, 1);
  chatStyleLay->addWidget(variant, 1, 0);
  chatStyleLay->addWidget(d->chatStyleVariant, 1, 1);
  chatStyleLay->addWidget(d->grouping, 2, 0, 1, 2);
  chatStyleLay->setColumnStretch(1, 1);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(mainStyleGroup);
  mainLay->addWidget(chatStyleGroup);
  mainLay->addStretch();

  connect(d->chatStyle, SIGNAL(currentIndexChanged(int)), SLOT(reloadVariants(int)));

  d->createStylesList();
  d->setStyle();
}


InterfaceSettings::~InterfaceSettings() { delete d; }


void InterfaceSettings::reset(int page)
{
  if (page == m_id) {
    d->mainStyle->setCurrentIndex(d->mainStyle->findText("Plastique"));
    d->chatStyle->setCurrentIndex(0);
    d->chatStyleVariant->setCurrentIndex(0);
    d->grouping->setChecked(true);
  }
}


void InterfaceSettings::save()
{
  if (d->mainStyle->currentIndex() != -1) {
    SimpleSettings->setString("Style", d->mainStyle->currentText()) ;
    QApplication::setStyle(d->mainStyle->currentText());
  }

  if (d->chatStyle->currentIndex() != -1)
    SimpleSettings->setString("ChatStyle", d->chatStyle->currentText());

  if (d->chatStyleVariant->currentIndex() == 0)
    SimpleSettings->setString("ChatStyleVariant", "");
  else if (d->chatStyleVariant->currentIndex() > 0)
    SimpleSettings->setString("ChatStyleVariant", d->chatStyleVariant->currentText());

  SimpleSettings->setBool("MessageGrouping", d->grouping->isChecked());
  SimpleSettings->notify(Settings::InterfaceSettingsChanged);
}


void InterfaceSettings::reloadVariants(int index)
{
  d->reloadVariants(index);
}




class EmoticonsSettings::Private
{
public:
  Private() {}
  bool createThemeList();

  QCheckBox *enable;
  QCheckBox *requireSpaces;
  QComboBox *combo;
  QGroupBox *group;
};


bool EmoticonsSettings::Private::createThemeList()
{
  combo->addItems(Emoticons::themeList());

  if (combo->count() == -1)
    return false;
  else {
    combo->setCurrentIndex(combo->findText(SimpleSettings->getString("EmoticonTheme")));
    return true;
  }
}


/*!
 * \brief Конструктор класса EmoticonsSettings.
 */
EmoticonsSettings::EmoticonsSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::EmoticonsPage, parent), d(new Private)
{
  d->combo = new QComboBox(this);

  d->group = new QGroupBox(tr("Тема смайликов"), this);
  QHBoxLayout *themeLay = new QHBoxLayout(d->group);
  themeLay->addWidget(d->combo);
  themeLay->addStretch();

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(d->group);

  d->enable = new QCheckBox(tr("Включить смайлики"), this);
  d->enable->setChecked(SimpleSettings->getBool("UseEmoticons"));
  d->enable->setToolTip(tr("Включает использование графических смайликов"));
  connect(d->enable, SIGNAL(clicked(bool)), SLOT(enable(bool)));

  d->requireSpaces = new QCheckBox(tr("Смайлики отделены пробелами"), this);
  d->requireSpaces->setToolTip(tr("Показывать смайлики только если они\nотделены пробелами от остального сообщения"));
  d->requireSpaces->setChecked(Emoticons::strictParse());

  QLabel *url = new QLabel(QString("<a style='text-decoration:none; color:#1a4d82;' href='#'>%1</a>").arg(tr("Темы смайликов")), this);
  url->setToolTip(tr("Открыть папку с темами смайликов"));
  url->setAlignment(Qt::AlignRight);
  connect(url, SIGNAL(linkActivated(const QString &)), SLOT(openFolder()));

  mainLay->addWidget(d->enable);
  mainLay->addWidget(d->requireSpaces);
  mainLay->addStretch();
  mainLay->addWidget(url);

  if (!d->createThemeList()) {
    d->enable->setEnabled(false);
    d->enable->setChecked(false);
    SimpleSettings->setBool("UseEmoticons", false);
  }

  enable(d->enable->isChecked());
}


EmoticonsSettings::~EmoticonsSettings() { delete d; }


void EmoticonsSettings::reset(int page)
{
  if (page == m_id) {
    d->enable->setChecked(true);
    d->requireSpaces->setChecked(true);
    d->combo->setCurrentIndex(d->combo->findText("Kolobok"));
    enable(true);
  }
}


void EmoticonsSettings::save()
{
  SimpleSettings->setBool("UseEmoticons", d->enable->isChecked());
  Emoticons::setStrictParse(d->requireSpaces->isChecked());
  SimpleSettings->setString("EmoticonTheme", d->combo->currentText());
  SimpleSettings->notify(Settings::EmoticonsChanged);
}


void EmoticonsSettings::enable(bool checked)
{
  d->group->setEnabled(checked);
  d->requireSpaces->setEnabled(checked);
}


void EmoticonsSettings::openFolder()
{
  QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/emoticons"));
}




class SoundSettings::Private
{
public:
  Private() {}

  QGroupBox *enable;
  SoundWidget *msg;
  SoundWidget *privateMsg;
  #ifdef Q_WS_X11
    QGroupBox *useCmd;
    QLineEdit *cmd;
  #endif
};

/*!
 * \brief Конструктор класса SoundSettings.
 */
SoundSettings::SoundSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::SoundPage, parent), d(new Private)
{
  d->enable = new QGroupBox(tr("&Звуковые уведомления"), this);
  d->enable->setCheckable(true);
  d->enable->setChecked(SimpleSettings->getBool("Sound"));

  QDir dir(QApplication::applicationDirPath() + "/sounds");
  QStringList list = dir.entryList(SimpleSettings->getList("Sound/NameFilter"), QDir::Files);

  d->msg = new SoundWidget("Message", tr("Сообщение"), tr("Сообщение в основной канал"), list, this);
  connect(d->msg, SIGNAL(play(const QString &)), SLOT(play(const QString &)));
  d->privateMsg = new SoundWidget("PrivateMessage", tr("Приватное сообщение"), tr("Сообщение в приват от другого пользователя"), list, this);
  connect(d->privateMsg, SIGNAL(play(const QString &)), SLOT(play(const QString &)));

  QVBoxLayout *soundLay = new QVBoxLayout(d->enable);
  soundLay->addWidget(d->msg);
  soundLay->addWidget(d->privateMsg);
  soundLay->setSpacing(0);

  #ifdef Q_WS_X11
    d->useCmd = new QGroupBox(tr("Внешняя команда для воспроизведения звука"), this);
    d->useCmd->setCheckable(true);
    d->useCmd->setChecked(SimpleSettings->getBool("Sound/UseExternalCmd"));
    d->cmd = new QLineEdit(SimpleSettings->getString("Sound/ExternalCmd"), this);
    d->cmd->setToolTip(tr("Внешняя команда, Вместо %1 подставляется\nимя звукового файла"));
    QVBoxLayout *cmdLay = new QVBoxLayout(d->useCmd);
    cmdLay->addWidget(d->cmd);
  #endif

  QLabel *url = new QLabel(QString("<a style='text-decoration:none; color:#1a4d82;' href='#'>%1</a>").arg(tr("Звуки")), this);
  url->setToolTip(tr("Открыть папку со звуками"));
  url->setAlignment(Qt::AlignRight);
  connect(url, SIGNAL(linkActivated(const QString &)), SLOT(openFolder()));

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(d->enable);
  #ifdef Q_WS_X11
    mainLay->addWidget(d->useCmd);
  #endif
  mainLay->addStretch();
  mainLay->addWidget(url);
}


SoundSettings::~SoundSettings() { delete d; }


void SoundSettings::reset(int page)
{
  if (page == m_id) {
    d->msg->reset(true, "Received.wav");
    d->privateMsg->reset(true, "Received.wav");

    #ifdef Q_WS_X11
      d->useCmd->setChecked(true);
      d->cmd->setText("aplay -q -N %1");
    #endif
  }
}


void SoundSettings::save()
{
  SimpleSettings->setBool("Sound", d->enable->isChecked());
  d->msg->save();
  d->privateMsg->save();
  #ifdef Q_WS_X11
    SimpleSettings->setBool("Sound/UseExternalCmd", d->useCmd->isChecked());
    SimpleSettings->setString("Sound/ExternalCmd", d->cmd->text());
  #endif
  SimpleSettings->notify(Settings::SoundChanged);
}


void SoundSettings::openFolder()
{
  QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/sounds"));
}


void SoundSettings::play(const QString &file)
{
  #ifdef Q_WS_X11
  if (d->useCmd->isChecked() && !d->cmd->text().isEmpty())
    QProcess::startDetached(d->cmd->text().arg(file));
  else
  #endif
    QSound::play(file);
}




class UpdateSettings::Private
{
public:
  Private()
  : last(-1)
  {}

  int last;
  QCheckBox *checkOnStartup;
  QComboBox *factor;
  QGroupBox *versionGroup;
  QSpinBox *interval;
  #ifndef SCHAT_NO_UPDATE
    QCheckBox *autoClean;
    QCheckBox *autoDownload;
  #endif
};

/*!
 * \brief Конструктор класса UpdateSettings.
 */
UpdateSettings::UpdateSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::UpdatePage, parent), d(new Private)
{
  // Уведомление о новых версиях.
  d->versionGroup = new QGroupBox(tr("&Уведомление о новых версиях"), this);
  d->versionGroup->setCheckable(true);
  d->versionGroup->setChecked(SimpleSettings->getBool("Updates/Enable"));

  d->checkOnStartup = new QCheckBox(tr("Проверять при &запуске"), this);
  d->checkOnStartup->setToolTip(tr("Проверять обновления при запуске программы"));
  d->checkOnStartup->setChecked(SimpleSettings->getBool("Updates/CheckOnStartup"));

  QLabel *interval = new QLabel(tr("&Интервал проверки:"), this);
  interval->setToolTip(tr("Временной интервал для периодической проверки\nобновлений"));
  QHBoxLayout *intervalLay = new QHBoxLayout;

  d->interval = new QSpinBox(this);
  d->interval->setRange(0, 60);
  connect(d->interval, SIGNAL(valueChanged(int)), SLOT(intervalChanged(int)));
  interval->setBuddy(interval);

  d->factor = new QComboBox(this);
  intervalChanged(SimpleSettings->getInt("Updates/CheckInterval"));
  connect(d->factor, SIGNAL(activated(int)), SLOT(factorChanged(int)));

  intervalLay->addWidget(interval);
  intervalLay->addWidget(d->interval);
  intervalLay->addWidget(d->factor);
  intervalLay->addStretch();

  QVBoxLayout *versionLay = new QVBoxLayout(d->versionGroup);
  versionLay->addWidget(d->checkOnStartup);
  versionLay->addLayout(intervalLay);

  // Автоматическое обновление
  #ifndef SCHAT_NO_UPDATE
    QGroupBox *updateGroup = new QGroupBox(tr("&Автоматическое обновление"), this);

    d->autoDownload = new QCheckBox(tr("Автоматически &загружать"), this);
    d->autoDownload->setToolTip(tr("Автоматически загружать обновления"));
    d->autoDownload->setChecked(SimpleSettings->getBool("Updates/AutoDownload"));

    d->autoClean = new QCheckBox(tr("&Удалять после установки"), this);
    d->autoClean->setToolTip(tr("Удалять обновления после установки"));
    d->autoClean->setChecked(SimpleSettings->getBool("Updates/AutoClean"));

    QVBoxLayout *updateLay = new QVBoxLayout(updateGroup);
    updateLay->addWidget(d->autoDownload);
    updateLay->addWidget(d->autoClean);

    connect(d->versionGroup, SIGNAL(toggled(bool)), updateGroup, SLOT(setEnabled(bool)));
    updateGroup->setEnabled(d->versionGroup->isChecked());
  #endif

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(d->versionGroup);
  #ifndef SCHAT_NO_UPDATE
    mainLay->addWidget(updateGroup);
  #endif
  mainLay->addStretch();
}

UpdateSettings::~UpdateSettings() { delete d; }


void UpdateSettings::reset(int page)
{
  if (page == m_id) {
    d->versionGroup->setChecked(true);
    d->checkOnStartup->setChecked(true);
    d->interval->setValue(d->factor->currentIndex() ? 1 : 60);

    #ifndef SCHAT_NO_UPDATE
      d->autoDownload->setChecked(false);
      d->autoClean->setChecked(true);
    #endif
  }
}


void UpdateSettings::save()
{
  SimpleSettings->setBool("Updates/Enable",         d->versionGroup->isChecked());
  SimpleSettings->setBool("Updates/CheckOnStartup", d->checkOnStartup->isChecked());
  SimpleSettings->setInt("Updates/CheckInterval",   d->factor->currentIndex() ? d->interval->value() * 60 : d->interval->value());
  SimpleSettings->notify(Settings::UpdateSettingsChanged);

  #ifndef SCHAT_NO_UPDATE
    SimpleSettings->setBool("Updates/AutoDownload",   d->autoDownload->isChecked());
    SimpleSettings->setBool("Updates/AutoClean",      d->autoClean->isChecked());
  #endif
}


/*!
 * Изменение отсчёта времени (минуты/часы).
 */
void UpdateSettings::factorChanged(int index)
{
  if (d->last != index)
    if (index)
      d->interval->setValue(1);
    else
      d->interval->setValue(59);

  d->last = index;
}


/*!
 * Обработка изменения значения в \a d->interval.
 */
void UpdateSettings::intervalChanged(int i)
{
  int value = i;
  int index = 0;

  if (d->factor->count() == 0) {
    if (value > 59) {
      value /= 60;
      index = 1;
    }
  }
  else {
    index = d->factor->currentIndex();

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

  d->factor->clear();
  d->factor->addItem(minute);
  d->factor->addItem(hour);
  d->factor->setCurrentIndex(index);
  d->last = index;

  d->interval->setValue(value);
}




class MiscSettings::Private
{
public:
  Private() {}
  #ifdef Q_WS_WIN
    void readAutostart();
    void writeAutostart();
  #endif

  QCheckBox *log;
  QCheckBox *logPrivate;
  #ifdef Q_WS_WIN
    QCheckBox *autostart;
    QCheckBox *autostartDaemon;
  #endif
};


/*!
 * Устанавливает состояние флажков добавления в автозагрузку.
 * Если файл программы управления сервером не найден, то скрываем флажок сервера.
 * Если ключа в реестре не найдено, снимаем флажок.
 */
#ifdef Q_WS_WIN
void MiscSettings::Private::readAutostart()
{
  QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

  if (QFile::exists(QApplication::applicationDirPath() + "/schatd-ui.exe")) {
    QString value = reg.value(QApplication::applicationName() + " Daemon", "").toString();
    if (value.isEmpty())
      autostartDaemon->setChecked(false);
  }
  else
    autostartDaemon->setVisible(false);

  QString value = reg.value(QApplication::applicationName(), "").toString();
  if (value.isEmpty())
    autostart->setChecked(false);
}


/*!
 * Добавляет/удаляет чат и при необходимости сервер из автозагрузки.
 */
void MiscSettings::Private::writeAutostart()
{
  QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

  if (autostart->checkState() == Qt::Checked)
    reg.setValue(QApplication::applicationName(), QDir::toNativeSeparators(QApplication::applicationFilePath()) + " -hide");
  else if (autostart->checkState() == Qt::Unchecked)
    reg.remove(QApplication::applicationName());

  if (autostartDaemon->isVisible()) {
    if (autostart->checkState() == Qt::Checked)
      reg.setValue(QApplication::applicationName() + " Daemon", QDir::toNativeSeparators(QApplication::applicationDirPath() + "/schatd-ui.exe") + " -start");
    else if (autostart->checkState() == Qt::Unchecked)
      reg.remove(QApplication::applicationName() + " Daemon");
  }
}
#endif


/*!
 * \brief Конструктор класса MiscSettings.
 */
MiscSettings::MiscSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::MiscPage, parent), d(new Private)
{
  #ifdef Q_WS_WIN
    QGroupBox *integration = new QGroupBox(tr("Интеграция"), this);

    d->autostart = new QCheckBox(tr("&Автозапуск"), this);
    d->autostart->setToolTip(tr("Автозапуск программы при старте системы"));
    d->autostart->setTristate();
    d->autostart->setCheckState(Qt::PartiallyChecked);

    d->autostartDaemon = new QCheckBox(tr("Автозапуск &сервера"), this);
    d->autostartDaemon->setToolTip(tr("Автозапуск сервера при старте системы"));
    d->autostartDaemon->setTristate();
    d->autostartDaemon->setCheckState(Qt::PartiallyChecked);

    QVBoxLayout *integrationLay = new QVBoxLayout(integration);
    integrationLay->addWidget(d->autostart);
    integrationLay->addWidget(d->autostartDaemon);
  #endif

  QGroupBox *logGroup = new QGroupBox(tr("&Журналирование"), this);

  d->log = new QCheckBox(tr("Журнал &основного канала"), this);
  d->log->setToolTip(tr("Вести журнал основного канала"));
  d->log->setChecked(SimpleSettings->getBool("Log"));

  d->logPrivate = new QCheckBox(tr("Журналы &приватов"), this);
  d->logPrivate->setToolTip(tr("Вести журналы приватных сообщений"));
  d->logPrivate->setChecked(SimpleSettings->getBool("LogPrivate"));

  QVBoxLayout *logLay = new QVBoxLayout(logGroup);
  logLay->addWidget(d->log);
  logLay->addWidget(d->logPrivate);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  #ifdef Q_WS_WIN
    mainLay->addWidget(integration);
  #endif
  mainLay->addWidget(logGroup);
  mainLay->addStretch();

  #ifdef Q_WS_WIN
    d->readAutostart();
  #endif
}


MiscSettings::~MiscSettings() { delete d; }


void MiscSettings::reset(int page)
{
  if (page == m_id) {
    d->log->setChecked(true);
    d->logPrivate->setChecked(true);
  }
}


void MiscSettings::save()
{
  #ifdef Q_WS_WIN
    d->writeAutostart();
  #endif

  SimpleSettings->setBool("Log", d->log->isChecked());
  SimpleSettings->setBool("LogPrivate", d->logPrivate->isChecked());
  SimpleSettings->notify(Settings::MiscSettingsChanged);
}
