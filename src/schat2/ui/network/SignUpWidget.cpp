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

#include <QComboBox>
#include <QFormLayout>
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
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/network/SecurityQuestion.h"
#include "ui/network/SignUpWidget.h"
#include "ui/network/NetworkButton.h"

SignUpWidget::SignUpWidget(QWidget *parent, const QString &action)
  : NetworkExtra(parent)
  , m_action(action)
{
  m_nameLabel = new QLabel(this);
  m_nameEdit = new QLineEdit(this);

  m_passwordLabel = new QLabel(this);
  m_passwordEdit = new QLineEdit(this);
  m_passwordEdit->setEchoMode(QLineEdit::Password);

  m_questionLabel = new QLabel(this);
  m_question = new SecurityQuestion(this);

  m_answerLabel = new QLabel(this);
  m_answerEdit = new QLineEdit(this);

  m_signUp = new NetworkButton(tr("Sign up"), this);
  m_signUp->setReady(false);

  QHBoxLayout *nameLay = new QHBoxLayout;
  nameLay->addWidget(m_nameLabel);
  nameLay->addWidget(m_nameEdit);
  nameLay->addWidget(m_passwordLabel);
  nameLay->addWidget(m_passwordEdit);
  nameLay->setMargin(0);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addLayout(nameLay, 0, 0, 1, 3);
  mainLay->addWidget(m_questionLabel, 1, 0);
  mainLay->addWidget(m_question, 1, 1, 1, 2);
  mainLay->addWidget(m_answerLabel, 2, 0);
  mainLay->addWidget(m_answerEdit, 2, 1);
  mainLay->addWidget(m_signUp, 2, 2);
  mainLay->setMargin(0);

  connect(m_nameEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(m_passwordEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(m_answerEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(m_question, SIGNAL(currentIndexChanged(int)), SLOT(reload()));

  connect(m_signUp->button(), SIGNAL(clicked(bool)), SLOT(signUp()));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));

  retranslateUi();
}


QString SignUpWidget::title() const
{
  if (m_action == LS("reset"))
    return tr("Reset your password");

  return tr("Sign up");
}


void SignUpWidget::focusInEvent(QFocusEvent *event)
{
  QWidget::focusInEvent(event);

  m_nameEdit->setFocus();
}


void SignUpWidget::retranslateUi()
{
  m_nameLabel->setText(tr("Name:"));
  m_questionLabel->setText(tr("Security question:"));
  m_answerLabel->setText(tr("Answer:"));

  if (m_action == LS("reset")) {
    m_signUp->setText(tr("Reset"));
    m_passwordLabel->setText(tr("New password:"));
  }
  else {
    m_signUp->setText(tr("Sign up"));
    m_passwordLabel->setText(tr("Password:"));
  }
}


void SignUpWidget::reload()
{
  m_signUp->setReady(isReady());

  makeRed(m_nameEdit, false);
  makeRed(m_answerEdit, false);
}


void SignUpWidget::notify(const Notify &notify)
{
  if (notify.type() == Notify::QueryError) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (!n.match(ChatClient::id(), LS("account"), m_action))
      return;

    if (n.status() == Notice::ObjectAlreadyExists) {
      m_signUp->setError(tr("User is already registered"));
      makeRed(m_nameEdit);
    }
    else if (n.status() == Notice::NotFound) {
      m_signUp->setError(tr("User does not exist"));
      makeRed(m_nameEdit);
    }
    else if (n.status() == Notice::Unauthorized) {
      m_signUp->setError(tr("Security question or answer is incorrect"));
      makeRed(m_answerEdit);
    }
    else
      m_signUp->setError(Notice::status(n.status()));
  }
  else if (notify.type() == Notify::FeedReply) {
    if (!static_cast<const FeedNotify &>(notify).match(ChatClient::id(), LS("account"), m_action))
      return;

    m_signUp->setReady(false);
    emit done();
  }
}


void SignUpWidget::signUp()
{
  m_signUp->setProgress();

  QVariantMap data;
  data[LS("name")] = m_nameEdit->text();
  data[LS("pass")] = SimpleID::encode(SimpleID::password(m_passwordEdit->text()));
  data[LS("q")]    = SimpleID::encode(SimpleID::make(m_question->currentText().toUtf8(), SimpleID::MessageId));
  data[LS("a")]    = SimpleID::encode(SimpleID::make(m_answerEdit->text().toUtf8(), SimpleID::MessageId));

  if (m_action == LS("reset")) {
    ChatClient::setAccount(m_nameEdit->text());
    ChatClient::setPassword(m_passwordEdit->text());
  }

  ChatClient::feeds()->query(LS("account"), m_action, data);
}


/*!
 * Возвращает \b true если виджет готов к регистрации.
 */
bool SignUpWidget::isReady() const
{
  if (m_nameEdit->text().isEmpty())
    return false;

  if (m_passwordEdit->text().isEmpty())
    return false;

  if (m_answerEdit->text().isEmpty())
    return false;

  if (m_question->currentIndex() < 1)
    return false;

  if (ChatClient::state() != ChatClient::Online)
    return false;

  if (ChatClient::serverId() != ChatCore::networks()->selected())
    return false;

  if (!ChatClient::channel()->account()->name().isEmpty())
    return false;

  return true;
}
