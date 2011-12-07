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
#include <QToolButton>

#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "messages/MessageAdapter.h"
#include "NetworkManager.h"
#include "QProgressIndicator/QProgressIndicator.h"
#include "ui/network/LoginWidget.h"
#include "ui/UserUtils.h"
#include "net/packets/Notice.h"

LoginWidget::LoginWidget(QWidget *parent)
  : QWidget(parent)
  , m_manager(ChatCore::i()->networks())
{
  m_nameLabel = new QLabel(this);
  m_nameEdit = new QLineEdit(this);

  m_passwordLabel = new QLabel(this);
  m_passwordEdit = new QLineEdit(this);
  m_passwordEdit->setEchoMode(QLineEdit::Password);

  m_login = new QToolButton(this);
  m_login->setIcon(SCHAT_ICON(KeyIcon));
  m_login->setAutoRaise(true);

  m_error = new QToolButton(this);
  m_error->setIcon(SCHAT_ICON(ExclamationRedIcon));
  m_error->setAutoRaise(true);
  m_error->setVisible(false);

  m_progress = new QProgressIndicator(this);
  m_progress->setAnimationDelay(100);
  m_progress->setMaximumSize(16, 16);
  m_progress->setVisible(false);

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(m_nameLabel);
  mainLay->addWidget(m_nameEdit);
  mainLay->addWidget(m_passwordLabel);
  mainLay->addWidget(m_passwordEdit);
  mainLay->addWidget(m_login);
  mainLay->addWidget(m_error);
  mainLay->addWidget(m_progress);
  mainLay->setMargin(4);

  connect(m_nameEdit, SIGNAL(textChanged(const QString &)), SLOT(textChanged()));
  connect(m_nameEdit, SIGNAL(editingFinished()), SLOT(editingFinished()));
  connect(m_passwordEdit, SIGNAL(textChanged(const QString &)), SLOT(textChanged()));
  connect(m_passwordEdit, SIGNAL(editingFinished()), SLOT(editingFinished()));

  connect(m_login, SIGNAL(clicked()), SLOT(login()));
  connect(ChatCore::i()->client(), SIGNAL(notice(const Notice &)), SLOT(notice(const Notice &)));

  textChanged();
  retranslateUi();
}


/*!
 * Возвращает \p true если в текущий момент времени возможна авторизация.
 */
bool LoginWidget::canLogIn() const
{
  QVariant selected = ChatCore::i()->networks()->selected();
  SimpleClient *client = ChatCore::i()->client();

  if (selected.type() != QVariant::ByteArray)
    return false;

  QByteArray id = selected.toByteArray();
  if (client->serverId() != id)
    return false;

  if (client->clientState() != SimpleClient::ClientOnline)
    return false;

//  if (!client->user()->account().isEmpty())
//    return false;

  return true;
}


void LoginWidget::retranslateUi()
{
  m_nameLabel->setText(tr("Name:"));
  m_passwordLabel->setText(tr("Password:"));
  m_login->setToolTip(tr("Log In"));
}


/*!
 * Обновление состояния виджета.
 */
void LoginWidget::reload()
{
  NetworkItem item = m_manager->item(m_manager->selected());
  m_nameEdit->setText(item.account());
  m_passwordEdit->setText(item.password());

  textChanged();
}


void LoginWidget::showEvent(QShowEvent *event)
{
  m_login->setMaximumHeight(m_passwordEdit->height());
  m_error->setMaximumHeight(m_passwordEdit->height());

  QWidget::showEvent(event);
}


void LoginWidget::editingFinished()
{
  NetworkItem& item = m_manager->edit(m_manager->selected());
  item.setAccount(m_nameEdit->text());
  item.setPassword(m_passwordEdit->text());
}


void LoginWidget::login()
{
  m_progress->startAnimation();
  m_progress->setVisible(true);
  m_login->setVisible(false);

//  ChatCore::i()->adapter()->login("login", m_nameEdit->text(), m_passwordEdit->text());
}


void LoginWidget::notice(const Notice &notice)
{
  if (notice.command() != "login.reply")
    return;

  m_progress->setVisible(false);

  if (notice.status() == Notice::OK) {
    reload();
    return;
  }

  m_error->setVisible(true);
  m_error->setToolTip(Notice::status(notice.status()));

  if (notice.status() == Notice::UserNotExists)
    ChatCore::makeRed(m_nameEdit);
  else if (notice.status() == Notice::Forbidden)
    ChatCore::makeRed(m_passwordEdit);
}


void LoginWidget::textChanged()
{
  if (m_nameEdit->text().isEmpty() || m_passwordEdit->text().isEmpty() || !canLogIn())
    m_login->setVisible(false);
  else
    m_login->setVisible(true);

  m_error->setVisible(false);

  ChatCore::makeRed(m_nameEdit, false);
  ChatCore::makeRed(m_passwordEdit, false);
}
