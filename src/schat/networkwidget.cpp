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
  
  m_networksPath = qApp->applicationDirPath() + "/networks/";
  
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
  connect(m_selectCombo, SIGNAL(editTextChanged(const QString &)), this, SLOT(editTextChanged(const QString &)));
  connect(m_settings, SIGNAL(networksModelIndexChanged(int)), this, SLOT(setCurrentIndex(int)));
  
  if (m_settings->needCreateNetworkList)
    createList();
  
  init();
}


/** [public]
 * Сохранение настроек
 */
bool NetworkWidget::save()
{
  QString currentText = m_selectCombo->currentText();
  
  // Если нынешний текущей текст отличается от предыдущего значит изменены настройки
  if (m_initItemText != currentText || m_initPort != m_portBox->value()) {
    
    // В случае если не существует итема с текущим текстом, создаём итем
    int index = m_selectCombo->findText(currentText);
    if (index == -1)
      m_selectCombo->addItem(QIcon(":/images/host.png"), currentText, m_portBox->value());
    
    index = m_selectCombo->findText(currentText);
    
    // Если данные итема содержат строку, значит, выбрана сеть, иначе одиночный сервер.
    // В данных хранится имя XML файла сети.
    QVariant data = m_selectCombo->itemData(index);
    if (data.type() == QVariant::String)
      m_settings->network.fromFile(data.toString());
    else
      m_settings->network.fromString(currentText + ':' + QString().setNum(m_portBox->value()));
    
    // Уведомление об изменении индекса
    m_settings->notify(Settings::NetworksModelIndexChanged, index);
    
    return true;
  }
  else
    return false;
}


/** [public]
 * Сброс настроек виджета.
 */
void NetworkWidget::reset()
{
  m_selectCombo->setCurrentIndex(m_selectCombo->findText("Achim Network"));
  m_portLabel->setEnabled(false);
  m_portBox->setEnabled(false);
  m_portBox->setValue(7666);
}


/** [private slots]
 * Слот вызывается при активации итема в `m_selectCombo`,
 * если тип данных активированного итема `QVariant::String`, то текущий итем является сетью,
 * поэтому отключаем возможность выбора порта.
 * Иначе разрешаем выбор порта и если тип данных итема равен `QVariant::Int`,
 * то устанавливаем значение `m_portBox` на основе данных итема.
 */
void NetworkWidget::activated(int index)
{
  QVariant data = m_selectCombo->itemData(index);
  if (data.type() == QVariant::String) {
    m_portBox->setEnabled(false);
    m_portBox->setValue(7666);
    m_portLabel->setEnabled(false);
  }
  else {
    if (data.type() == QVariant::Int)
      m_portBox->setValue(data.toInt());
    
    m_portBox->setEnabled(true);
    m_portLabel->setEnabled(true);
  }
}


/** [private slots]
 * Слот вызывается при смене текущего индекса в `m_selectCombo`,
 * если итем не содержит иконки, значит, происходит добавление новой записи.
 * Устанавливаем иконку итема и в данные записываем порт `m_portBox->value()`. * 
 */
void NetworkWidget::currentIndexChanged(int index)
{
  QIcon icon = m_selectCombo->itemIcon(index);
  if (icon.isNull()) {
    icon.addFile(":/images/host.png");
    m_selectCombo->setItemIcon(index, icon);
    m_selectCombo->setItemData(index, m_portBox->value());
  }
}


/** [private slots]
 * Слот вызывается при изменении текста в `m_selectCombo`.
 * Разрешаем выбор порта.
 */
void NetworkWidget::editTextChanged(const QString &text)
{
  m_portLabel->setEnabled(true);
  m_portBox->setEnabled(true);
}


/** [private slots]
 * 
 */
void NetworkWidget::setCurrentIndex(int index)
{
  m_selectCombo->setCurrentIndex(index);
  m_initItemText = m_selectCombo->currentText();
  
  if (m_settings->network.isNetwork()) {
    m_portLabel->setEnabled(false);
    m_portBox->setEnabled(false);
    m_portBox->setValue(7666);
  }
  else {
    m_portLabel->setEnabled(true);
    m_portBox->setEnabled(true);
    m_portBox->setValue(m_settings->network.port());
  }
  
  m_initPort = m_portBox->value();
}


/** [private]
 * 
 */
void NetworkWidget::createList()
{ 
  QDir directory(m_networksPath);
  directory.setNameFilters(QStringList() << "*.xml");
  QStringList files = directory.entryList(QDir::Files | QDir::NoSymLinks);
  NetworkReader network;
  
  foreach (QString file, files) {
    if (network.readFile(m_networksPath + file))
      m_selectCombo->addItem(QIcon(":/images/network.png"), network.networkName(), file);
  }
  m_settings->needCreateNetworkList = false;
}


/** [private]
 * 
 */
void NetworkWidget::init()
{
  // Устанавливаем индекс на основе текущего выбора
  if (m_settings->network.isNetwork()) {
    m_selectCombo->setCurrentIndex(m_selectCombo->findText(m_settings->network.name()));
    m_portLabel->setEnabled(false);
    m_portBox->setEnabled(false);
    m_portBox->setValue(7666);
  }
  else {
    QString server = m_settings->network.server();
    int i = m_selectCombo->findText(server);
    
    if (i == -1)
      m_selectCombo->addItem(QIcon(":/images/host.png"), server, m_settings->network.port());
    
    m_selectCombo->setCurrentIndex(m_selectCombo->findText(server));
    m_portBox->setValue(m_settings->network.port());
    m_portLabel->setEnabled(true);
    m_portBox->setEnabled(true);
  }
  
  m_initItemText = m_selectCombo->currentText();
  m_initPort     = m_portBox->value();
}
