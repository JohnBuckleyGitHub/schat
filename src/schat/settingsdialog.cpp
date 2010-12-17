/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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
#include "profilewidget.h"
#include "settings.h"
#include "settingsdialog.h"
#include "simplechatapp.h"
#include "widget/networkwidget.h"
#include "widget/nickedit.h"
#include "widget/soundwidget.h"

#ifndef SCHAT_NO_WEBKIT
  #include "chatwindow/chatwindowstyle.h"
#endif

/*!
 * \brief Конструктор класса SettingsDialog.
 */
SettingsDialog::SettingsDialog(QWidget *parent)
  : AbstractSettingsDialog(parent)
{
  ProfileSettings *profilePage = new ProfileSettings(this);
  NetworkSettings *networkPage = new NetworkSettings(this);

  createPage(QIcon(":/images/profile.png"),               tr("Личные данные"), profilePage);
  createPage(QIcon(":/images/network.png"),               tr("Сеть"),          networkPage);
  createPage(QIcon(":/images/applications-graphics.png"), tr("Интерфейс"),     new InterfaceSettings(this));
  createPage(QIcon(":/images/emoticon.png"),              tr("Смайлики"),      new EmoticonsSettings(this));
  createPage(QIcon(":/images/sound.png"),                 tr("Звуки"),         new SoundSettings(this));
  createPage(QIcon(":/images/notification.png"),          tr("Оповещатель"),   new NotificationSettings(this));
  createPage(QIcon(":/images/update.png"),                tr("Обновление"),    new UpdateSettings(this));
  createPage(QIcon(":/images/application-x-desktop.png"), tr("Разное"),        new MiscSettings(this));

  connect(profilePage, SIGNAL(validNick(bool)), m_okButton, SLOT(setEnabled(bool)));
  connect(networkPage, SIGNAL(validServer(bool)), m_okButton, SLOT(setEnabled(bool)));
}


/*!
 * Открывает папку, которая при необходимости будет создана.
 */
void SettingsDialog::openFolder(int path)
{
  QString dir = SimpleSettings->path(static_cast<Settings::Paths>(path)).at(0);
  if (!QDir().exists(dir))
    QDir().mkpath(dir);

  QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
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
  Private()
  : profile(SimpleSettings->profile()),
  maxRecentItems(SimpleSettings->getInt("Profile/MaxRecentItems"))
  {}

  AbstractProfile *profile;
  bool maxRecentItems;
  ProfileWidget *profileWidget;
  QCheckBox *autoAway;
  QCheckBox *exitAwayOnSend;
  QLineEdit *byeMsgEdit;
  QSpinBox *autoAwayTime;
};


/*!
 * \brief Конструктор класса ProfileSettings.
 */
