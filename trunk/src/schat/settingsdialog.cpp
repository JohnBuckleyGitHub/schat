/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "settingsdialog.h"
#include "schatwindow.h"


/** [SettingsDialog/public]
 * Конструктор SettingsDialog
 */
SettingsDialog::SettingsDialog(QWidget *parent)
  : QDialog(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);
  
  resize(500, 256);
  
  chat           = static_cast<SChatWindow *>(parent);
  okButton       = new QPushButton(QIcon(":/images/ok.png"), tr("OK"), this);
  cancelButton   = new QPushButton(QIcon(":/images/cancel.png"), tr("Отмена"), this);
  resetButton    = new QPushButton(QIcon(":/images/undo.png"), "", this);  
  contentsWidget = new QListWidget(this);
  pagesWidget    = new QStackedWidget;

  profilePage = new ProfileSettings(this);
  networkPage = new NetworkSettings(this);
  
  resetButton->setToolTip(tr("Вернуть настройки по умолчанию"));
  pagesWidget->addWidget(profilePage);
  pagesWidget->addWidget(networkPage);
  
  QFrame *line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  
  new QListWidgetItem(QIcon(":/images/profile.png"), tr("Личные данные"), contentsWidget);
  new QListWidgetItem(QIcon(":/images/network.png"), tr("Сеть"), contentsWidget);
  
  connect(contentsWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(resetButton, SIGNAL(clicked()), this, SLOT(reset()));
  
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
  }  
}



/** [ProfileSettings/public]
 * Конструктор `ProfileSettings`
 */
ProfileSettings::ProfileSettings(QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  QLabel *label = new QLabel(tr("Cтраница `ProfileSettings` пока не реализована."));
  
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(label);
  mainLayout->addStretch();
}


/** [ProfileSettings/public]
 * 
 */
void ProfileSettings::reset()
{
  QMessageBox::information(this, "d", "void ProfileSettings::reset()");
}


/** [ProfileSettings/public]
 * Сохраняем настройки
 */
void ProfileSettings::save()
{
}


/** [NetworkSettings/public]
 * Конструктор `NetSettingsPage`
 */
NetworkSettings::NetworkSettings(QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  QLabel *label = new QLabel(tr("Cтраница `NetworkSettings` пока не реализована."));
  
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(label);
  mainLayout->addStretch();
}


/** [NetworkSettings/public]
 * 
 */
void NetworkSettings::reset()
{
  QMessageBox::information(this, "d", "void NetworkSettings::reset()");
}


/** [NetworkSettings/public]
 * Сохраняем настройки
 */
void NetworkSettings::save()
{
}
