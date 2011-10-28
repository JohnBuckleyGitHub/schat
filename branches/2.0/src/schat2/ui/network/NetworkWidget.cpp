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

#include <QDebug>

#include <QComboBox>
#include <QEvent>
#include <QGridLayout>
#include <QKeyEvent>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>

#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "ui/network/NetworkWidget.h"

NetworkWidget::NetworkWidget(QWidget *parent)
  : QWidget(parent)
  , m_manager(ChatCore::i()->networks())
  , m_client(ChatCore::i()->client())
{
  m_combo = new QComboBox(this);

  m_menu = new QMenu(this);
  connect(m_menu, SIGNAL(aboutToShow()), SLOT(showMenu()));
  m_connectAction = m_menu->addAction("", this, SLOT(open()));
  m_menu->addSeparator();
  m_edit = m_menu->addAction(SCHAT_ICON(TopicEdit), tr("Edit"), this, SLOT(edit()));
  m_menu->addSeparator();
  m_addAction = m_menu->addAction(SCHAT_ICON(AddIcon), tr("Add"), this, SLOT(add()));
  m_removeAction = m_menu->addAction(SCHAT_ICON(RemoveIcon), tr("Remove"), this, SLOT(remove()));

  m_config = new QToolButton(this);
  m_config->setIcon(SCHAT_ICON(GearIcon));
  m_config->setMenu(m_menu);
  m_config->setPopupMode(QToolButton::InstantPopup);

  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(16, 16));
  m_toolBar->addWidget(m_config);
  m_toolBar->setStyleSheet(QLatin1String("QToolBar { margin:0px; border:0px; }"));

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(m_combo, 0, 0);
  mainLay->addWidget(m_toolBar, 0, 1);
  mainLay->setColumnStretch(0, 1);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  load();

  connect(m_combo, SIGNAL(currentIndexChanged(int)), SLOT(indexChanged(int)));
  connect(ChatCore::i(), SIGNAL(notify(int, const QVariant &)), SLOT(notify(int, const QVariant &)));
}


QAction *NetworkWidget::connectAction()
{
  QByteArray id = m_combo->itemData(m_combo->currentIndex()).toByteArray();
  int state = isCurrentActive();

  if (state == 1) {
    m_connectAction->setIcon(SCHAT_ICON(DisconnectIcon));
    m_connectAction->setText(tr("Disconnect"));
  }
  else if (state == 2) {
    m_connectAction->setIcon(SCHAT_ICON(DisconnectIcon));
    m_connectAction->setText(tr("Abort"));
  }
  else {
    m_connectAction->setIcon(SCHAT_ICON(ConnectIcon));
    m_connectAction->setText(tr("Connect"));
  }

  return m_connectAction;
}


/*!
 * Обработка действия для \p m_connectAction.
 */
void NetworkWidget::open()
{
  QMenu *popup = qobject_cast<QMenu *>(parentWidget());
  if (popup && isVisible())
    popup->close();

  int index = m_combo->currentIndex();
  if (index == -1)
    return;

  QByteArray id = m_combo->itemData(index).toByteArray();

  if (isCurrentActive()) {
    m_client->leave();
    return;
  }

  if (id.isEmpty()) {
    m_combo->setItemText(index, m_combo->currentText());
    m_manager->open(m_combo->currentText());
  }

  m_manager->open(id);
}


void NetworkWidget::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void NetworkWidget::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Return) {
    open();
  }
  else
    QWidget::keyPressEvent(event);
}


/*!
 * Добавление нового подключения.
 */
void NetworkWidget::add()
{
  m_combo->insertItem(0, "schat://");
  m_combo->setCurrentIndex(0);
  m_combo->setFocus();
}


/*!
 * Редактирование адреса сети.
 */
void NetworkWidget::edit()
{
  int index = m_combo->currentIndex();
  if (index == -1)
    return;

  QByteArray id = m_combo->itemData(index).toByteArray();
  if (SimpleID::typeOf(id) != SimpleID::ServerId)
    return;

  NetworkItem item = m_manager->item(id);
  if (!item.isValid())
    return;

  m_combo->setItemText(index, item.url());
  m_combo->setEditable(true);
  m_combo->setFocus();
}