ProfileSettings::ProfileSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::ProfilePage, parent), d(new Private)
{
  d->profileWidget = new ProfileWidget(SimpleSettings->getBool("CompactGenderWidget"), this);
  connect(d->profileWidget, SIGNAL(validNick(bool)), SIGNAL(validNick(bool)));

  d->byeMsgEdit = new QLineEdit(d->profile->byeMsg(), this);
  d->byeMsgEdit->setMaxLength(AbstractProfile::MaxByeMsgLength);
  d->byeMsgEdit->setToolTip(tr("Сообщение которое увидят другие пользователи\nесли вы выйдете из чата"));
  if (d->maxRecentItems) {
    d->byeMsgEdit->setCompleter(new QCompleter(SimpleSettings->getList("Profile/RecentByeMsgs"), this));
    d->byeMsgEdit->completer()->setCaseSensitivity(Qt::CaseInsensitive);
  }
  QLabel *byeMsgLabel = new QLabel(tr("Сообщение при &выходе:"), this);
  byeMsgLabel->setBuddy(d->byeMsgEdit);

  QGroupBox *profileGroup = new QGroupBox(tr("Профиль"), this);
  QVBoxLayout *profileLay = new QVBoxLayout(profileGroup);
  profileLay->setMargin(6);
  profileLay->setSpacing(4);
  profileLay->addWidget(d->profileWidget);
  profileLay->addWidget(byeMsgLabel);
  profileLay->addWidget(d->byeMsgEdit);

  d->autoAway = new QCheckBox(tr("Включить &статус при простое:"), this);
  d->autoAway->setToolTip(tr("Автоматически переходить в статус Отсутствую\nпри простое и возвращаться в обычный режим\nпри появлении активности"));
  d->autoAway->setChecked(SimpleSettings->getBool("AutoAway"));

  d->autoAwayTime = new QSpinBox(this);
  d->autoAwayTime->setRange(1, 1440);
  d->autoAwayTime->setSuffix(tr(" мин"));
  d->autoAwayTime->setValue(SimpleSettings->getInt("AutoAwayTime"));

  connect(d->autoAway, SIGNAL(clicked(bool)), d->autoAwayTime, SLOT(setEnabled(bool)));
  d->autoAwayTime->setEnabled(d->autoAway->isChecked());

  d->exitAwayOnSend = new QCheckBox(tr("Возвращаться в &обычный режим при отправке"), this);
  d->exitAwayOnSend->setToolTip(tr("Возвращаться в обычный режим при отправке\nсообщения, если до этого статус Отсутствую\nбыл установлен вручную"));
  d->exitAwayOnSend->setChecked(SimpleSettings->getBool("ExitAwayOnSend"));

  QGroupBox *awayGroup = new QGroupBox(tr("Статус: Отсутствую"), this);
  QGridLayout *awayLay = new QGridLayout(awayGroup);
  awayLay->addWidget(d->autoAway, 0, 0);
  awayLay->addWidget(d->autoAwayTime, 0, 1);
  awayLay->addWidget(d->exitAwayOnSend, 1, 0, 1, 2);
  awayLay->setColumnStretch(0, 1);
  awayLay->setMargin(6);
  awayLay->setSpacing(4);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(profileGroup);
  mainLay->addSpacing(12);
  mainLay->addWidget(awayGroup);
  mainLay->addStretch();
  mainLay->setContentsMargins(3, 3, 3, 0);
}

ProfileSettings::~ProfileSettings() { delete d; }


void ProfileSettings::reset(int page)
{
  if (page == m_id) {
    d->profileWidget->reset();
    d->byeMsgEdit->setText(QApplication::applicationName());
    d->autoAway->setChecked(true);
    d->autoAwayTime->setValue(10);
    d->autoAwayTime->setEnabled(true);
    d->exitAwayOnSend->setChecked(true);
  }
}


void ProfileSettings::save()
{
  d->profileWidget->save();

  if (d->profile->byeMsg() != d->byeMsgEdit->text()) {
    d->profile->setByeMsg(d->byeMsgEdit->text());
    if (d->maxRecentItems)
      NickEdit::modifyRecentList("Profile/RecentByeMsgs", d->profile->byeMsg());

    SimpleSettings->notify(Settings::ByeMsgChanged);
  }

  int modified = 0;
  modified += SimpleSettings->save("AutoAway", d->autoAway->isChecked());
  modified += SimpleSettings->save("AutoAwayTime", d->autoAwayTime->value());
  modified += SimpleSettings->save("ExitAwayOnSend", d->exitAwayOnSend->isChecked());

  if (modified)
    SimpleSettings->notify(Settings::AwaySettingsChanged);
}




class NetworkSettings::Private
{
public:
  Private() {}

  NetworkWidget *network;
  QCheckBox *welcome;
  QComboBox *type;
  QGroupBox *proxyGroup;
  QLineEdit *host;
  QLineEdit *password;
  QLineEdit *userName;
  QSpinBox *port;
};

/*!
 * \brief Конструктор класса NetworkSettings.
 */
