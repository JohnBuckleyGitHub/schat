/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "networkwidget.h"
#include "profilewidget.h"
#include "schatwindow.h"
#include "settings.h"
#include "settingsdialog.h"


/** [SettingsDialog/public]
 * Конструктор SettingsDialog
 */
SettingsDialog::SettingsDialog(Profile *p, Settings *s, QWidget *parent)
  : QDialog(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);
  
  resize(500, 256);
  
  chat           = static_cast<SChatWindow *>(parent);
  profile        = p;
  settings       = s;
  okButton       = new QPushButton(QIcon(":/images/ok.png"), tr("OK"), this);
  cancelButton   = new QPushButton(QIcon(":/images/cancel.png"), tr("Отмена"), this);
  resetButton    = new QPushButton(QIcon(":/images/undo.png"), "", this);  
  contentsWidget = new QListWidget(this);
  pagesWidget    = new QStackedWidget;

  profilePage       = new ProfileSettings(chat, profile, this);
  networkPage       = new NetworkSettings(chat, settings, this);
  interfaceSettings = new InterfaceSettings(settings, this);
  
  resetButton->setToolTip(tr("Вернуть настройки по умолчанию"));
  pagesWidget->addWidget(profilePage);
  pagesWidget->addWidget(networkPage);
  pagesWidget->addWidget(interfaceSettings);
  
  QFrame *line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  
  new QListWidgetItem(QIcon(":/images/profile.png"), tr("Личные данные"), contentsWidget);
  new QListWidgetItem(QIcon(":/images/network.png"), tr("Сеть"), contentsWidget);
  new QListWidgetItem(QIcon(":/images/appearance.png"), tr("Интерфейс"), contentsWidget);
  
  connect(contentsWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));
  connect(profilePage, SIGNAL(validNick(bool)), this, SLOT(validNick(bool)));
  
  contentsWidget->setCurrentRow(ProfilePage);
  
  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch();
  buttonLayout->addWidget(resetButton);
  buttonLayout->addWidget(okButton);
  buttonLayout->addWidget(cancelButton);
  buttonLayout->setSpacing(3);
  
  QGridLayout *mainLayout = new QGridLayout(this);
  mainLayout->setColumnStretch(0, 1);
  mainLayout->setColumnStretch(1, 3);
  mainLayout->addWidget(contentsWidget, 0, 0);
  mainLayout->addWidget(pagesWidget, 0, 1);
  mainLayout->addWidget(line, 1, 0, 1, 2);
  mainLayout->addLayout(buttonLayout, 2, 0, 1, 2);
  mainLayout->setMargin(3);
  mainLayout->setSpacing(3);
  
  setWindowTitle(tr("Настройка"));
}


/** [SettingsDialog/public]
 * 
 */
void SettingsDialog::setPage(int page)
{
  contentsWidget->setCurrentRow(page);
  pagesWidget->setCurrentIndex(page);
}


/** [SettingsDialog/public slots]
 * Вызаваем сохранение настроек и закрываем диалог
 */
void SettingsDialog::accept()
{
  profilePage->save();
  networkPage->save();
  interfaceSettings->save();
  close();
}


/** [SettingsDialog/public slots]
 * 
 */
void SettingsDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
  if (!current)
    current = previous;

  pagesWidget->setCurrentIndex(contentsWidget->row(current));
}


/** [SettingsDialog/public slots]
 * 
 */
void SettingsDialog::reset()
{
  switch (pagesWidget->currentIndex()) {
    case ProfilePage:
      profilePage->reset();      
      break;
      
    case NetworkPage:
      networkPage->reset();
      break;
      
    case InterfacePage:
      interfaceSettings->reset();
      break;
  }  
}



/** [ProfileSettings/public]
 * Конструктор `ProfileSettings`
 */
ProfileSettings::ProfileSettings(SChatWindow *w, Profile *p, QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  chat          = w;
  profileWidget = new ProfileWidget(p, this);
  connect(profileWidget, SIGNAL(validNick(bool)), this, SIGNAL(validNick(bool)));
  
  QGroupBox *profileGroupBox = new QGroupBox(tr("Профиль"), this);
  QVBoxLayout *profileGroupLayout = new QVBoxLayout(profileGroupBox);
  profileGroupLayout->addWidget(profileWidget);
  
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(profileGroupBox);
  mainLayout->addStretch();
}


