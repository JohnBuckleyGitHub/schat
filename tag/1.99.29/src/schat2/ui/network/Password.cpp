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

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QToolButton>

#include "ChatNotify.h"
#include "client/ChatClient.h"
#include "client/ClientFeeds.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/network/NetworkButton.h"
#include "ui/network/Password.h"
#include "ui/network/SecurityQuestion.h"

Password::Password(QWidget *parent)
  : NetworkExtra(parent)
{
  m_password = new QRadioButton(this);
  m_password->setChecked(true);
  QFont font = m_password->font();
  font.setBold(true);
  m_password->setFont(font);

  m_question = new QRadioButton(this);
  m_question->setFont(font);

  m_passwordWidget = new PasswordWidget(this);
  m_questionWidget = new QuestionWidget(this);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_password);
  mainLay->addWidget(m_passwordWidget);
  mainLay->addWidget(m_question);
  mainLay->addWidget(m_questionWidget);
  mainLay->setMargin(0);

  setMode(PasswordMode);

  connect(m_password, SIGNAL(toggled(bool)), SLOT(toggled()));
  connect(m_passwordWidget, SIGNAL(done()), SIGNAL(done()));
  connect(m_questionWidget, SIGNAL(done()), SIGNAL(done()));

  retranslateUi();
}


void Password::retranslateUi()
{
  m_password->setText(tr("Change password"));
  m_question->setText(tr("Change security question"));

  m_passwordWidget->retranslateUi();
  m_questionWidget->retranslateUi();
}


void Password::toggled()
{
  if (m_password->isChecked())
    setMode(PasswordMode);
  else
    setMode(QuestionMode);
}


void Password::setMode(Mode mode)
{
  m_passwordWidget->setVisible(mode == PasswordMode);
  m_questionWidget->setVisible(mode == QuestionMode);
  m_mode = mode;
}


PasswordBase::PasswordBase(QWidget *parent)
  : QWidget(parent)
{
  m_passwordLabel = new QLabel(this);
  m_passwordEdit = new QLineEdit(this);
  m_passwordEdit->setEchoMode(QLineEdit::Password);

  m_ok = new NetworkButton(tr("OK"), this);

  connect(m_passwordEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));

  retranslateUi();
}


void PasswordBase::retranslateUi()
{
  m_passwordLabel->setText(tr("Current password:"));
  m_ok->setText(tr("OK"));
}


void PasswordBase::reload()
{
  m_ok->setReady(isReady());
  NetworkExtra::makeRed(m_passwordEdit, false);
}


void PasswordBase::notify(const Notify &notify)
{
  if (!isVisible())
    return;

  if (notify.type() == Notify::QueryError) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (!n.match(ChatClient::id(), LS("account"), LS("password")))
      return;

    if (n.status() == Notice::Forbidden) {
      m_ok->setError(tr("Password is incorrect"));
      m_passwordEdit->setFocus();
      NetworkExtra::makeRed(m_passwordEdit);
    }
    else
      m_ok->setError(Notice::status(n.status()));
  }
  else if (notify.type() == Notify::FeedReply) {
    if (!static_cast<const FeedNotify &>(notify).match(ChatClient::id(), LS("account"), LS("password")))
      return;

    m_ok->setReady(false);
    emit done();
  }
}


bool PasswordBase::isReady() const
{
  if (ChatClient::state() != ChatClient::Online)
    return false;

  if (m_passwordEdit->text().isEmpty())
    return false;

  return true;
}


PasswordWidget::PasswordWidget(QWidget *parent)
  : PasswordBase(parent)
{
  m_newLabel = new QLabel(this);
  m_newEdit = new QLineEdit(this);
  m_newEdit->setEchoMode(QLineEdit::Password);

  setTabOrder(m_passwordEdit, m_newEdit);
  setTabOrder(m_newEdit, m_ok);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(m_passwordLabel, 0, 0);
  mainLay->addWidget(m_passwordEdit, 0, 1, 1, 2);
  mainLay->addWidget(m_newLabel, 1, 0);
  mainLay->addWidget(m_newEdit, 1, 1);
  mainLay->addWidget(m_ok, 1, 2);
  mainLay->setContentsMargins(20, 0, 0, 0);

  connect(m_newEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));

  connect(m_passwordEdit, SIGNAL(returnPressed()), SLOT(execute()));
  connect(m_newEdit, SIGNAL(returnPressed()), SLOT(execute()));
  connect(m_ok->button(), SIGNAL(clicked()), SLOT(execute()));

  reload();
}


void PasswordWidget::retranslateUi()
{
  PasswordBase::retranslateUi();
  m_newLabel->setText(tr("New password:"));
}


bool PasswordWidget::isReady() const
{
  if (!PasswordBase::isReady())
    return false;

  if (m_newEdit->text().isEmpty())
    return false;

  return true;
}


void PasswordWidget::execute()
{
  if (!isReady())
    return;

  m_ok->setProgress();

  QVariantMap data;
  data[LS("pass")]   = SimpleID::encode(SimpleID::password(m_passwordEdit->text()));
  data[LS("new")]    = SimpleID::encode(SimpleID::password(m_newEdit->text()));

  ChatClient::feeds()->query(LS("account"), LS("password"), data);
}


QuestionWidget::QuestionWidget(QWidget *parent)
  : PasswordBase(parent)
{
  m_questionLabel = new QLabel(this);
  m_question = new SecurityQuestion(this);

  m_answerLabel = new QLabel(this);
  m_answerEdit = new QLineEdit(this);

  setTabOrder(m_passwordEdit, m_question);
  setTabOrder(m_question, m_answerEdit);
  setTabOrder(m_answerEdit, m_ok);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(m_passwordLabel, 0, 0);
  mainLay->addWidget(m_passwordEdit, 0, 1, 1, 2);
  mainLay->addWidget(m_questionLabel, 1, 0);
  mainLay->addWidget(m_question, 1, 1, 1, 2);
  mainLay->addWidget(m_answerLabel, 2, 0);
  mainLay->addWidget(m_answerEdit, 2, 1);
  mainLay->addWidget(m_ok, 2, 2);
  mainLay->setContentsMargins(20, 0, 0, 0);

  connect(m_answerEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(m_question, SIGNAL(currentIndexChanged(int)), SLOT(reload()));

  connect(m_passwordEdit, SIGNAL(returnPressed()), SLOT(execute()));
  connect(m_answerEdit, SIGNAL(returnPressed()), SLOT(execute()));
  connect(m_ok->button(), SIGNAL(clicked()), SLOT(execute()));

  reload();
}


void QuestionWidget::retranslateUi()
{
  PasswordBase::retranslateUi();
  m_questionLabel->setText(tr("New question:"));
  m_answerLabel->setText(tr("New answer:"));
}


bool QuestionWidget::isReady() const
{
  if (!PasswordBase::isReady())
    return false;

  if (m_answerEdit->text().isEmpty())
    return false;

  if (m_question->currentIndex() < 1)
    return false;

  return true;
}


void QuestionWidget::execute()
{
  if (!isReady())
    return;

  m_ok->setProgress();

  QVariantMap data;
  data[LS("pass")]   = SimpleID::encode(SimpleID::password(m_passwordEdit->text()));
  data[LS("q")]      = SimpleID::encode(SimpleID::make(m_question->currentText().toUtf8(), SimpleID::MessageId));
  data[LS("a")]      = SimpleID::encode(SimpleID::make(m_answerEdit->text().toUtf8(), SimpleID::MessageId));

  ChatClient::feeds()->query(LS("account"), LS("password"), data);
}
