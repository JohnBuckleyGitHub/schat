/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#include "Account.h"
#include "ChatCore.h"
#include "ChatNotify.h"
#include "client/ChatClient.h"
#include "client/ClientFeeds.h"
#include "client/SimpleClient.h"
#include "hooks/RegCmds.h"
#include "net/packets/Notice.h"
#include "NetworkManager.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/network/LoginWidget.h"
#include "ui/network/NetworkButton.h"

LoginWidget::LoginWidget(QWidget *parent)
  : NetworkExtra(parent)
{
  m_nameEdit = new QLineEdit(this);
  m_nameEdit->setMaxLength(255);
  m_nameLabel = new QLabel(this);
  m_nameLabel->setBuddy(m_nameEdit);

  m_passwordEdit = new QLineEdit(this);
  m_passwordEdit->setEchoMode(QLineEdit::Password);
  m_passwordEdit->setMaxLength(255);
  m_passwordLabel = new QLabel(this);
  m_passwordLabel->setBuddy(m_passwordEdit);

  m_login = new NetworkButton(tr("Sign in"), this);

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(m_nameLabel);
  mainLay->addWidget(m_nameEdit);
  mainLay->addWidget(m_passwordLabel);
  mainLay->addWidget(m_passwordEdit);
  mainLay->addWidget(m_login);
  mainLay->setMargin(0);

  connect(m_nameEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(m_nameEdit, SIGNAL(returnPressed()), SLOT(login()));
  connect(m_passwordEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(m_passwordEdit, SIGNAL(returnPressed()), SLOT(login()));

  connect(m_login->button(), SIGNAL(clicked()), SLOT(login()));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));

  reload();
  retranslateUi();
}


QString LoginWidget::title() const
{
  return tr("Sign in");
}


void LoginWidget::retranslateUi()
{
  m_nameLabel->setText(tr("&Name:"));
  m_passwordLabel->setText(tr("&Password:"));
  m_login->setText(tr("Sign in"));
}


void LoginWidget::focusInEvent(QFocusEvent *event)
{
  QWidget::focusInEvent(event);

  m_nameEdit->setFocus();
}


void LoginWidget::login()
{
  if (!isReady())
    return;

  m_login->setProgress();
  ChatClient::i()->login(m_nameEdit->text(), m_passwordEdit->text());
}


void LoginWidget::notify(const Notify &notify)
{
  if (notify.type() == Notify::QueryError) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (!n.match(ChatClient::id(), LS("account"), LS("login")))
      return;

    if (n.status() == Notice::NotFound) {
      m_login->setError(tr("User does not exist"));
      makeRed(m_nameEdit);
      m_nameEdit->setFocus();
    }
    else if (n.status() == Notice::Forbidden) {
      m_login->setError(tr("Password is incorrect"));
      makeRed(m_passwordEdit);
      m_passwordEdit->setFocus();
    }
    else
      m_login->setError(Notice::status(n.status()));
  }
}


/*!
 * Обновление состояния виджета.
 */
void LoginWidget::reload()
{
  m_login->setReady(isReady());

  makeRed(m_nameEdit, false);
  makeRed(m_passwordEdit, false);
}


/*!
 * Возвращает \p true если в текущий момент времени возможна авторизация.
 */
bool LoginWidget::isReady() const
{
  if (m_nameEdit->text().isEmpty())
    return false;

  if (m_passwordEdit->text().isEmpty())
    return false;

  if (ChatClient::state() != ChatClient::Online)
    return false;

  if (ChatClient::serverId() != ChatCore::networks()->selected())
    return false;

  if (!ChatClient::channel()->account()->name().isEmpty())
    return false;

  return true;
}
