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

#include <QDebug>

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
#include "QProgressIndicator/QProgressIndicator.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/network/LoginWidget.h"

LoginWidget::LoginWidget(QWidget *parent)
  : QWidget(parent)
  , m_manager(ChatCore::networks())
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

  m_login = new QToolButton(this);
  m_login->setIcon(SCHAT_ICON(OK));
  m_login->setAutoRaise(true);
  m_login->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

  m_error = new QToolButton(this);
  m_error->setIcon(SCHAT_ICON(ExclamationRed));
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
  mainLay->setMargin(0);

  connect(m_nameEdit, SIGNAL(textChanged(const QString &)), SLOT(textChanged()));
  connect(m_nameEdit, SIGNAL(editingFinished()), SLOT(editingFinished()));
  connect(m_nameEdit, SIGNAL(returnPressed()), SLOT(login()));
  connect(m_passwordEdit, SIGNAL(textChanged(const QString &)), SLOT(textChanged()));
  connect(m_passwordEdit, SIGNAL(editingFinished()), SLOT(editingFinished()));
  connect(m_passwordEdit, SIGNAL(returnPressed()), SLOT(login()));

  connect(m_login, SIGNAL(clicked()), SLOT(login()));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));

  textChanged();
  retranslateUi();
}


/*!
 * Возвращает \p true если в текущий момент времени возможна авторизация.
 */
bool LoginWidget::canLogIn() const
{
  if (ChatClient::state() != ChatClient::Online)
    return false;

  if (ChatClient::serverId() != ChatCore::networks()->selected())
    return false;

  if (!ChatClient::channel()->account()->name().isEmpty())
    return false;

  return true;
}


void LoginWidget::retranslateUi()
{
  m_nameLabel->setText(tr("&Name:"));
  m_passwordLabel->setText(tr("&Password:"));
  m_login->setText(tr("Sign in"));
  m_login->setToolTip(m_login->text());
}


/*!
 * Обновление состояния виджета.
 */
void LoginWidget::reload()
{
  textChanged();
}


void LoginWidget::focusInEvent(QFocusEvent *event)
{
  QWidget::focusInEvent(event);

  m_nameEdit->setFocus();
}


void LoginWidget::showEvent(QShowEvent *event)
{
  m_login->setMaximumHeight(m_passwordEdit->height());
  m_error->setMaximumHeight(m_passwordEdit->height());

  QWidget::showEvent(event);
}


void LoginWidget::editingFinished()
{
}


void LoginWidget::login()
{
  if (!m_login->isVisible())
    return;

  m_progress->startAnimation();
  m_progress->setVisible(true);
  m_login->setVisible(false);

  ChatClient::feeds()->request(ChatClient::id(), LS("query"), LS("account"), RegCmds::request(LS("login"), m_nameEdit->text(), m_passwordEdit->text()));
}


void LoginWidget::notify(const Notify &notify)
{
  if (notify.type() == Notify::QueryError) {
    QVariantMap data = notify.data().toMap();
    if (data.value(LS("name")) != LS("account"))
      return;

    if (data.value(LS("id")) != ChatClient::id())
      return;

    m_progress->setVisible(false);
    m_error->setVisible(true);

    int status = data.value(LS("status")).toInt();
    if (status == Notice::NotFound) {
      m_error->setToolTip(tr("User does not exist"));
      makeRed(m_nameEdit);
      m_nameEdit->setFocus();
    }
    else if (status == Notice::Forbidden) {
      m_error->setToolTip(tr("Password is incorrect"));
      makeRed(m_passwordEdit);
      m_passwordEdit->setFocus();
    }
    else
      m_error->setToolTip(Notice::status(status));
  }
}


void LoginWidget::textChanged()
{
  if (m_nameEdit->text().isEmpty() || m_passwordEdit->text().isEmpty() || !canLogIn())
    m_login->setVisible(false);
  else
    m_login->setVisible(true);

  m_error->setVisible(false);

  makeRed(m_nameEdit, false);
  makeRed(m_passwordEdit, false);
}


void LoginWidget::makeRed(QWidget *widget, bool red)
{
  QPalette palette = widget->palette();

  if (red)
    palette.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));
  else
    palette.setColor(QPalette::Active, QPalette::Base, Qt::white);

  widget->setPalette(palette);
}
