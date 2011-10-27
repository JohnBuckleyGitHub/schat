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

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

#include "ChatCore.h"
#include "ui/network/LoginWidget.h"
#include "NetworkManager.h"

LoginWidget::LoginWidget(QWidget *parent)
  : QWidget(parent)
{
  m_nameLabel = new QLabel(this);
  m_nameEdit = new QLineEdit(this);

  m_passwordLabel = new QLabel(this);
  m_passwordEdit = new QLineEdit(this);
  m_passwordEdit->setEchoMode(QLineEdit::Password);

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(m_nameLabel);
  mainLay->addWidget(m_nameEdit);
  mainLay->addWidget(m_passwordLabel);
  mainLay->addWidget(m_passwordEdit);
  mainLay->setMargin(4);

  retranslateUi();
}


void LoginWidget::retranslateUi()
{
  m_nameLabel->setText(tr("Name"));
  m_passwordLabel->setText(tr("Password"));
}


void LoginWidget::update(const QByteArray &id)
{
  NetworkItem item = ChatCore::i()->networks()->item(id);
  qDebug() << item.name();
  m_nameEdit->setText(item.account());
  m_passwordEdit->setText(QString());
}
