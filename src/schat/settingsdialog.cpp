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

#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <QPushButton>
#include <QSound>
#include <QSpinBox>
#include <QStyleFactory>
#include <QTextStream>

#include "abstractprofile.h"
#include "emoticons/emoticons.h"
#include "languagebox.h"
#include "profilewidget.h"
#include "settings.h"
#include "settingsdialog.h"
#include "simplechatapp.h"
#include "translation.h"
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

  createPage(QIcon(":/images/profile.png"),               tr("Personal data"), profilePage);
  createPage(QIcon(":/images/network.png"),               tr("Network"),       networkPage);
  createPage(QIcon(":/images/applications-graphics.png"), tr("Interface"),     new InterfaceSettings(this));
  createPage(QIcon(":/images/emoticon.png"),              tr("Emoticons"),     new EmoticonsSettings(this));
  createPage(QIcon(":/images/sound.png"),                 tr("Sounds"),        new SoundSettings(this));
  createPage(QIcon(":/images/notification.png"),          tr("Notifications"), new NotificationSettings(this));
  createPage(QIcon(":/images/statuses.png"),              tr("Statuses"),      new StatusesSettings(this));
  createPage(QIcon(":/images/update.png"),                tr("Update"),        new UpdateSettings(this));
  createPage(QIcon(":/images/application-x-desktop.png"), tr("Others"),        new MiscSettings(this));

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
  d->welcome = new QCheckBox(tr("Always use this connection"), this);
  d->welcome->setChecked(SimpleSettings->getBool("HideWelcome"));
  d->welcome->setToolTip(tr("Do not require personal information and server address at startup"));

  d->network = new NetworkWidget(this);
  connect(d->network, SIGNAL(validServer(bool)), SIGNAL(validServer(bool)));

  QGroupBox *serverGroup = new QGroupBox(tr("Connection"), this);
  QVBoxLayout *serverLay = new QVBoxLayout(serverGroup);
  serverLay->addWidget(d->network);
  serverLay->addWidget(d->welcome);
  serverLay->setMargin(6);
  serverLay->setSpacing(4);

  d->proxyGroup = new QGroupBox(tr("Connection via proxy server"), this);
  d->proxyGroup->setCheckable(true);
  d->proxyGroup->setChecked(SimpleSettings->getBool("Proxy/Enable"));
  d->proxyGroup->setVisible(!SimpleSettings->getBool("Proxy/HideAndDisable"));

  d->type = new QComboBox(this);
  d->type->addItem("HTTP");
  d->type->addItem("SOCKS5");
  d->type->setToolTip(tr("Proxy server type"));

  if (SimpleSettings->getInt("Proxy/Type") == 1)
    d->type->setCurrentIndex(1);
  else
    d->type->setCurrentIndex(0);

  QLabel *type = new QLabel(tr("&Type:"), this);
  type->setBuddy(d->type);

  d->host = new QLineEdit(SimpleSettings->getString("Proxy/Host"), this);
  d->host->setToolTip(tr("Proxy server address"));
  QLabel *host = new QLabel(tr("&Address:"), this);
  host->setBuddy(d->host);

  d->port = new QSpinBox(this);
  d->port->setRange(1, 65536);
  d->port->setToolTip(tr("Proxy server port"));
  d->port->setValue(SimpleSettings->getInt("Proxy/Port"));
  QLabel *port = new QLabel(tr("&Port:"), this);
  port->setBuddy(d->port);

  d->userName = new QLineEdit(SimpleSettings->getString("Proxy/UserName"), this);
  d->userName->setToolTip(tr("User name for authorization at proxy server"));
  QLabel *userName = new QLabel(tr("&Name:"), this);
  userName->setBuddy(d->userName);

  d->password = new QLineEdit(SimpleSettings->getString("Proxy/Password"), this);
  d->password->setEchoMode(QLineEdit::Password);
  d->password->setToolTip(tr("Password for authorization at proxy server"));
  QLabel *password = new QLabel(tr("P&assword:"), this);
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

  QLabel *url = new QLabel(QString("<a style='text-decoration:none; color:#1a4d82;' href='#'>%1</a>").arg(tr("Network files")), this);
  url->setToolTip(tr("Open folder with network files"));
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
  if (SimpleSettings->getBool("HideWelcome") != d->welcome->isChecked()) {
    SimpleSettings->setBool("HideWelcome", d->welcome->isChecked());
    SimpleSettings->notify(Settings::HideWelcomeChanged);
  }

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
  LanguageBox *language;

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
  chatStyleVariant->addItem(tr("(without variants)"));

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

  QGroupBox *mainStyleGroup = new QGroupBox(tr("&Global style"), this);
  QHBoxLayout *mainStyleLay = new QHBoxLayout(mainStyleGroup);
  mainStyleLay->addWidget(d->mainStyle);
  mainStyleLay->addStretch();
  mainStyleLay->setMargin(6);
  mainStyleLay->setSpacing(4);
  #endif

  QGroupBox *language = new QGroupBox(tr("Language"), this);
  d->language = new LanguageBox(CURRENT_LANG, "schat_", SimpleSettings->path(Settings::TranslationsPath), this);

  QHBoxLayout *languageLay = new QHBoxLayout(language);
  languageLay->addWidget(d->language);
  languageLay->addStretch();
  languageLay->setMargin(6);
  languageLay->setSpacing(4);

  #if !defined(SCHAT_NO_STYLE)
  QHBoxLayout *topLay = new QHBoxLayout;
  topLay->addWidget(mainStyleGroup, 0);
  topLay->addWidget(language, 1);
  #endif

  #ifndef SCHAT_NO_WEBKIT
    d->chatStyle = new QComboBox(this);
    d->chatStyle->addItem("Default");
    QLabel *name = new QLabel(tr("&Style name:"), this);
    name->setBuddy(d->chatStyle);

    d->chatStyleVariant = new QComboBox(this);
    QLabel *variant = new QLabel(tr("&Variant:"), this);
    variant->setBuddy(d->chatStyleVariant);

    d->grouping = new QCheckBox(tr("Group &consecutive messages"), this);
    d->grouping->setToolTip(tr("Group consecutive messages from one user, if the chosen style supports this option"));
    d->grouping->setChecked(SimpleSettings->getBool("MessageGrouping"));

    QGroupBox *chatStyleGroup = new QGroupBox(tr("&Text style"), this);
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
  mainLay->addLayout(topLay);
  #else
  mainLay->addWidget(language);
  #endif
  #ifndef SCHAT_NO_WEBKIT
    mainLay->addSpacing(12);
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

  if (d->language->currentText() != CURRENT_LANG) {
    Translation *translation = SimpleChatApp::instance()->translation();
    translation->load(d->language->qmFile());
    SimpleSettings->setString("Translation", translation->name());
  }
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

  QCheckBox *requireSpaces;
  QComboBox *combo;
  QGroupBox *group;
};


