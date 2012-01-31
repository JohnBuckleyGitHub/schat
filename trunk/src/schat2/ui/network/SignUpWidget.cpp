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
#include <QPushButton>
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
#include "QProgressIndicator/QProgressIndicator.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/network/SecurityQuestion.h"
#include "ui/network/SignUpWidget.h"

SignUpWidget::SignUpWidget(QWidget *parent, const QString &action)
  : QWidget(parent)
  , m_manager(ChatCore::networks())
  , m_action(action)
  , m_state(Idle)
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

  m_error = new QToolButton(this);
  m_error->setIcon(SCHAT_ICON(ExclamationRed));
  m_error->setAutoRaise(true);
  m_error->setVisible(false);

  m_progress = new QProgressIndicator(this);
  m_progress->setAnimationDelay(100);
  m_progress->setMaximumSize(16, 16);
  m_progress->setVisible(false);

  m_signUp = new QPushButton(SCHAT_ICON(OK), tr("Sign up"), this);
  m_signUp->setEnabled(false);

  QHBoxLayout *nameLay = new QHBoxLayout;
  nameLay->addWidget(m_nameLabel);
  nameLay->addWidget(m_nameEdit);
  nameLay->addWidget(m_passwordLabel);
  nameLay->addWidget(m_passwordEdit);
  nameLay->setMargin(0);

  QHBoxLayout *buttonLay = new QHBoxLayout;
  buttonLay->addWidget(m_error);
  buttonLay->addWidget(m_progress);
  buttonLay->addWidget(m_signUp);
  buttonLay->setMargin(0);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addLayout(nameLay, 0, 0, 1, 3);
  mainLay->addWidget(m_questionLabel, 1, 0);
  mainLay->addWidget(m_question, 1, 1, 1, 2);
  mainLay->addWidget(m_answerLabel, 2, 0);
  mainLay->addWidget(m_answerEdit, 2, 1);
  mainLay->addLayout(buttonLay, 2, 2);
  mainLay->setMargin(0);

  connect(m_nameEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(m_passwordEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(m_answerEdit, SIGNAL(textChanged(const QString &)), SLOT(reload()));
  connect(m_question, SIGNAL(currentIndexChanged(int)), SLOT(reload()));

  connect(m_signUp, SIGNAL(clicked(bool)), SLOT(signUp()));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));

  retranslateUi();
}


SignUpWidget::~SignUpWidget()
{
  qDebug() << "~SignUpWidget()";
}


/*!
 * Возвращает \b true если виджет готов к регистрации.
 */
bool SignUpWidget::ready() const
{
  if (m_nameEdit->text().isEmpty())
    return false;

  if (m_passwordEdit->text().isEmpty())
    return false;

  if (m_answerEdit->text().isEmpty())
    return false;

  if (m_question->currentIndex() < 1)
    return false;

  return true;
}


/*!
 * Проверка возможности регистрации.
 *
 * \return true если регистрация возможна.
 */
bool SignUpWidget::canSignUp()
{
  if (ChatClient::state() != ChatClient::Online)
    return false;

  if (ChatClient::serverId() != ChatCore::networks()->selected())
    return false;

  if (!ChatClient::channel()->account()->name().isEmpty())
    return false;

  return true;
}


void SignUpWidget::retranslateUi()
{
  m_nameLabel->setText(tr("Name:"));

  if (m_action == LS("reset")) {
    m_passwordLabel->setText(tr("New password:"));
    m_signUp->setText(tr("Reset"));
  }
  else {
    m_passwordLabel->setText(tr("Password:"));
    m_signUp->setText(tr("Sign up"));
  }

  m_questionLabel->setText(tr("Security question:"));
  m_answerLabel->setText(tr("Answer:"));
}


void SignUpWidget::focusInEvent(QFocusEvent *event)
{
  QWidget::focusInEvent(event);

  m_nameEdit->setFocus();
}


void SignUpWidget::reload()
{
  setState(Idle);
}


void SignUpWidget::notify(const Notify &notify)
{
  if (notify.type() == Notify::QueryError) {
    QVariantMap data = notify.data().toMap();
    if (data.value(LS("name")) != LS("account"))
      return;

    if (data.value(LS("id")) != ChatClient::id())
      return;

    setState(Idle);

    int status = data.value(LS("status")).toInt();
    if (status == Notice::ObjectAlreadyExists) {
      m_error->setToolTip(tr("User is already registered"));
      makeRed(m_nameEdit);
    }
    else if (status == Notice::NotFound) {
      m_error->setToolTip(tr("User does not exist"));
      makeRed(m_nameEdit);
    }
    else if (status == Notice::Unauthorized) {
      m_error->setToolTip(tr("Security question or answer is incorrect"));
      makeRed(m_answerEdit);
    }
    else
      m_error->setToolTip(Notice::status(status));

    setState(Error);
  }
  else if (notify.type() == Notify::FeedReply) {
    QVariantMap data = notify.data().toMap();
    if (data.value(LS("name")) != LS("account"))
      return;

    if (data.value(LS("id")) != ChatClient::id())
      return;

    if (data.value(LS("data")).toMap().value(LS("action")) != m_action)
      return;

    m_progress->setVisible(false);
    m_progress->stopAnimation();
    emit done();
  }
}


void SignUpWidget::signUp()
{
  setState(Progress);

  QVariantMap data = RegCmds::request(m_action, m_nameEdit->text(), m_passwordEdit->text());

  data[LS("q")] = SimpleID::encode(SimpleID::make(m_question->currentText().toUtf8(), SimpleID::MessageId));
  data[LS("a")] = SimpleID::encode(SimpleID::make(m_answerEdit->text().toUtf8(), SimpleID::MessageId));

  ChatClient::feeds()->request(ChatClient::id(), LS("query"), LS("account"), data);
}


void SignUpWidget::makeRed(QWidget *widget, bool red)
{
  QPalette palette = widget->palette();

  if (red)
    palette.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));
  else
    palette.setColor(QPalette::Active, QPalette::Base, Qt::white);

  widget->setPalette(palette);
}


void SignUpWidget::setState(WidgetState state)
{
  m_state = state;

  if (m_state == Idle) {
    m_nameEdit->setEnabled(true);
    makeRed(m_nameEdit, false);
    makeRed(m_answerEdit, false);
    m_passwordEdit->setEnabled(true);
    m_question->setEnabled(true);
    m_answerEdit->setEnabled(true);
    m_signUp->setEnabled(ready());

    m_progress->setVisible(false);
    m_progress->stopAnimation();
    m_error->setVisible(false);
  }
  else if (m_state == Progress) {
    m_nameEdit->setEnabled(false);
    m_passwordEdit->setEnabled(false);
    m_question->setEnabled(false);
    m_answerEdit->setEnabled(false);
    m_signUp->setEnabled(false);

    m_progress->setVisible(true);
    m_progress->startAnimation();
    m_error->setVisible(false);
  }
  else if (m_state == Error) {
    m_signUp->setEnabled(false);
    m_error->setVisible(true);
  }
}