/*!
 * Обработка изменения индекса, запрещается редактирование сохранённых сетей.
 */
void NetworkWidget::indexChanged(int index)
{
  QVariantMap map;
  QVariant data = m_combo->itemData(index);

  map["id"] = data.toByteArray();

  if (data.type() == QVariant::Invalid) {
    m_combo->setEditable(true);
    map["url"] = m_combo->itemText(index);
  }
  else
    m_combo->setEditable(false);

  m_manager->setSelected(map);
}


void NetworkWidget::notify(int notice, const QVariant &data)
{
  if (notice == ChatCore::NetworkChangedNotice) {
    QByteArray id = data.toByteArray();
    NetworkItem item = m_manager->item(id);

    if (!item.isValid())
      return;

    int index = m_combo->findText(item.url());
    if (index != -1)
      m_combo->removeItem(index);

    index = m_combo->findText("schat://");
    if (index != -1)
      m_combo->removeItem(index);

    index = m_combo->findData(id);
    if (index != -1)
      m_combo->removeItem(index);

    m_combo->insertItem(0, SCHAT_ICON(GlobeIcon), item.name(), item.id());
    m_combo->setCurrentIndex(0);
  }
  else if (notice == ChatCore::NetworkSelectedNotice) {
    qDebug() << "+++";
    if (data.type() != QVariant::Map)
      return;

    QByteArray id = data.toMap().value("id").toByteArray();
    if (id.isEmpty())
      return;

    int index = m_combo->findData(id);
    if (index != -1 && index != m_combo->currentIndex())
      m_combo->setCurrentIndex(index);
  }
}


/*!
 * Удаление сервера.
 */
void NetworkWidget::remove()
{
  int index = m_combo->currentIndex();
  if (index == -1)
    return;

  QByteArray id = m_combo->itemData(index).toByteArray();
  if (!id.isEmpty())
    m_manager->removeItem(id);

  m_combo->removeItem(index);
}


void NetworkWidget::showMenu()
{
  int index = m_combo->currentIndex();
  if (index == -1 || m_combo->itemData(index).type() != QVariant::ByteArray || m_combo->isEditable())
    m_edit->setVisible(false);
  else
    m_edit->setVisible(true);

  QByteArray id = m_combo->itemData(index).toByteArray();
  connectAction();
}


/*!
 * Возвращает состояние текущего выбранного итема.
 *
 * \return Возвращаемые значения:
 * - 0 Подключение не ассоциировано с выбранным итемом.
 * - 1 Подключение активно для текущего итема.
 * - 2 Идёт подключение.
 */
int NetworkWidget::isCurrentActive() const
{
  int index = m_combo->currentIndex();
  if (index == -1)
    return 0;

  QByteArray id = m_combo->itemData(index).toByteArray();

  if (m_client->clientState() == SimpleClient::ClientOnline && !id.isEmpty() && id == m_client->serverId()) {
    return 1;
  }

  if (m_client->clientState() == SimpleClient::ClientConnecting) {
    QString url;
    if (id.isEmpty())
      url = m_combo->currentText();
    else
      url = m_manager->item(id).url();

    if (m_client->url().toString() == url) {
      return 2;
    }
  }

  return 0;
}


/*!
 * Загрузка списка серверов в виджет.
 */
void NetworkWidget::load()
{
  QList<NetworkItem> items = m_manager->items();

  for (int i = 0; i < items.size(); ++i) {
    m_combo->addItem(SCHAT_ICON(GlobeIcon), items.at(i).name(), items.at(i).id());
  }

  if (m_combo->count() == 0) {
    m_combo->addItem("schat://schat.me");
    m_combo->setEditable(true);
  }
}


void NetworkWidget::retranslateUi()
{
  m_edit->setText(tr("Edit"));
  m_addAction->setText(tr("Add"));
  m_removeAction->setText(tr("Remove"));
}
