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
  QLineEdit *byeMsgEdit;
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
  d->byeMsgEdit->setToolTip(tr("The message is seen by the other users if you are left of the chat"));
  if (d->maxRecentItems) {
    d->byeMsgEdit->setCompleter(new QCompleter(SimpleSettings->getList("Profile/RecentByeMsgs"), this));
    d->byeMsgEdit->completer()->setCaseSensitivity(Qt::CaseInsensitive);
  }
  QLabel *byeMsgLabel = new QLabel(tr("Message at the exit:"), this);
  byeMsgLabel->setBuddy(d->byeMsgEdit);

  QGroupBox *profileGroup = new QGroupBox(tr("Profile"), this);
  QVBoxLayout *profileLay = new QVBoxLayout(profileGroup);
  profileLay->setMargin(6);
  profileLay->setSpacing(4);
  profileLay->addWidget(d->profileWidget);
  profileLay->addWidget(byeMsgLabel);
  profileLay->addWidget(d->byeMsgEdit);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(profileGroup);
  mainLay->addStretch();
  mainLay->setContentsMargins(3, 3, 3, 0);
}

ProfileSettings::~ProfileSettings() { delete d; }


void ProfileSettings::reset(int page)
{
  if (page == m_id) {
    d->profileWidget->reset();
    d->byeMsgEdit->setText("");
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
  d->language = new LanguageBox(SimpleChatApp::instance()->translation(), this);

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

  if (d->language->save()) {
    SimpleSettings->setString("Translation", SimpleChatApp::instance()->translation()->name());
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
  d->enable = new QGroupBox(tr("Sound notifications"), this);
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

  d->msg = new SoundWidget("Message", tr("Message"), tr("Message to main channel"), list, this);
  connect(d->msg, SIGNAL(play(const QString &)), SLOT(play(const QString &)));

  d->privateMsg = new SoundWidget("PrivateMessage", tr("Private message"), tr("Private message from another user"), list, this);
  connect(d->privateMsg, SIGNAL(play(const QString &)), SLOT(play(const QString &)));

  QVBoxLayout *soundLay = new QVBoxLayout(d->enable);
  soundLay->addWidget(d->msg);
  soundLay->addWidget(d->privateMsg);
  soundLay->setMargin(6);
  soundLay->setSpacing(0);

  #ifdef Q_WS_X11
    d->useCmd = new QGroupBox(tr("External command for sound play"), this);
    d->useCmd->setCheckable(true);
    d->useCmd->setChecked(SimpleSettings->getBool("Sound/UseExternalCmd"));
    d->cmd = new QLineEdit(SimpleSettings->getString("Sound/ExternalCmd"), this);
    d->cmd->setToolTip(tr("External command, instead of %1 sound file name is inserted"));
    QVBoxLayout *cmdLay = new QVBoxLayout(d->useCmd);
    cmdLay->addWidget(d->cmd);
  #endif

  QLabel *url = new QLabel(QString("<a style='text-decoration:none; color:#1a4d82;' href='#'>%1</a>").arg(tr("Sounds")), this);
  url->setToolTip(tr("Open folder with sounds"));
  url->setAlignment(Qt::AlignRight);
  connect(url, SIGNAL(linkActivated(const QString &)), SLOT(openFolder()));

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(d->enable);
  #ifdef Q_WS_X11
    mainLay->addWidget(d->useCmd);
  #endif
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
  d->privateMsg = new QCheckBox(tr("Private message"), this);
  d->privateMsg->setToolTip(tr("Notification of private messages"));
  d->privateMsg->setChecked(SimpleSettings->getBool("Notification"));

  d->publicMsg = new QCheckBox(tr("Personal message in main channel"), this);
  d->publicMsg->setToolTip(tr("Notification of personal messages by nick in the main channel"));
  d->publicMsg->setChecked(SimpleSettings->getBool("NotificationPublic"));

  QGroupBox *eventGroup = new QGroupBox(tr("Notification of events"), this);
  QVBoxLayout *eventLay = new QVBoxLayout(eventGroup);
  eventLay->addWidget(d->privateMsg);
  eventLay->addWidget(d->publicMsg);
  eventLay->setMargin(6);
  eventLay->setSpacing(0);

  d->timeOut = new QCheckBox(tr("Automatically close after:"), this);
  d->timeOut->setToolTip(tr("Automatically close notification windows after a set number of seconds"));
  d->timeOut->setChecked(SimpleSettings->getBool("PopupAutoClose"));

  d->timeOutSpin = new QSpinBox(this);
  d->timeOutSpin->setRange(1, 1440);
  d->timeOutSpin->setSuffix(tr(" sec"));
  d->timeOutSpin->setValue(SimpleSettings->getInt("PopupAutoCloseTime"));

  d->autoAway = new QCheckBox(tr("But not with automatic \"Away\" status"), this);
  d->autoAway->setToolTip(tr("Do not close automatic pop-up windows at automatic \"Away\" status"));
  d->autoAway->setChecked(SimpleSettings->getBool("NoPopupAutoCloseInAway"));

  d->popupGroup = new QGroupBox(tr("Notification window settings"), this);
  QGridLayout *popupLay = new QGridLayout(d->popupGroup);
  popupLay->addWidget(d->timeOut, 0, 0, 1, 2);
  popupLay->addWidget(d->timeOutSpin, 0, 2);
  popupLay->addItem(new QSpacerItem(20, 0), 1, 0);
  popupLay->addWidget(d->autoAway, 1, 1, 1, 2);
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

  QCheckBox *autoAway;
  QCheckBox *dnd;
  QCheckBox *exitAwayOnSend;
  QCheckBox *muteInDnD;
  QComboBox *statuses;
  QSpinBox *autoAwayTime;
};


/*!
 * \brief Конструктор класса StatusesSettings.
 */
StatusesSettings::StatusesSettings(QWidget *parent)
  : AbstractSettingsPage(SettingsDialog::StatusesPage, parent), d(new Private)
{
  d->exitAwayOnSend = new QCheckBox(tr("Reset the status after sending the message"), this);
  d->exitAwayOnSend->setChecked(SimpleSettings->getBool("ExitAwayOnSend"));

  QLabel *statusLabel = new QLabel(tr("&Status:"), this);
  d->statuses = new QComboBox(this);
  d->statuses->addItem(tr("Away"));
  d->statuses->addItem(tr("Do Not Disturb"));
  connect(d->statuses, SIGNAL(currentIndexChanged(int)), this, SLOT(showOptions(int)));

  statusLabel->setBuddy(d->statuses);

  QGroupBox *group = new QGroupBox(tr("Status options"), this);

  d->autoAway = new QCheckBox(tr("Switch on the status after idle time:"), this);
  d->autoAway->setToolTip(tr("Turn automatically status Away after idle time and return to the standard mode if the activity occurred"));
  d->autoAway->setChecked(SimpleSettings->getBool("AutoAway"));

  d->autoAwayTime = new QSpinBox(this);
  d->autoAwayTime->setRange(1, 1440);
  d->autoAwayTime->setSuffix(tr(" min"));
  d->autoAwayTime->setValue(SimpleSettings->getInt("AutoAwayTime"));

  d->muteInDnD = new QCheckBox(tr("The status switches off the sound"), this);
  d->muteInDnD->setToolTip(tr("Use of “Do Not Disturb” status switches off the sound"));
  d->muteInDnD->setChecked(SimpleSettings->getBool("Sound/MuteInDnD"));

  d->dnd = new QCheckBox(tr("The status switches off the notification windows"), this);
  d->dnd->setToolTip(tr("Use of “Do Not Disturb” status switches off the notification windows"));
  d->dnd->setChecked(SimpleSettings->getBool("NoNotificationInDnD"));

  connect(d->autoAway, SIGNAL(clicked(bool)), d->autoAwayTime, SLOT(setEnabled(bool)));
  d->autoAwayTime->setEnabled(d->autoAway->isChecked());

  QGridLayout *groupLay = new QGridLayout(group);
  groupLay->addWidget(d->autoAway, 0, 0);
  groupLay->addWidget(d->autoAwayTime, 0, 1);
  groupLay->addWidget(d->muteInDnD, 1, 0, 1, 2);
  groupLay->addWidget(d->dnd, 2, 0, 1, 2);
  groupLay->setColumnStretch(0, 1);
  groupLay->setMargin(6);
  groupLay->setSpacing(4);

  QHBoxLayout *statusesLay = new QHBoxLayout;
  statusesLay->addWidget(statusLabel);
  statusesLay->addWidget(d->statuses);
  statusesLay->addStretch();
  statusesLay->setMargin(6);
  statusesLay->setSpacing(4);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(d->exitAwayOnSend);
  mainLay->addSpacing(12);
  mainLay->addLayout(statusesLay);
  mainLay->addWidget(group);
  mainLay->addStretch();
  mainLay->setContentsMargins(3, 3, 3, 0);

  showOptions(0);
}


StatusesSettings::~StatusesSettings() { delete d; }


void StatusesSettings::reset(int page)
{
  if (page == m_id) {
    d->autoAway->setChecked(true);
    d->autoAwayTime->setValue(10);
    d->autoAwayTime->setEnabled(true);
    d->exitAwayOnSend->setChecked(true);
    d->muteInDnD->setChecked(true);
    d->dnd->setChecked(true);
  }
}


void StatusesSettings::save()
{
  int modified = 0;
  modified += SimpleSettings->save("AutoAway", d->autoAway->isChecked());
  modified += SimpleSettings->save("AutoAwayTime", d->autoAwayTime->value());
  modified += SimpleSettings->save("ExitAwayOnSend", d->exitAwayOnSend->isChecked());

  if (modified)
    SimpleSettings->notify(Settings::AwaySettingsChanged);

  if (SimpleSettings->save("Sound/MuteInDnD", d->muteInDnD->isChecked()))
    SimpleSettings->notify(Settings::SoundChanged);

  if (SimpleSettings->save("NoNotificationInDnD", d->dnd->isChecked()))
    SimpleSettings->notify(Settings::NotificationChanged);
}


void StatusesSettings::showOptions(int index)
{
  if (index == 0) {
    d->autoAway->setVisible(true);
    d->autoAwayTime->setVisible(true);
    d->muteInDnD->setVisible(false);
    d->dnd->setVisible(false);
  }
  else {
    d->autoAway->setVisible(false);
    d->autoAwayTime->setVisible(false);
    d->muteInDnD->setVisible(true);
    d->dnd->setVisible(true);
  }
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

  #if defined(SCHAT_NO_UPDATE)
  QCheckBox *updateGroup;
  #else
  QGroupBox *updateGroup;
  QCheckBox *autoDownload;
  QCheckBox *upgrade;
  #endif

  #if defined(Q_WS_WIN)
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

  #if defined(SCHAT_NO_UPDATE)
  d->updateGroup = new QCheckBox(tr("Checks for updates"), this);
  #else
  d->updateGroup = new QGroupBox(tr("Checks for updates"), this);
  #endif
  d->updateGroup->setCheckable(true);
  d->updateGroup->setChecked(SimpleSettings->getBool("Updates/Enable"));

# if !defined(SCHAT_NO_UPDATE)
  d->autoDownload = new QCheckBox(tr("Automatically download updates"), this);
  d->autoDownload->setChecked(SimpleSettings->getBool("Updates/AutoDownload"));
  d->upgrade = new QCheckBox(tr("Allow upgrade to version 2"), this);
  d->upgrade->setChecked(SimpleSettings->getBool("Migrate/Wizard"));

  QVBoxLayout *updateLay = new QVBoxLayout(d->updateGroup);
  updateLay->addWidget(d->autoDownload);
  updateLay->addWidget(d->upgrade);
  updateLay->setContentsMargins(16, 6, 6, 6);
  updateLay->setSpacing(0);
# endif

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(integration);
# if !defined(Q_OS_MAC)
  mainLay->addSpacing(12);
# endif
  mainLay->addWidget(logGroup);
  mainLay->addSpacing(12);
  mainLay->addWidget(d->updateGroup);
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
    d->updateGroup->setChecked(true);

#   if !defined(SCHAT_NO_UPDATE)
    d->autoDownload->setChecked(false);
    d->upgrade->setChecked(true);
#   endif
  }
}


void MiscSettings::save()
{
  d->writeAutostart();

  SimpleSettings->setBool("Log", d->log->isChecked());
  SimpleSettings->setBool("LogPrivate", d->logPrivate->isChecked());
  SimpleSettings->setBool("Updates/Enable", d->updateGroup->isChecked());

# if !defined(SCHAT_NO_UPDATE)
  SimpleSettings->setBool("Updates/AutoDownload", d->autoDownload->isChecked());
  SimpleSettings->setBool("Migrate/Wizard",       d->upgrade->isChecked());
# endif

  SimpleSettings->notify(Settings::MiscSettingsChanged);
}
