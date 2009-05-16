/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include <QtGui>

#include "mainchannel.h"
#include "settings.h"
#include "widget/networkwidget.h"

/*!
 * \brief Конструктор класса MainChannel.
 */
MainChannel::MainChannel(const QIcon &icon, QWidget *parent)
  : AbstractTab(Main, icon, parent)
{
  m_view->channel("#main");
  m_view->log(SimpleSettings->getBool("Log"));

  createActions();

  m_connectCreateButton->setVisible(false);

  m_networkWidget = new NetworkWidget(this);
  m_networkWidget->setVisible(false);

  m_networkLayout = new QHBoxLayout;
  m_networkLayout->addWidget(m_networkWidget);
  m_networkLayout->addWidget(m_connectCreateButton);
  m_networkLayout->addStretch();
  m_networkLayout->setMargin(0);

  m_mainLayout = new QVBoxLayout;
  m_mainLayout->addLayout(m_networkLayout);
  m_mainLayout->addWidget(m_view);
  m_mainLayout->setMargin(0);
  m_mainLayout->setSpacing(2);
  setLayout(m_mainLayout);

  connect(m_networkWidget, SIGNAL(validServer(bool)), m_connectCreateButton, SLOT(setEnabled(bool)));
  connect(SimpleSettings, SIGNAL(changed(int)), SLOT(notify(int)));
}


/*!
 * Добавления события подключения нового пользователя.
 * Пользователь добавляется в очередь, для объединения этих событий в одно.
 */
void MainChannel::addNewUser(quint8 gender, const QString &nick)
{
  m_view->addServiceMsg(ChatView::statusNewUser(gender, nick));
//  if (!m_usersJoin.isActive())
//    m_usersJoin.start(1000, this);
//
//  m_newUsers.insert(nick, gender);
}


void MainChannel::addUserLeft(quint8 gender, const QString &nick, const QString &bye)
{
  m_view->addServiceMsg(ChatView::statusUserLeft(gender, nick, bye));
}


/*!
 * Включает/выключает показ виджета выбора сервера/сети.
 *
 * \param display \a true Показать виджет, \a Скрыть.
 */
void MainChannel::displayChoiceServer(bool display)
{
  if (display)
    m_networkLayout->setContentsMargins(4, 2, 4, 0);

    m_networkWidget->setVisible(display);
    m_connectCreateButton->setVisible(display);

  if (!display)
    m_networkLayout->setMargin(0);

  m_view->scroll();
}


//void MainChannel::timerEvent(QTimerEvent *event)
//{
//  if (event->timerId() == m_usersJoin.timerId()) {
//    m_usersJoin.stop();
//    if (!m_newUsers.isEmpty()) {
//      if (m_newUsers.size() == 1)
//        m_view->addServiceMsg(ChatView::statusNewUser(m_newUsers.values().at(0), m_newUsers.keys().at(0)));
//      else
//        addNewUsers(m_newUsers.keys());
//
//      m_newUsers.clear();
//    }
//  } else
//    AbstractTab::timerEvent(event);
//}


void MainChannel::notify(int code)
{
  if (code == Settings::MiscSettingsChanged)
    m_view->log(SimpleSettings->getBool("Log"));
}


void MainChannel::serverChanged()
{
  m_networkWidget->save();
  SimpleSettings->notify(Settings::ServerChanged);
}


void MainChannel::createActions()
{
  m_connectCreateButton = new QToolButton(this);
  m_connectCreateAction = new QAction(QIcon(":/images/network_connect.png"), tr("Подключится"), this);
  m_connectCreateButton->setDefaultAction(m_connectCreateAction);
  m_connectCreateButton->setAutoRaise(true);
  connect(m_connectCreateAction, SIGNAL(triggered()), this, SLOT(serverChanged()));
}


//void MainChannel::addNewUsers(const QStringList &nicks)
//{
//  QString out = "<span class='newUser'>";
//
//  if (nicks.size() > 10) {
//    out += tr("<b>%n</b> пользователь заходят в чат", "", nicks.size());
//  }
//  else {
//    out += QString("<a href='nick:%1'>%2</a>").arg(QString(nicks.at(0).toUtf8().toHex())).arg(Qt::escape(nicks.at(0)));
//    for (int i = 1; i < nicks.size(); ++i) {
//      out += QString(", <a href='nick:%1'>%2</a>").arg(QString(nicks.at(i).toUtf8().toHex())).arg(Qt::escape(nicks.at(i)));
//    }
//    out += tr(" заходят в чат");
//  }
//
//  out += "</span>";
//  m_view->addServiceMsg(out);
//}
