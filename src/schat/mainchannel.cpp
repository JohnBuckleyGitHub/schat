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

#include <QEvent>
#include <QSplitter>
#include <QVBoxLayout>
//#include <QDebug>

#include "mainchannel.h"
#include "settings.h"
#include "widget/networkwidget.h"
#include "widget/userview.h"
#include "widget/welcome.h"

/*!
 * \brief Конструктор класса MainChannel.
 */
MainChannel::MainChannel(const QIcon &icon, UserView *userView, QTabWidget *parent)
  : AbstractTab(Main, icon, parent),
    m_count(0),
    m_networkWidget(0),
    m_tabs(parent),
    m_userView(userView),
    m_welcome(0)
{
  m_view->channel("#main");
  m_view->log(SimpleSettings->getBool("Log"));

  m_splitter = new QSplitter(this);
  m_splitter->addWidget(m_view);
  m_splitter->addWidget(createUserView());
  m_splitter->setStretchFactor(0, 1);
  m_splitter->setStretchFactor(1, 1);

  #if defined(Q_OS_MAC)
  m_splitter->setHandleWidth(1);
  m_splitter->setStyleSheet("QSplitter::handle { background: #919191; }");
  #else
  m_splitter->setHandleWidth(m_splitter->handleWidth() + 2);
  #endif

  m_mainLayout = new QVBoxLayout(this);
  m_mainLayout->addWidget(m_splitter);
  m_mainLayout->setStretchFactor(m_splitter, 999);
  m_mainLayout->setMargin(0);
  m_mainLayout->setSpacing(2);

  QStringList splitterSizes = SimpleSettings->getList("SplitterSizes");
  if (splitterSizes.size() == 2) {
    int size1 = splitterSizes.at(0).toInt();
    if (size1 > 10)
      m_splitter->setSizes(QList<int>() << size1 << splitterSizes.at(1).toInt());
  }

  connect(SimpleSettings, SIGNAL(changed(int)), SLOT(notify(int)));
  connect(m_splitter, SIGNAL(splitterMoved(int, int)), SLOT(splitterMoved()));
  connect(m_userView, SIGNAL(usersCountChanged(int)), SLOT(usersCountChanged(int)));

  m_tabs->setCurrentIndex(m_tabs->addTab(this, ""));
  m_tabs->setTabIcon(m_tabs->indexOf(this), icon);

  retranslateUi();
}


/*!
 * Добавления события подключения нового пользователя.
 */
void MainChannel::addNewUser(quint8 gender, const QString &nick)
{
  if (SimpleSettings->getBool("ServiceMessages"))
    m_view->addServiceMsg(ChatView::statusNewUser(gender, nick));
}


void MainChannel::addUserLeft(quint8 gender, const QString &nick, const QString &bye)
{
  if (SimpleSettings->getBool("ServiceMessages"))
    m_view->addServiceMsg(ChatView::statusUserLeft(gender, nick, bye));
}


/*!
 * Включает/выключает показ виджета выбора сервера/сети.
 *
 * \param display \a true Показать виджет, \a Скрыть.
 */
void MainChannel::displayChoiceServer(bool display)
{
  if (display) {
    if (!m_networkWidget) {
      m_networkWidget = new NetworkWidget(this, NetworkWidget::NetworkLabel | NetworkWidget::ApplyButton | NetworkWidget::AddStretch);
      m_networkWidget->layout()->setContentsMargins(4, 2, 4, 0);
    }
    m_mainLayout->insertWidget(0, m_networkWidget);
    m_view->scroll();
    return;
  }

  if (m_networkWidget) {
    m_mainLayout->removeWidget(m_networkWidget);
    m_networkWidget->deleteLater();
    m_networkWidget = 0;
  }
  m_view->scroll();
}


void MainChannel::displayWelcome(bool display)
{
  if (display) {
    displayChoiceServer(false);
    if (!m_welcome) {
      m_welcome = new WelcomeWidget(this);
    }
    m_welcome->setFocus();
    m_mainLayout->insertWidget(0, m_welcome);
    m_mainLayout->removeWidget(m_splitter);
    m_splitter->setVisible(false);
    #if QT_VERSION >= 0x040500
    m_tabs->setTabsClosable(false);
    #endif
    return;
  }

  if (m_welcome) {
    m_mainLayout->removeWidget(m_welcome);
    m_welcome->deleteLater();
    m_welcome = 0;
    m_mainLayout->insertWidget(0, m_splitter);
    m_splitter->setVisible(true);
    #if QT_VERSION >= 0x040500
    m_tabs->setTabsClosable(true);
    #endif
  }

  retranslateUi();
}


void MainChannel::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  AbstractTab::changeEvent(event);
}


void MainChannel::notify(int code)
{
  if (code == Settings::MiscSettingsChanged) {
    m_view->log(SimpleSettings->getBool("Log"));
  }
  else if (code == Settings::ServerChanged) {
    displayWelcome(false);
  }

  if (m_welcome)
    m_welcome->notify(code);
}


/*!
 * Обработка перемещения разделителя.
 * Новое положение сохраняется в настройках.
 */
void MainChannel::splitterMoved()
{
  QList<int> sizes = m_splitter->sizes();
  if (sizes.size() == 2)
    SimpleSettings->setList("SplitterSizes", QStringList() << QString::number(sizes.at(0)) << QString::number(sizes.at(1)));
}


/*!
 * Обновление заголовка вкладки при изменении числа пользователей
 *
 * \param count Число пользователей, 0 - означает отсутствие соединения.
 */
void MainChannel::usersCountChanged(int count)
{
  m_count = count;
  retranslateUi();
}


/*!
 * Создание комбинированного виджета списка пользователей
 * и быстрого поиска.
 */
QWidget* MainChannel::createUserView()
{
  QWidget *userWidget = new QWidget(this);
  QuickUserSearch *userSearch = new QuickUserSearch(m_userView);
  m_userView->setQuickSearch(userSearch);
  QVBoxLayout *userLay = new QVBoxLayout(userWidget);
  userLay->setMargin(0);
  userLay->setSpacing(1);
  userLay->addWidget(m_userView);
  userLay->addWidget(userSearch);
  return userWidget;
}


void MainChannel::retranslateUi()
{
  int index = m_tabs->indexOf(this);

  if (m_welcome) {
    m_tabs->setTabText(index, tr("Welcome"));
    return;
  }

  if (m_count)
    m_tabs->setTabText(index, tr("Main (%1)").arg(m_count));
  else
    m_tabs->setTabText(index, tr("Main"));
}
