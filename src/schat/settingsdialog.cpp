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
SettingsDialog::SettingsDialog(Profile *profile, Settings *settings, QWidget *parent)
  : QDialog(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);
  
  m_okButton       = new QPushButton(QIcon(":/images/ok.png"), tr("OK"), this);
  m_cancelButton   = new QPushButton(QIcon(":/images/cancel.png"), tr("Отмена"), this);
  m_resetButton    = new QPushButton(QIcon(":/images/undo.png"), "", this);  
  m_contentsWidget = new QListWidget(this);
  m_pagesWidget    = new QStackedWidget;

  m_profilePage    = new ProfileSettings(settings, profile, this);
  m_networkPage    = new NetworkSettings(settings, this);
  m_interfacePage  = new InterfaceSettings(settings, this);
  
  m_resetButton->setToolTip(tr("Вернуть настройки по умолчанию"));
  m_pagesWidget->addWidget(m_profilePage);
  m_pagesWidget->addWidget(m_networkPage);
  m_pagesWidget->addWidget(m_interfacePage);

  #ifdef SCHAT_UPDATE
  m_updatePage = new UpdateSettings(settings, this);
  m_pagesWidget->addWidget(m_updatePage);
  #endif
  
  QFrame *line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  
  new QListWidgetItem(QIcon(":/images/profile.png"), tr("Личные данные"), m_contentsWidget);
  new QListWidgetItem(QIcon(":/images/network.png"), tr("Сеть"), m_contentsWidget);
  new QListWidgetItem(QIcon(":/images/appearance.png"), tr("Интерфейс"), m_contentsWidget);
  
  #ifdef SCHAT_UPDATE
  new QListWidgetItem(QIcon(":/images/update.png"), tr("Обновления"), m_contentsWidget);
  #endif
  
  connect(m_contentsWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
  connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(m_resetButton, SIGNAL(clicked()), this, SLOT(reset()));
  connect(m_profilePage, SIGNAL(validNick(bool)), this, SLOT(validNick(bool)));
  
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


/** [SettingsDialog/public]
 * 
 */
void SettingsDialog::setPage(int page)
{
  m_contentsWidget->setCurrentRow(page);
  m_pagesWidget->setCurrentIndex(page);
}


/** [SettingsDialog/public slots]
 * Вызаваем сохранение настроек и закрываем диалог
 */
void SettingsDialog::accept()
{
  m_profilePage->save();
  m_networkPage->save();
  m_interfacePage->save();
  
  #ifdef SCHAT_UPDATE
  m_updatePage->save();
  #endif
  close();
}


/** [SettingsDialog/public slots]
 * 
 */
void SettingsDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
  if (!current)
    current = previous;

  m_pagesWidget->setCurrentIndex(m_contentsWidget->row(current));
}


/** [SettingsDialog/public slots]
 * 
 */
void SettingsDialog::reset()
{
  switch (m_pagesWidget->currentIndex()) {
    case ProfilePage:
      m_profilePage->reset();      
      break;
      
    case NetworkPage:
      m_networkPage->reset();
      break;
      
    case InterfacePage:
      m_interfacePage->reset();
      break;
    
    #ifdef SCHAT_UPDATE
    case UpdatePage:
      m_updatePage->reset();
      break;
    #endif
  }  
}



/** [ProfileSettings/public]
 * Конструктор `ProfileSettings`
 */
ProfileSettings::ProfileSettings(Settings *settings, Profile *profile, QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  m_settings = settings;
  m_profileWidget = new ProfileWidget(profile, this);
  connect(m_profileWidget, SIGNAL(validNick(bool)), this, SIGNAL(validNick(bool)));
  
  QGroupBox *profileGroupBox = new QGroupBox(tr("Профиль"), this);
  QVBoxLayout *profileGroupLayout = new QVBoxLayout(profileGroupBox);
  profileGroupLayout->addWidget(m_profileWidget);
  
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(profileGroupBox);
  mainLayout->addStretch();
}


/** [ProfileSettings/public]
 * 
 */
void ProfileSettings::reset()
{
  m_profileWidget->reset();
}


/** [ProfileSettings/public]
 * Сохраняем настройки
 */
void ProfileSettings::save()
{
  m_profileWidget->save();
  
  if (m_profileWidget->isModifiled())
    m_settings->notify(Settings::ProfileSettingsChanged);
}




/** [NetworkSettings/public]
 * Конструктор `NetworkSettings`
 */
NetworkSettings::NetworkSettings(Settings *settings, QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  m_settings = settings;
  
  m_welcomeCheckBox = new QCheckBox(tr("Всегда использовать этот сервер"), this);
  m_welcomeCheckBox->setChecked(m_settings->hideWelcome);
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
void NetworkSettings::reset()
{
  m_networkWidget->reset();
  m_welcomeCheckBox->setChecked(true);
}


/** [NetworkSettings/public]
 * Сохраняем настройки
 */
void NetworkSettings::save()
{
  if (m_networkWidget->save())
    m_settings->notify(Settings::NetworkSettingsChanged);
  
  m_settings->hideWelcome = m_welcomeCheckBox->isChecked();
}




/** [InterfaceSettings/public]
 * Конструктор `InterfaceSettings`
 */
InterfaceSettings::InterfaceSettings(Settings *settings, QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  m_settings = settings;
  
  m_styleComboBox = new QComboBox(this);
  m_styleComboBox->addItems(QStyleFactory::keys());  
  m_styleComboBox->setCurrentIndex(m_styleComboBox->findText(m_settings->style));
  
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
void InterfaceSettings::reset()
{
  m_styleComboBox->setCurrentIndex(m_styleComboBox->findText("Plastique"));
}


/** [InterfaceSettings/public]
 * Сохраняем настройки
 */
void InterfaceSettings::save()
{
  if (m_styleComboBox->currentIndex() != -1) {
    m_settings->style = m_styleComboBox->currentText();
    qApp->setStyle(m_settings->style);
  }
}




/** [UpdateSettings/public]
 * Конструктор `InterfaceSettings`
 */
#ifdef SCHAT_UPDATE
UpdateSettings::UpdateSettings(Settings *settings, QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  m_settings = settings;
  
  m_autoDownload = new QCheckBox(tr("Автоматически загружать обновления"), this);
  m_autoDownload->setChecked(m_settings->updateAutoDownload);
  m_autoDownload->setEnabled(false);
  
  m_autoClean = new QCheckBox(tr("Удалять обновления после установки"), this);
  m_autoClean->setChecked(m_settings->updateAutoClean);
  
  QLabel *interval = new QLabel(tr("Интервал проверки обновлений:"));
  QHBoxLayout *intervalLayout = new QHBoxLayout;
  
  m_interval = new QSpinBox(this);
  m_interval->setValue(m_settings->updateCheckInterval);
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
void UpdateSettings::reset()
{
  m_autoDownload->setChecked(true);
  m_autoClean->setChecked(true);
  m_interval->setValue(60);
}




/** [UpdateSettings/public]
 * Сохраняем настройки
 */
void UpdateSettings::save()
{
  m_settings->updateAutoDownload = m_autoDownload->isChecked();
  m_settings->updateAutoClean = m_autoClean->isChecked();
  m_settings->updateCheckInterval = m_interval->value();
  m_settings->notify(Settings::UpdateSettingsChanged);
}
#endif
