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

#include <QComboBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
//#include <QDebug>

#include "network.h"
#include "networkwidget.h"
#include "settings.h"

/*!
 * Конструктор класса NetworkWidget.
 */
NetworkWidget::NetworkWidget(QWidget *parent, Options options)
  : TranslateWidget(parent),
  m_networkLabel(0),
  m_applyButton(0),
  m_settings(SimpleSettings)
{
  setAttribute(Qt::WA_DeleteOnClose);

  m_select = new QComboBox(this);
  m_select->setEditable(true);
  #ifndef SCHAT_WINCE_VGA
  m_select->setIconSize(QSize(18, 18));
  #endif
  m_select->setModel(&m_settings->networksModel);

  QHBoxLayout *mainLay = new QHBoxLayout;
  mainLay->setSpacing(2);
  mainLay->setMargin(0);

  if (options & NetworkLabel) {
    m_networkLabel = new QLabel(this);
    m_networkLabel->setBuddy(m_select);
    mainLay->addWidget(m_networkLabel);
  }

  mainLay->addWidget(m_select);
  mainLay->setStretchFactor(m_select, 1);

  if (options & ApplyButton) {
    m_applyButton = new QToolButton(this);
    m_applyButton->setIcon(QIcon(":/images/arrow-right.png"));
    m_applyButton->setAutoRaise(true);
    mainLay->addWidget(m_applyButton);

    connect(m_applyButton, SIGNAL(clicked(bool)), SLOT(link()));
  }

  if (options & AddStretch) {
    QHBoxLayout *stretchLay = new QHBoxLayout(this);
    stretchLay->addLayout(mainLay);
    stretchLay->addStretch();
  }
  else
    setLayout(mainLay);

  connect(m_select, SIGNAL(currentIndexChanged(int)), SLOT(currentIndexChanged(int)));
  connect(m_select, SIGNAL(editTextChanged(const QString &)), SLOT(editTextChanged(const QString &)));
  connect(m_settings, SIGNAL(networksModelIndexChanged(int)), SLOT(setCurrentIndex(int)));

  init();
  retranslateUi();
}


/*!
 * Формирует структуру с информацией о сервере на основе строки.
 */
ServerInfo NetworkWidget::singleServer(const QString &url)
{
  ServerInfo out;
  out.port = 7666;
  int lastIndex = url.lastIndexOf(':');
  if (lastIndex == -1) {
    out.address = url;
    return out;
  }

  out.address = url.left(lastIndex);
  out.port = QString(url.mid(lastIndex + 1)).toUInt();
  if (!out.port)
    out.port = 7666;

  return out;
}


/*!
 * Сброс настроек виджета.
 */
void NetworkWidget::reset()
{
  int index = m_select->findText("Simple Network");

  if (index != -1)
    m_select->setCurrentIndex(index);
  else
    addSingleServer(Network::failBack());
}


/*!
 * Сохранение настроек введённых в виджете.
 *
 * Если нынешний текущей текст отличается от предыдущего значит, изменены настройки.
 * В случае необходимости будет добавлен одиночный сервер.
 */
int NetworkWidget::save(bool notify)
{
  QString currentText = m_select->currentText();

  if (m_initText != currentText) {
    int index = m_select->findText(currentText);
    if (index == -1)
      index = addSingleServer(singleServer(currentText), false);

    index = m_select->findText(currentText);

    // Если данные итема содержат строку, значит, выбрана сеть, иначе одиночный сервер.
    // В данных хранится имя XML файла сети.
    QVariant data = m_select->itemData(index);
    if (data.type() == QVariant::String)
      m_settings->network.fromFile(data.toString());
    else {
      ServerInfo info = singleServer(currentText);
      m_settings->network.fromString(info.address + ':' + QString::number(info.port));
    }

    m_settings->notify(Settings::NetworksModelIndexChanged, index);
    if (notify)
      m_settings->notify(Settings::NetworkSettingsChanged);

    return 1;
  }
  else
    return 0;
}


/*!
 * Слот вызывается при смене текущего индекса
 * если итем не содержит иконки, значит, происходит добавление новой записи.
 */
void NetworkWidget::currentIndexChanged(int index)
{
  QIcon icon = m_select->itemIcon(index);
  if (icon.isNull()) {
    icon.addFile(":/images/computer.png");
    m_select->setItemIcon(index, icon);
  }
}


/*!
 * Слот вызывается при изменении текста.
 */
void NetworkWidget::editTextChanged(const QString &text)
{
  QPalette palette = m_select->palette();
  bool ok = !text.isEmpty();

  if (ok)
    palette.setColor(QPalette::Active, QPalette::Base, Qt::white);
  else
    palette.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));

  m_select->setPalette(palette);
  emit validServer(ok);
  if (m_applyButton)
    m_applyButton->setEnabled(ok);
}


void NetworkWidget::link()
{
  save(false);
  SimpleSettings->notify(Settings::ServerChanged);
}


/*!
 * Установка текущего индекса.
 */
void NetworkWidget::setCurrentIndex(int index)
{
  m_select->setCurrentIndex(index);
  m_initText = m_select->currentText();
}


/*!
 * Добавление в случае необходимости нового одиночного сервера.
 */
int NetworkWidget::addSingleServer(const QString &address, quint16 port, bool current)
{
  int index = findSingleServer(address, port);
  if (index == -1) {
    if (port == 7666)
      m_select->addItem(QIcon(":/images/computer.png"), address);
    else
      m_select->addItem(QIcon(":/images/computer.png"), address + ":" + QString::number(port));

    index = findSingleServer(address, port);
  }

  if (current && index != -1)
    m_select->setCurrentIndex(index);

  return index;
}


/*!
 * Поиск одиночного сервера.
 * В случае стандартного порта, сначала ищется укороченная запись, затем
 * сочетание address:port.
 *
 * \return -1 в случае не удачного поиска.
 */
int NetworkWidget::findSingleServer(const QString &address, quint16 port) const
{
  int index = -1;
  if (port == 7666) {
    index = m_select->findText(address);
    if (index != -1)
      return index;
  }

  index = m_select->findText(address + ":" + QString::number(port));
  return index;
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
  if (m_settings->network.isNetwork())
    m_select->setCurrentIndex(m_select->findText(m_settings->network.name()));
  else
    addSingleServer(m_settings->network.server());

  m_initText = m_select->currentText();
}


void NetworkWidget::retranslateUi()
{
  if (m_networkLabel)
    m_networkLabel->setText(tr("&Network:"));

  if (m_applyButton)
    m_applyButton->setToolTip(tr("Connect"));
}
