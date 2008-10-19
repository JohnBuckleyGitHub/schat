/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui>

#include "network.h"
#include "networkwidget.h"
#include "settings.h"

/*!
 * \brief Конструктор класса NetworkWidget.
 */
NetworkWidget::NetworkWidget(QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  m_settings = settings;

  m_select = new QComboBox(this);
  m_select->setEditable(true);
  m_select->setIconSize(QSize(18, 18));
  m_select->setModel(&m_settings->networksModel);

  m_infoLabel = new QLabel(tr("&Сервер/Сеть:"), this);
  m_infoLabel->setBuddy(m_select);

  m_port = new QSpinBox(this);
  m_port->setRange(1024, 65536);
  m_port->setValue(7666);
  m_port->setToolTip(tr("Порт сервера, по умолчанию 7666\nДоступно только при подключении к одиночному серверу"));
  m_port->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  m_portLabel = new QLabel(tr("&Порт:"), this);
  m_portLabel->setBuddy(m_port);

  m_networksPath = qApp->applicationDirPath() + "/networks/";

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(m_infoLabel);
  mainLay->addWidget(m_select, 1);
  mainLay->addSpacing(6);
  mainLay->addWidget(m_portLabel);
  mainLay->addWidget(m_port);
  mainLay->setSpacing(2);
  mainLay->setMargin(0);

  connect(m_select, SIGNAL(activated(int)), SLOT(activated(int)));
  connect(m_select, SIGNAL(currentIndexChanged(int)), SLOT(currentIndexChanged(int)));
  connect(m_select, SIGNAL(editTextChanged(const QString &)), SLOT(editTextChanged(const QString &)));
  connect(m_settings, SIGNAL(networksModelIndexChanged(int)), SLOT(setCurrentIndex(int)));

  init();
}


/*!
 * \brief Сохранение настроек введённых в виджете.
 */
bool NetworkWidget::save()
{
  QString currentText = m_select->currentText();

  // Если нынешний текущей текст отличается от предыдущего значит изменены настройки
  if (m_initText != currentText || m_initPort != m_port->value()) {

    // В случае если не существует итема с текущим текстом, создаём итем
    int index = m_select->findText(currentText);
    if (index == -1)
      m_select->addItem(QIcon(":/images/host.png"), currentText, m_port->value());

    index = m_select->findText(currentText);

    // Если данные итема содержат строку, значит, выбрана сеть, иначе одиночный сервер.
    // В данных хранится имя XML файла сети.
    QVariant data = m_select->itemData(index);
    if (data.type() == QVariant::String)
      m_settings->network.fromFile(data.toString());
    else {
      m_settings->network.fromString(currentText + ':' + QString().setNum(m_port->value()));
      m_select->setItemData(index, m_port->value());
    }

    // Уведомление об изменении индекса
    m_settings->notify(Settings::NetworksModelIndexChanged, index);

    return true;
  }
  else
    return false;
}


/*!
 * Сброс настроек виджета.
 */
void NetworkWidget::reset()
{
  int index = m_select->findText("Simple Network");

  if (index != -1) {
    m_select->setCurrentIndex(index);
    m_portLabel->setEnabled(false);
    m_port->setEnabled(false);
    m_port->setValue(7666);
  }
  else
    addServer(Network::failBack());
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
  QVariant data = m_select->itemData(index);
  if (data.type() == QVariant::String) {
    m_port->setEnabled(false);
    m_port->setValue(7666);
    m_portLabel->setEnabled(false);
  }
  else {
    if (data.type() == QVariant::Int)
      m_port->setValue(data.toInt());

    m_port->setEnabled(true);
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
  QIcon icon = m_select->itemIcon(index);
  if (icon.isNull()) {
    icon.addFile(":/images/host.png");
    m_select->setItemIcon(index, icon);
    m_select->setItemData(index, m_port->value());
  }
}


/*!
 * Слот вызывается при изменении текста в `m_selectCombo`.
 * Разрешаем выбор порта.
 */
void NetworkWidget::editTextChanged(const QString &text)
{
  QPalette palette = m_select->palette();
  bool ok = !text.isEmpty();

  if (ok)
    palette.setColor(QPalette::Active, QPalette::Base, Qt::white);
  else
    palette.setColor(QPalette::Active, QPalette::Base, QColor("#f66"));

  m_select->setPalette(palette);
  m_portLabel->setEnabled(ok);
  m_port->setEnabled(ok);

  emit validServer(ok);
}


/** [private slots]
 *
 */
void NetworkWidget::setCurrentIndex(int index)
{
  m_select->setCurrentIndex(index);
  m_initText = m_select->currentText();

  if (m_settings->network.isNetwork()) {
    m_portLabel->setEnabled(false);
    m_port->setEnabled(false);
    m_port->setValue(7666);
  }
  else {
    m_portLabel->setEnabled(true);
    m_port->setEnabled(true);
    m_port->setValue(m_settings->network.port());
  }

  m_initPort = m_port->value();
}


void NetworkWidget::addServer(const ServerInfo &info)
{
  if (m_select->findText(info.address) == -1)
    m_select->addItem(QIcon(":/images/host.png"), info.address, info.port);

  m_select->setCurrentIndex(m_select->findText(info.address));
  m_port->setValue(info.port);
  m_portLabel->setEnabled(true);
  m_port->setEnabled(true);
}


/*!
 * \brief Инициализация виджета (установка на индекса на основе текущего выбора).
 *
 * Если текущая сеть является настоящей сетью (не одиночный сервер), то индексом устанавливается
 * название сети, также отключается возможность выбирать порт.
 *
 * В случае одиночного сервера, при необходимости добавляется необходимый пункт
 * и индекс устанавливается на основе адреса сервера, также устанавливается порт и разрешается
 * возможность его изменения.
 *
 * Устанавливаются значения \a m_initText и \a m_initPort на основе текущего выбора.
 */
void NetworkWidget::init()
{
  if (m_settings->network.isNetwork()) {
    m_select->setCurrentIndex(m_select->findText(m_settings->network.name()));
    m_portLabel->setEnabled(false);
    m_port->setEnabled(false);
    m_port->setValue(7666);
  }
  else
    addServer(m_settings->network.server());

  m_initText = m_select->currentText();
  m_initPort = m_port->value();
}