bool EmoticonsSettings::Private::createThemeList()
{
  combo->addItems(Emoticons::themeList());

  if (combo->count() == 0)
    return false;

  combo->setCurrentIndex(combo->findText(SimpleSettings->getString("EmoticonTheme")));
  if (combo->currentIndex() == -1) {
    combo->setCurrentIndex(combo->findText("Kolobok"));
    if (combo->currentIndex() == -1) {
      combo->setCurrentIndex(0);
    }
  }

  return true;
}


/*!
 * \brief Конструктор класса EmoticonsSettings.
 */
EmoticonsSettings::EmoticonsSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::EmoticonsPage, parent), d(new Private)
{
  d->combo = new QComboBox(this);

  d->group = new QGroupBox(tr("&Emoticons"), this);
  d->group->setCheckable(true);
  d->group->setChecked(SimpleSettings->getBool("UseEmoticons"));
  connect(d->group, SIGNAL(clicked(bool)), SLOT(enable(bool)));

  QHBoxLayout *themeLay = new QHBoxLayout(d->group);
  themeLay->addWidget(new QLabel(tr("Theme:"), this));
  themeLay->addWidget(d->combo);
  themeLay->addStretch();
  themeLay->setMargin(6);
  themeLay->setSpacing(4);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(d->group);

  d->requireSpaces = new QCheckBox(tr("Emoticons are &separated by spaces"), this);
  d->requireSpaces->setToolTip(tr("Show emoticons only if they are separated by spaces from the rest of the message"));
  d->requireSpaces->setChecked(Emoticons::strictParse());

  QLabel *url = new QLabel(QString("<a style='text-decoration:none; color:#1a4d82;' href='#'>%1</a>").arg(tr("Emoticons themes")), this);
  url->setToolTip(tr("Open folder with emoticons themes"));
  url->setAlignment(Qt::AlignRight);
  connect(url, SIGNAL(linkActivated(const QString &)), SLOT(openFolder()));

  mainLay->addWidget(d->requireSpaces);
  mainLay->addStretch();
  mainLay->addWidget(url);
  mainLay->setSpacing(4);
  mainLay->setContentsMargins(3, 3, 3, 0);

  if (!d->createThemeList()) {
    d->group->setEnabled(false);
    d->group->setChecked(false);
    SimpleSettings->setBool("UseEmoticons", false);
  }

  enable(d->group->isChecked());
}