NetworkSettings::NetworkSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::NetworkPage, parent), d(new Private)
{
  d->welcome = new QCheckBox(tr("Всегда использовать это подключение"), this);
  d->welcome->setChecked(SimpleSettings->getBool("HideWelcome"));
  d->welcome->setToolTip(tr("Не запрашивать персональную информацию\nи адрес сервера при запуске программы"));

  d->network = new NetworkWidget(this);
  connect(d->network, SIGNAL(validServer(bool)), SIGNAL(validServer(bool)));

  QGroupBox *serverGroup = new QGroupBox(tr("Подключение"), this);
  QVBoxLayout *serverLay = new QVBoxLayout(serverGroup);
  serverLay->addWidget(d->network);
  serverLay->addWidget(d->welcome);
  serverLay->setMargin(6);
  serverLay->setSpacing(4);

  d->proxyGroup = new QGroupBox(tr("Подключение через прокси-сервер"), this);
  d->proxyGroup->setCheckable(true);
  d->proxyGroup->setChecked(SimpleSettings->getBool("Proxy/Enable"));
  d->proxyGroup->setVisible(!SimpleSettings->getBool("Proxy/HideAndDisable"));

  d->type = new QComboBox(this);
  d->type->addItem(tr("HTTP"));
  d->type->addItem(tr("SOCKS5"));
  d->type->setToolTip(tr("Тип прокси-сервера"));

  if (SimpleSettings->getInt("Proxy/Type") == 1)
    d->type->setCurrentIndex(1);
  else
    d->type->setCurrentIndex(0);

  QLabel *type = new QLabel(tr("&Тип:"), this);
  type->setBuddy(d->type);

  d->host = new QLineEdit(SimpleSettings->getString("Proxy/Host"), this);
  d->host->setToolTip(tr("Адрес прокси-сервера"));
  QLabel *host = new QLabel(tr("&Адрес:"), this);
  host->setBuddy(d->host);

  d->port = new QSpinBox(this);
  d->port->setRange(1, 65536);
  d->port->setToolTip(tr("Порт прокси-сервера"));
  d->port->setValue(SimpleSettings->getInt("Proxy/Port"));
  QLabel *port = new QLabel(tr("&Порт:"), this);
  port->setBuddy(d->port);

  d->userName = new QLineEdit(SimpleSettings->getString("Proxy/UserName"), this);
  d->userName->setToolTip(tr("Имя пользователя для авторизации на прокси-сервере"));
  QLabel *userName = new QLabel(tr("&Имя:"), this);
  userName->setBuddy(d->userName);

  d->password = new QLineEdit(SimpleSettings->getString("Proxy/Password"), this);
  d->password->setEchoMode(QLineEdit::Password);
  d->password->setToolTip(tr("Пароль для авторизации на прокси-сервере"));
  QLabel *password = new QLabel(tr("Па&роль:"), this);
  password->setBuddy(d->password);

  QGridLayout *proxyLay = new QGridLayout(d->proxyGroup);
  proxyLay->addWidget(type, 0, 0);
  proxyLay->addWidget(d->type, 0, 1);
  proxyLay->addWidget(host, 1, 0);
  proxyLay->addWidget(d->host, 1, 1, 1, 3);
  proxyLay->addWidget(port, 1, 4);
  proxyLay->addWidget(d->port, 1, 5);
  proxyLay->addWidget(userName, 2, 0);
  proxyLay->addWidget(d->userName, 2, 1);
  proxyLay->addWidget(password, 2, 2);
  proxyLay->addWidget(d->password, 2, 3, 1, 3);

  QLabel *url = new QLabel(QString("<a style='text-decoration:none; color:#1a4d82;' href='#'>%1</a>").arg(tr("Файлы сети")), this);
  url->setToolTip(tr("Открыть папку с файлами сети"));
  url->setAlignment(Qt::AlignRight);
  connect(url, SIGNAL(linkActivated(const QString &)), SLOT(openFolder()));

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(serverGroup);
  mainLay->addSpacing(12);
  mainLay->addWidget(d->proxyGroup);
  mainLay->addStretch();
  mainLay->addWidget(url);
  mainLay->setContentsMargins(3, 3, 3, 0);
}


NetworkSettings::~NetworkSettings() { delete d; }


void NetworkSettings::reset(int page)
{
  if (page == m_id) {
    d->network->reset();
    d->welcome->setChecked(true);
    d->proxyGroup->setChecked(false);
    d->host->clear();
    d->port->setValue(3128);
    d->userName->clear();
    d->password->clear();
  }
}


void NetworkSettings::save()
{
  int modified = d->network->save(false);
  SimpleSettings->setBool("HideWelcome", d->welcome->isChecked());

  modified += SimpleSettings->save("Proxy/Enable", d->proxyGroup->isChecked());

  int proxyPref = 0;
  proxyPref += SimpleSettings->save("Proxy/Type",     d->type->currentIndex());
  proxyPref += SimpleSettings->save("Proxy/Port",     d->port->value());
  proxyPref += SimpleSettings->save("Proxy/Host",     d->host->text());
  proxyPref += SimpleSettings->save("Proxy/UserName", d->userName->text());
  proxyPref += SimpleSettings->save("Proxy/Password", d->password->text());

  if (d->proxyGroup->isChecked())
    modified += proxyPref;

  if (modified) {
    SimpleSettings->setApplicationProxy();
    SimpleSettings->notify(Settings::NetworkSettingsChanged);
  }
}


