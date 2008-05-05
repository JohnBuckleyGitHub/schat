/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "networkwidget.h"
#include "networkreader.h"
#include "settings.h"

NetworkWidget::NetworkWidget(Settings *settings, QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  m_settings = settings;
  
  m_selectCombo = new QComboBox(this);
  m_selectCombo->setEditable(true);
  m_selectCombo->setDuplicatesEnabled(false);
  m_selectCombo->setIconSize(QSize(18, 18));
  m_selectCombo->setModel(&m_settings->networksModel);
  
  m_infoLabel = new QLabel(tr("&Сервер/Сеть:"), this);
  m_infoLabel->setBuddy(m_selectCombo);
  
  m_portBox = new QSpinBox(this);
  m_portBox->setRange(1024, 65536);
  m_portBox->setValue(7666);
  m_portBox->setToolTip(tr("<div style='white-space:nowrap;'>Порт сервера, по умолчанию <b>7666</b><br />"
      "<i>Доступно только при подключении к одиночному серверу</i>.</div>"));
  m_portBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  m_portLabel = new QLabel(tr("&Порт:"), this);
  m_portLabel->setBuddy(m_portBox);
  
  m_networkPath = qApp->applicationDirPath() + "/networks/";
  
  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->addWidget(m_infoLabel);
  mainLayout->addWidget(m_selectCombo, 1);
  mainLayout->addSpacing(6);
  mainLayout->addWidget(m_portLabel);
  mainLayout->addWidget(m_portBox);
  mainLayout->setSpacing(2);
  mainLayout->setMargin(0);
  
  connect(m_selectCombo, SIGNAL(activated(int)), this, SLOT(activated(int)));
  connect(m_selectCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
  
  init();
}


/** [private slots]
 * 
 */
void NetworkWidget::activated(int index)
{
}


/** [private slots]
 * 
 */
void NetworkWidget::currentIndexChanged(int index)
{
  QVariant data = m_selectCombo->itemData(index);
  if (data.type() == QVariant::String) {
    m_portBox->setEnabled(false);
    m_portLabel->setEnabled(false);
  }
  else {
    m_portBox->setEnabled(true);
    m_portLabel->setEnabled(true);
  }
  
  QIcon icon = m_selectCombo->itemIcon(index);
  if (icon.isNull()) {
    icon.addFile(":/images/host.png");
    m_selectCombo->setItemIcon(index, icon);
  }
}


/** [private]
 * 
 */
void NetworkWidget::init()
{
  if (m_settings->needInitNetworkList) {
    QDir directory(m_networkPath);
    directory.setNameFilters(QStringList() << "*.xml");
    QStringList files = directory.entryList(QDir::Files | QDir::NoSymLinks);
    NetworkReader network;
    
    foreach (QString file, files) {
      if (network.readFile(m_networkPath + file))
        m_selectCombo->addItem(QIcon(":/images/network.png"), network.networkName(), file);
    }
    m_settings->needInitNetworkList = false;
  }
  
  // Устанавливаем индекс на основе текущего выбора
  if (m_settings->network.isNetwork()) {
    m_selectCombo->setCurrentIndex(m_selectCombo->findText(m_settings->network.name()));
    m_portLabel->setEnabled(false);
    m_portBox->setEnabled(false);
  }
  else {
    QString server = m_settings->network.server();
    int i = m_selectCombo->findText(server);
    
    if (i == -1)
      m_selectCombo->addItem(QIcon(":/images/host.png"), server);
    
    m_selectCombo->setCurrentIndex(m_selectCombo->findText(server));
    m_portBox->setValue(m_settings->network.port());
    m_portLabel->setEnabled(true);
    m_portBox->setEnabled(true);
  }
    
}