EmoticonsSettings::~EmoticonsSettings() { delete d; }


void EmoticonsSettings::reset(int page)
{
  if (page == m_id) {
    d->group->setChecked(true);
    d->requireSpaces->setChecked(true);
    d->combo->setCurrentIndex(d->combo->findText("Kolobok"));
    enable(true);
  }
}


void EmoticonsSettings::save()
{
  SimpleSettings->setBool("UseEmoticons", d->group->isChecked());
  Emoticons::setStrictParse(d->requireSpaces->isChecked());
  SimpleSettings->setString("EmoticonTheme", d->combo->currentText());
  SimpleSettings->notify(Settings::EmoticonsChanged);
}


void EmoticonsSettings::enable(bool checked)
{
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




class StatusesSettings::Private
{
public:
  Private() {}
};


/*!
 * \brief Конструктор класса StatusesSettings.
 */
StatusesSettings::StatusesSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::StatusesPage, parent), d(new Private)
{

}


StatusesSettings::~StatusesSettings() { delete d; }


void StatusesSettings::reset(int page)
{
  if (page == m_id) {

  }
}


void StatusesSettings::save()
{

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

  QString minute = tr("%n Minute", "", value);
  QString hour   = tr("%n Hour", "", value);

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
  void readAutostart();
  void writeAutostart();

  QCheckBox *autostart;
  QCheckBox *log;
  QCheckBox *logPrivate;

  #ifdef Q_WS_WIN
  QCheckBox *autostartDaemon;
  #endif
};


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
  QGroupBox *integration = new QGroupBox(tr("Integration"), this);
  #if defined(Q_OS_MAC)
  integration->setVisible(false);
  #endif

  d->autostart = new QCheckBox(tr("&Autorun"), this);
  d->autostart->setToolTip(tr("Autorun at system startup"));
  d->autostart->setTristate();
  d->autostart->setCheckState(Qt::PartiallyChecked);

  #ifdef Q_WS_WIN
  d->autostartDaemon = new QCheckBox(tr("&Server autorun"), this);
  d->autostartDaemon->setToolTip(tr("Server autorun at system startup"));
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

  QGroupBox *logGroup = new QGroupBox(tr("&Logging"), this);

  d->log = new QCheckBox(tr("&Main channel log"), this);
  d->log->setToolTip(tr("Keep a log of main channel"));
  d->log->setChecked(SimpleSettings->getBool("Log"));

  d->logPrivate = new QCheckBox(tr("&Private logs"), this);
  d->logPrivate->setToolTip(tr("Keep logs of private messages"));
  d->logPrivate->setChecked(SimpleSettings->getBool("LogPrivate"));

  QVBoxLayout *logLay = new QVBoxLayout(logGroup);
  logLay->addWidget(d->log);
  logLay->addWidget(d->logPrivate);
  logLay->setMargin(6);
  logLay->setSpacing(4);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
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