void NetworkSettings::openFolder()
{
  SettingsDialog::openFolder(Settings::NetworksPath);
}




class InterfaceSettings::Private
{
public:
  Private() {}
  #if !defined(SCHAT_NO_STYLE)
  QComboBox *mainStyle;
  #endif

  #ifndef SCHAT_NO_WEBKIT
    void createStylesList();
    void reloadVariants(int index);
    void setStyle();

    QCheckBox *grouping;
    QComboBox *chatStyle;
    QComboBox *chatStyleVariant;
  #endif
};


#ifndef SCHAT_NO_WEBKIT
void InterfaceSettings::Private::createStylesList()
{
  QStringList stylesDirs = SimpleSettings->path(Settings::StylesPath);
  int index = 0;

  for (int i = 0; i < stylesDirs.count(); ++i) {
    QString dir = stylesDirs.at(i) + '/';
    QDir qdir(dir);
    qdir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    qdir.setSorting(QDir::Name);

    QStringList entryList = qdir.entryList();
    if (entryList.contains("Default"))
      entryList.removeAll("Default");

    foreach (QString style, entryList) {
      QString styleDirPath = dir + style;
      if (ChatWindowStyle::isValid(styleDirPath)) {
        index++;
        chatStyle->addItem(style);
        chatStyle->setItemData(index, QStringList(ChatWindowStyle::variants(styleDirPath + "/Contents/Resources/Variants/").keys()), Qt::UserRole + 1);
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
  else if (index == 0)
    chatStyleVariant->addItem("Monospace");
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
#endif


/*!
 * \brief Конструктор класса InterfaceSettings.
 */
InterfaceSettings::InterfaceSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::InterfacePage, parent), d(new Private)
{
  #if !defined(SCHAT_NO_STYLE)
  d->mainStyle = new QComboBox(this);
  d->mainStyle->addItems(QStyleFactory::keys());
  d->mainStyle->setCurrentIndex(d->mainStyle->findText(SimpleSettings->getString("Style")));

  QGroupBox *mainStyleGroup = new QGroupBox(tr("&Глобальный стиль"), this);
  QHBoxLayout *mainStyleLay = new QHBoxLayout(mainStyleGroup);
  mainStyleLay->addWidget(d->mainStyle);
  mainStyleLay->addStretch();
  mainStyleLay->setMargin(6);
  mainStyleLay->setSpacing(4);
  #endif

  #ifndef SCHAT_NO_WEBKIT
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
    chatStyleLay->setMargin(6);
    chatStyleLay->setSpacing(4);
  #endif

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  #if !defined(SCHAT_NO_STYLE)
  mainLay->addWidget(mainStyleGroup);
  #endif
  #ifndef SCHAT_NO_WEBKIT
    #if !defined(SCHAT_NO_STYLE)
    mainLay->addSpacing(12);
    #endif
    mainLay->addWidget(chatStyleGroup);
  #endif
  mainLay->addStretch();
  mainLay->setContentsMargins(3, 3, 3, 0);

  #ifndef SCHAT_NO_WEBKIT
    connect(d->chatStyle, SIGNAL(currentIndexChanged(int)), SLOT(reloadVariants(int)));

    d->createStylesList();
    d->setStyle();
  #endif
}


InterfaceSettings::~InterfaceSettings() { delete d; }


void InterfaceSettings::reset(int page)
{
  if (page == m_id) {
    #if !defined(SCHAT_NO_STYLE)
    d->mainStyle->setCurrentIndex(d->mainStyle->findText(SimpleChatApp::defaultStyle()));
    #endif
    #ifndef SCHAT_NO_WEBKIT
      d->chatStyle->setCurrentIndex(0);
      d->chatStyleVariant->setCurrentIndex(0);
      d->grouping->setChecked(false);
   #endif
  }
}


void InterfaceSettings::save()
{
  #if !defined(SCHAT_NO_STYLE)
  if (d->mainStyle->currentIndex() != -1) {
    SimpleSettings->setString("Style", d->mainStyle->currentText()) ;
    QApplication::setStyle(d->mainStyle->currentText());
  }
  #endif

  #ifndef SCHAT_NO_WEBKIT
    if (d->chatStyle->currentIndex() != -1)
      SimpleSettings->setString("ChatStyle", d->chatStyle->currentText());

    if (d->chatStyleVariant->currentIndex() == 0)
      SimpleSettings->setString("ChatStyleVariant", "");
    else if (d->chatStyleVariant->currentIndex() > 0)
      SimpleSettings->setString("ChatStyleVariant", d->chatStyleVariant->currentText());

    SimpleSettings->setBool("MessageGrouping", d->grouping->isChecked());
    SimpleSettings->notify(Settings::InterfaceSettingsChanged);
  #endif
}


#ifndef SCHAT_NO_WEBKIT
void InterfaceSettings::reloadVariants(int index)
{
  d->reloadVariants(index);
}
#endif



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
  themeLay->setMargin(6);
  themeLay->setSpacing(4);

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
  mainLay->setSpacing(4);
  mainLay->setContentsMargins(3, 3, 3, 0);

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
  SettingsDialog::openFolder(Settings::EmoticonsPath);
}




class SoundSettings::Private
{
public:
  Private() {}

  QCheckBox *muteInDnD;
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

  QStringList sounds = SimpleSettings->path(Settings::SoundsPath);
  QStringList nameFilter = SimpleSettings->getList("Sound/NameFilter");
  QStringList list;

  foreach (QString path, sounds) {
    QDir dir(path);
    foreach (QString file, dir.entryList(nameFilter, QDir::Files)) {
      if (!list.contains(file))
        list << file;
    }
  }

  d->msg = new SoundWidget("Message", tr("&Сообщение"), tr("Сообщение в основной канал"), list, this);
  connect(d->msg, SIGNAL(play(const QString &)), SLOT(play(const QString &)));

  d->privateMsg = new SoundWidget("PrivateMessage", tr("&Приватное сообщение"), tr("Сообщение в приват от другого пользователя"), list, this);
  connect(d->privateMsg, SIGNAL(play(const QString &)), SLOT(play(const QString &)));

  QVBoxLayout *soundLay = new QVBoxLayout(d->enable);
  soundLay->addWidget(d->msg);
  soundLay->addWidget(d->privateMsg);
  soundLay->setMargin(6);
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

  d->muteInDnD = new QCheckBox(tr("Статус \"Не беспокоить\" &отключает звук"), this);
  d->muteInDnD->setToolTip(tr("Использование статуса \"Не беспокоить\" отключает звук"));
  d->muteInDnD->setChecked(SimpleSettings->getBool("Sound/MuteInDnD"));

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(d->enable);
  #ifdef Q_WS_X11
    mainLay->addWidget(d->useCmd);
  #endif
  mainLay->addWidget(d->muteInDnD);
  mainLay->addStretch();
  mainLay->addWidget(url);
  mainLay->setContentsMargins(3, 3, 3, 0);
}


SoundSettings::~SoundSettings() { delete d; }


void SoundSettings::reset(int page)
{
  if (page == m_id) {
    d->enable->setChecked(true);
    d->msg->reset(true, "Received.wav");
    d->privateMsg->reset(true, "Received.wav");
    d->muteInDnD->setChecked(true);

    #ifdef Q_WS_X11
      d->useCmd->setChecked(true);
      d->cmd->setText("aplay -q -N %1");
    #endif
  }
}


void SoundSettings::save()
{
  int modified = 0;

  modified += SimpleSettings->save("Sound", d->enable->isChecked());
  modified += d->msg->save();
  modified += d->privateMsg->save();
  modified += SimpleSettings->save("Sound/MuteInDnD", d->muteInDnD->isChecked());
  #ifdef Q_WS_X11
    modified += SimpleSettings->save("Sound/UseExternalCmd", d->useCmd->isChecked());
    modified += SimpleSettings->save("Sound/ExternalCmd", d->cmd->text());
  #endif

  if (modified)
    SimpleSettings->notify(Settings::SoundChanged);
}


void SoundSettings::openFolder()
{
  SettingsDialog::openFolder(Settings::SoundsPath);
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




class NotificationSettings::Private
{
public:
  Private() {}

  QCheckBox *autoAway;
  QCheckBox *dnd;
  QCheckBox *privateMsg;
  QCheckBox *publicMsg;
  QCheckBox *timeOut;
  QGroupBox *popupGroup;
  QSpinBox *timeOutSpin;
};


/*!
 * \brief Конструктор класса NotificationSettings.
 */
NotificationSettings::NotificationSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::NotificationPage, parent), d(new Private)
{
  d->privateMsg = new QCheckBox(tr("&Приватные сообщение"), this);
  d->privateMsg->setToolTip(tr("Оповещать о приватных сообщениях"));
  d->privateMsg->setChecked(SimpleSettings->getBool("Notification"));

  d->publicMsg = new QCheckBox(tr("&Обращение в основном канале"), this);
  d->publicMsg->setToolTip(tr("Оповещать об обращениях\nпо нику в основном канале"));
  d->publicMsg->setChecked(SimpleSettings->getBool("NotificationPublic"));

  QGroupBox *eventGroup = new QGroupBox(tr("Оповещение о событиях"), this);
  QVBoxLayout *eventLay = new QVBoxLayout(eventGroup);
  eventLay->addWidget(d->privateMsg);
  eventLay->addWidget(d->publicMsg);
  eventLay->setMargin(6);
  eventLay->setSpacing(0);

  d->dnd = new QCheckBox(tr("&Статус \"Не беспокоить\" отключает оповещения"), this);
  d->dnd->setToolTip(tr("Использование статуса \"Не беспокоить\"\nотключает оповещения"));
  d->dnd->setChecked(SimpleSettings->getBool("NoNotificationInDnD"));

  d->timeOut = new QCheckBox(tr("&Автоматически закрывать спустя:"), this);
  d->timeOut->setToolTip(tr("Автоматически закрывать всплывающие\nокна спустя заданное число секунд"));
  d->timeOut->setChecked(SimpleSettings->getBool("PopupAutoClose"));

  d->timeOutSpin = new QSpinBox(this);
  d->timeOutSpin->setRange(1, 1440);
  d->timeOutSpin->setSuffix(tr(" сек"));
  d->timeOutSpin->setValue(SimpleSettings->getInt("PopupAutoCloseTime"));

  d->autoAway = new QCheckBox(tr("&Но не при автоматическом статусе \"Отсутствую\""), this);
  d->autoAway->setToolTip(tr("Не закрывать автоматически всплывающие окна\nпри автоматическом статусе \"Отсутствую\""));
  d->autoAway->setChecked(SimpleSettings->getBool("NoPopupAutoCloseInAway"));

  d->popupGroup = new QGroupBox(tr("Параметры всплывающих окон"), this);
  QGridLayout *popupLay = new QGridLayout(d->popupGroup);
  popupLay->addWidget(d->dnd, 0, 0, 1, 3);
  popupLay->addWidget(d->timeOut, 1, 0, 1, 2);
  popupLay->addWidget(d->timeOutSpin, 1, 2);
  popupLay->addItem(new QSpacerItem(32, 0), 2, 0);
  popupLay->addWidget(d->autoAway, 2, 1, 1, 2);
  popupLay->setColumnStretch(1, 1);
  popupLay->setMargin(6);
  popupLay->setSpacing(0);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(eventGroup);
  mainLay->addSpacing(12);
  mainLay->addWidget(d->popupGroup);
  mainLay->addStretch();
  mainLay->setContentsMargins(3, 3, 3, 0);

  d->autoAway->setEnabled(d->timeOut->isChecked());
  d->timeOutSpin->setEnabled(d->timeOut->isChecked());
  popupGroupState();

  connect(d->timeOut,    SIGNAL(clicked(bool)), d->autoAway,    SLOT(setEnabled(bool)));
  connect(d->timeOut,    SIGNAL(clicked(bool)), d->timeOutSpin, SLOT(setEnabled(bool)));
  connect(d->privateMsg, SIGNAL(clicked(bool)),                 SLOT(popupGroupState()));
  connect(d->publicMsg,  SIGNAL(clicked(bool)),                 SLOT(popupGroupState()));
}


NotificationSettings::~NotificationSettings() { delete d; }


void NotificationSettings::reset(int page)
{
  if (page == m_id) {
    d->privateMsg->setChecked(true);
    d->publicMsg->setChecked(true);
    d->dnd->setChecked(true);
    d->timeOut->setChecked(true);
    d->autoAway->setChecked(true);
    d->autoAway->setEnabled(true);
    d->timeOutSpin->setValue(10);
    d->timeOutSpin->setEnabled(true);
    d->popupGroup->setEnabled(true);
  }
}


void NotificationSettings::save()
{
  int modified = 0;

  modified += SimpleSettings->save("Notification",           d->privateMsg->isChecked());
  modified += SimpleSettings->save("NotificationPublic",     d->publicMsg->isChecked());
  modified += SimpleSettings->save("NoNotificationInDnD",    d->dnd->isChecked());
  modified += SimpleSettings->save("PopupAutoClose",         d->timeOut->isChecked());
  modified += SimpleSettings->save("PopupAutoCloseTime",     d->timeOutSpin->value());
  modified += SimpleSettings->save("NoPopupAutoCloseInAway", d->autoAway->isChecked());

  if (modified)
    SimpleSettings->notify(Settings::NotificationChanged);
}


void NotificationSettings::popupGroupState()
{
  d->popupGroup->setEnabled(d->privateMsg->isChecked() || d->publicMsg->isChecked());
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
  intervalLay->setMargin(6);
  intervalLay->setSpacing(4);

  QVBoxLayout *versionLay = new QVBoxLayout(d->versionGroup);
  versionLay->addWidget(d->checkOnStartup);
  versionLay->addLayout(intervalLay);
  versionLay->setMargin(6);
  versionLay->setSpacing(4);

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
    updateLay->setMargin(6);
    updateLay->setSpacing(4);
  #endif

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(d->versionGroup);
  #ifndef SCHAT_NO_UPDATE
    mainLay->addSpacing(12);
    mainLay->addWidget(updateGroup);
  #endif
  mainLay->addStretch();
  mainLay->setContentsMargins(3, 3, 3, 0);
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
  if (d->last != index) {
    if (index)
      d->interval->setValue(1);
    else
      d->interval->setValue(59);
  }

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
  QString languageIcon(const QString &file) const;
  QString languageName(const QString &file) const;
  QStringList findQmFiles() const;
  void readAutostart();
  void translations();
  void writeAutostart();

  QCheckBox *autostart;
  QCheckBox *log;
  QCheckBox *logPrivate;
  QComboBox *language;

  #ifdef Q_WS_WIN
  QCheckBox *autostartDaemon;
  #endif
};


/*!
 * Возвращает путь к файлу изображения флага языка.
 * Предполагается, что имя файла с изображением флага языка совпадает с именем языкового файла
 * за исключением расширения, например schat_ru.qm и schat_ru.png.
 * В случае если не будут найдены изображения для русского или английского языка,
 * то для них будут использованы изображения по умолчанию.
 */
QString MiscSettings::Private::languageIcon(const QString &file) const
{
  QString icon = file.left(file.size() - 3) + ".png";
  if (QFile::exists(icon))
    return icon;

  if (icon.endsWith("schat_ru.png"))
    return ":/translations/schat_ru.png";

  if (icon.endsWith("schat_en.png"))
    return ":/translations/schat_en.png";

  return ":/images/lang/unknown.png";
}


/*!
 * Получаем переведённое имя языка из qm файла.
 */
QString MiscSettings::Private::languageName(const QString &file) const
{
  QTranslator translator;
  translator.load(file);

  return translator.translate("SimpleChatApp", "English");
}


/*!
 * \todo проверить обработку коллизий.
 */
QStringList MiscSettings::Private::findQmFiles() const
{
  QStringList dirs = SimpleSettings->path(Settings::TranslationsPath);
  QStringList fileNames;

  foreach (QString d, dirs) {
    QDir dir(d);
    QStringList fn = dir.entryList(QStringList("*.qm"), QDir::Files, QDir::Name);
    QMutableStringListIterator i(fn);
    while (i.hasNext()) {
        i.next();
        i.setValue(dir.filePath(i.value()));
    }
    fileNames += fn;
  }

  return fileNames;
}


/*!
 * Устанавливает состояние флажков добавления в автозагрузку.
 * Если файл программы управления сервером не найден, то скрываем флажок сервера.
 * Если ключа в реестре не найдено, снимаем флажок.
 */
void MiscSettings::Private::readAutostart()
{
  #if defined(Q_WS_WIN)
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
  #elif defined(Q_WS_X11)
  QString desktopFile = QDir::homePath() + "/.config/autostart/schat.desktop";
  if (!QFile::exists(desktopFile) || QSettings(desktopFile, QSettings::IniFormat).value("Desktop Entry/X-GNOME-Autostart-enabled").toBool() == false) {
    autostart->setChecked(false);
    return;
  }
  #endif
}


/*!
 * Загрузка языковых файлов.
 */
void MiscSettings::Private::translations()
{
  QStringList qmFiles = findQmFiles();
  qmFiles.append(":/translations/schat_ru.qm");
  bool canOverrideEnglish = true;

  for (int i = 0; i < qmFiles.size(); ++i) {
    QString file = qmFiles[i];
    QString langName = languageName(file);

    // Добавляем в список только уникальные языки.
    if (language->findText(langName) == -1) {
      language->addItem(QIcon(languageIcon(file)), langName, file);
    }
    else if (canOverrideEnglish && file.endsWith("schat_en.qm")) {
      language->setItemIcon(0, QIcon(languageIcon(file)));
      language->setItemData(0, file);
      canOverrideEnglish = false;
    }
  }

  language->setCurrentIndex(language->findText(SimpleChatApp::instance()->language()));
}


/*!
 * Добавляет/удаляет чат и при необходимости сервер из автозагрузки.
 */
void MiscSettings::Private::writeAutostart()
{
  #if defined(Q_WS_WIN)
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
  #elif defined(Q_WS_X11)
  QString path = QDir::homePath() + "/.config/autostart/";
  if (autostart->checkState() == Qt::Checked) {
    if (!QDir().exists(path))
      QDir().mkpath(path);

    QFile file(path + "schat.desktop");
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
      QTextStream out(&file);
      out << "[Desktop Entry]" << endl;
      out << "Version=1.0" << endl;
      out << "Type=Application" << endl;
      out << "Name=IMPOMEZIA Simple Chat" << endl;
      out << "Comment=" << endl;
      out << "Icon=schat" << endl;
      out << "Exec=schat -hide" << endl;
      out << "Hidden=false" << endl;
      out << "NoDisplay=false" << endl;
      out << "X-GNOME-Autostart-enabled=true" << endl;
      file.close();
    }
  }
  else if (autostart->checkState() == Qt::Unchecked) {
    QFile::remove(path + "schat.desktop");
  }
  #endif
}


/*!
 * \brief Конструктор класса MiscSettings.
 */
MiscSettings::MiscSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::MiscPage, parent), d(new Private)
{
  QGroupBox *language = new QGroupBox(tr("Language"), this);
  d->language = new QComboBox(this);
  d->language->setIconSize(QSize(25, 15));
  d->language->addItem(QIcon(":/translations/schat_en.png"), "English", ":/translations/schat_en.qm");

  d->translations();

  QHBoxLayout *languageLay = new QHBoxLayout(language);
  languageLay->addWidget(d->language);
  languageLay->addStretch();
  languageLay->setMargin(6);
  languageLay->setSpacing(4);

  QGroupBox *integration = new QGroupBox(tr("Интеграция"), this);
  #if defined(Q_OS_MAC)
  integration->setVisible(false);
  #endif

  d->autostart = new QCheckBox(tr("&Автозапуск"), this);
  d->autostart->setToolTip(tr("Автозапуск программы при старте системы"));
  d->autostart->setTristate();
  d->autostart->setCheckState(Qt::PartiallyChecked);

  #ifdef Q_WS_WIN
  d->autostartDaemon = new QCheckBox(tr("Автозапуск &сервера"), this);
  d->autostartDaemon->setToolTip(tr("Автозапуск сервера при старте системы"));
  d->autostartDaemon->setTristate();
  d->autostartDaemon->setCheckState(Qt::PartiallyChecked);
  #endif

  QVBoxLayout *integrationLay = new QVBoxLayout(integration);
  integrationLay->addWidget(d->autostart);
  #ifdef Q_WS_WIN
  integrationLay->addWidget(d->autostartDaemon);
  #endif
  integrationLay->setMargin(6);
  integrationLay->setSpacing(4);

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
  logLay->setMargin(6);
  logLay->setSpacing(4);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(language);
  mainLay->addSpacing(12);
  mainLay->addWidget(integration);
  #if !defined(Q_OS_MAC)
  mainLay->addSpacing(12);
  #endif
  mainLay->addWidget(logGroup);
  mainLay->addStretch();
  mainLay->setContentsMargins(3, 3, 3, 0);

  d->readAutostart();
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
  d->writeAutostart();

  SimpleSettings->setBool("Log", d->log->isChecked());
  SimpleSettings->setBool("LogPrivate", d->logPrivate->isChecked());
  SimpleSettings->notify(Settings::MiscSettingsChanged);
}