/** [ProfileSettings/public]
 * 
 */
void ProfileSettings::reset()
{
  profileWidget->reset();
}


/** [ProfileSettings/public]
 * Сохраняем настройки
 */
void ProfileSettings::save()
{
  profileWidget->save();
  
  if (profileWidget->isModifiled())
    chat->reconnect();
}




/** [NetworkSettings/public]
 * Конструктор `NetSettingsPage`
 */
NetworkSettings::NetworkSettings(SChatWindow *w, Settings *s, QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  chat              = w;
  settings          = s;
  QLabel *addrLabel = new QLabel(tr("Адрес:"), this);
  QLabel *portLabel = new QLabel(tr("Порт:"), this);
  serverEdit        = new QLineEdit(settings->network.server(), this);
  port              = new QSpinBox(this);
  welcomeCheckBox   = new QCheckBox(tr("Всегда использовать этот сервер"), this);
  
  serverEdit->setToolTip(tr("Адрес сервера"));
  port->setRange(1, 65536);
  port->setValue(settings->network.port());
  port->setToolTip(tr("Порт сервера, по умолчанию <b>7666</b>"));
  welcomeCheckBox->setChecked(settings->hideWelcome);
  welcomeCheckBox->setToolTip(tr("Не запрашивать персональную информацию и адрес сервера при запуске программы"));
  
  m_networkWidget = new NetworkWidget(settings, this);
  
  QHBoxLayout *networkLayout = new QHBoxLayout;
  networkLayout->addWidget(m_networkWidget);
  networkLayout->addStretch();
  networkLayout->setMargin(0);
  
  QHBoxLayout *serverLayout = new QHBoxLayout;
  serverLayout->addWidget(addrLabel);
  serverLayout->addWidget(serverEdit);
  serverLayout->addWidget(portLabel);
  serverLayout->addWidget(port);
  serverLayout->addStretch();
  
  QGroupBox *serverGroupBox = new QGroupBox(tr("Сервер"), this);
  QVBoxLayout *serverGroupLayout = new QVBoxLayout(serverGroupBox);
  serverGroupLayout->addLayout(networkLayout);
  serverGroupLayout->addLayout(serverLayout);
  serverGroupLayout->addWidget(welcomeCheckBox);
  
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(serverGroupBox);
  mainLayout->addStretch();
}


/** [NetworkSettings/public]
 * 
 */
void NetworkSettings::reset()
{
  serverEdit->setText("192.168.5.130");
  port->setValue(7666);
  welcomeCheckBox->setChecked(true);
}


/** [NetworkSettings/public]
 * Сохраняем настройки
 */
void NetworkSettings::save()
{
  if ((settings->network.server() != serverEdit->text()) || (settings->network.port() != quint16(port->value()))) {
//    settings->server = serverEdit->text(); // ЭТО НЕ РАБОТАЕТ
//    settings->serverPort = quint16(port->value());
    chat->reconnect();
  }
  welcomeCheckBox->isChecked() ? settings->hideWelcome = true : settings->hideWelcome = false;
}




/** [InterfaceSettings/public]
 * Конструктор `InterfaceSettings`
 */
InterfaceSettings::InterfaceSettings(Settings *s, QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  settings = s;
  
  styleComboBox = new QComboBox(this);
  styleComboBox->addItems(QStyleFactory::keys());  
  styleComboBox->setCurrentIndex(styleComboBox->findText(settings->style));
  
  QGroupBox *styleGroupBox = new QGroupBox(tr("Внешний вид"), this);
  QHBoxLayout *styleGroupLayout = new QHBoxLayout(styleGroupBox);
  styleGroupLayout->addWidget(styleComboBox);
  styleGroupLayout->addStretch();
  
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(styleGroupBox);
  mainLayout->addStretch();
}


/** [InterfaceSettings/public]
 * 
 */
void InterfaceSettings::reset()
{
  styleComboBox->setCurrentIndex(styleComboBox->findText("Plastique"));
}


/** [InterfaceSettings/public]
 * Сохраняем настройки
 */
void InterfaceSettings::save()
{
  if (styleComboBox->currentIndex() != -1) {
    settings->style = styleComboBox->currentText();
    qApp->setStyle(settings->style);
  }
}
