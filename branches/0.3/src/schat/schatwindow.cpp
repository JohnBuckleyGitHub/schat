/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui>
#include <QtNetwork>

#include "aboutdialog.h"
#include "abstractprofile.h"
#include "mainchannel.h"
#include "protocol.h"
#include "schatwindow.h"
#include "settings.h"
#include "settingsdialog.h"
#include "tab.h"
#include "trayicon.h"
#include "version.h"
#include "welcomedialog.h"
#include "widget/sendwidget.h"
#include "widget/userview.h"

QMap<QString, QString> SChatWindow::m_cmds;

/*!
 * \brief Конструктор класса SChatWindow.
 */
SChatWindow::SChatWindow(QWidget *parent)
  : QMainWindow(parent)
{
  m_profile     = new AbstractProfile(this);
  m_settings    = new Settings(qApp->applicationDirPath() + "/schat.conf", m_profile, this);
  m_settings->read();

  m_send        = new SendWidget(m_settings, this);
  m_central     = new QWidget(this);
  m_splitter    = new QSplitter(m_central);
  m_tabs        = new QTabWidget(m_splitter);
  m_tabs->installEventFilter(this);
  m_right       = new QWidget(m_splitter);
  m_users       = new UserView(m_profile, m_right);
  m_rightLay    = new QVBoxLayout(m_right);
  m_mainLay     = new QVBoxLayout(m_central);
  m_toolsLay    = new QHBoxLayout;
  m_statusBar   = new QStatusBar(this);
  m_statusLabel = new QLabel(this);

  m_splitter->addWidget(m_tabs);
  m_splitter->addWidget(m_right);
  m_splitter->setStretchFactor(0, 4);
  m_splitter->setStretchFactor(1, 1);

  m_rightLay->addLayout(m_toolsLay);
  m_rightLay->addWidget(m_users);
  m_rightLay->setMargin(0);
  m_rightLay->setSpacing(4);

  m_mainLay->addWidget(m_splitter);
  m_mainLay->addWidget(m_send);
  m_mainLay->setMargin(4);
  m_mainLay->setSpacing(1);
  m_mainLay->setStretchFactor(m_splitter, 999);
  m_mainLay->setStretchFactor(m_send, 1);

  setCentralWidget(m_central);
  setStatusBar(m_statusBar);
  m_statusBar->addWidget(m_statusLabel, 1);
  m_statusLabel->setText(tr("Не подключено"));

  setWindowTitle(tr("IMPOMEZIA Simple Chat"));

  m_tabs->setElideMode(Qt::ElideRight);

  restoreGeometry();

  #ifdef SCHAT_UPDATE
  m_updateTimer = new QTimer(this);
  m_updateTimer->setInterval(m_settings->getInt("Updates/CheckInterval") * 60 * 1000);
  connect(m_updateTimer, SIGNAL(timeout()), SLOT(update()));
  #endif

  createActions();
  createCornerWidgets();
  createToolButtons();
  createTrayIcon();
  createService();

  connect(m_send, SIGNAL(sendMsg(const QString &)), SLOT(sendMsg(const QString &)));
  connect(m_send, SIGNAL(needCopy()), SLOT(copy()));
  connect(m_users, SIGNAL(addTab(const QString &)), SLOT(addTab(const QString &)));
  connect(m_users, SIGNAL(insertNick(const QString &)), m_send, SLOT(insertHtml(const QString &)));
  connect(m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
  connect(m_tabs, SIGNAL(currentChanged(int)), SLOT(stopNotice(int)));
  connect(m_tray, SIGNAL(messageClicked()), SLOT(messageClicked()));
  connect(m_settings, SIGNAL(changed(int)), SLOT(settingsChanged(int)));

  m_main = new MainChannel(QIcon(":/images/main.png"), m_settings, this);
  connect(m_main, SIGNAL(nickClicked(const QString &)), m_users, SLOT(nickClicked(const QString &)));

  m_tabs->setCurrentIndex(m_tabs->addTab(m_main, tr("Общий")));
  m_tabs->setTabIcon(0, m_main->icon());

  if (!m_settings->getBool("HideWelcome") || m_settings->getBool("FirstRun")) {
    m_welcome = new WelcomeDialog(m_settings, m_profile, this);
    connect(m_welcome, SIGNAL(accepted()), this, SLOT(welcomeOk()));
    if (!m_welcome->exec())
      m_main->displayChoiceServer(true);
  }
  else
    m_clientService->connectToHost();

  qsrand(QDateTime(QDateTime::currentDateTime()).toTime_t());

  // Пытаемся запустить сервер, в случае неудачи удаляем сервер.
//  daemon = new Server(this);
//  if (daemon->start()) {
//    daemon->setLocalProfile(profile);
//    connect(daemon, SIGNAL(incomingDirectConnection(const QString &, ServerSocket *)), this, SLOT(incomingDirectConnection(const QString &, ServerSocket *)));
//  }
//  else
//    delete daemon;

  #ifdef SCHAT_UPDATE
  QTimer::singleShot(0, this, SLOT(update()));
  #endif
}


/*!
 * \brief Обработка события закрытия чата.
 */
void SChatWindow::closeEvent(QCloseEvent *event)
{
  saveGeometry();
  m_settings->write();
  hideChat();

  QMainWindow::closeEvent(event);
}


/*!
 * Обработка событий.
 */
bool SChatWindow::event(QEvent *event)
{
  if (event->type() == QEvent::WindowActivate)
    stopNotice(m_tabs->currentIndex());

  return QMainWindow::event(event);
}


#ifndef DISABLE_SINGLE_APP
void SChatWindow::handleMessage(const QString &message)
{
  QStringList list = message.split(", ");

  if (list.contains("-exit")) {
    closeChat();
    return;
  }

  showChat();
}
#endif


/** [private slots]
 *
 */
void SChatWindow::about()
{
  if (isHidden())
    show();

  if (!m_about) {
    m_about = new AboutDialog(this);
    m_about->show();
  }

  m_about->activateWindow();
}


/** [private slots]
 *
 */
void SChatWindow::accessDenied(quint16 reason)
{
  bool notify = true;

  switch (reason) {
    case ErrorNickAlreadyUse:
      uniqueNick();
      m_clientService->connectToHost();
      break;

    case ErrorOldClientProtocol:
      m_main->msg(ChatBrowser::msgOldClientProtocol());
      break;

    case ErrorOldServerProtocol:
      m_main->msg(ChatBrowser::msgOldServerProtocol());
      break;

    case ErrorBadNickName:
      m_main->msg(ChatBrowser::msgBadNickName(m_profile->nick()));
      break;

    case ErrorUsersLimitExceeded:
    case ErrorLinksLimitExceeded:
    case ErrorMaxUsersPerIpExceeded:
      notify = false;
      break;

    default:
      m_main->msg(ChatBrowser::msgAccessDenied(reason));
      break;
  }

  if (notify)
    unconnected(false);
}


/** [private slots]
 * Слот вызывается кода `m_clientService` получает пакет с опкодом `OpcodeAccessGranted`,
 * что означает успешное подключение к серверу/сети.
 */
void SChatWindow::accessGranted(const QString &network, const QString &server, quint16 /*level*/)
{
  if (network.isEmpty()) {
    m_main->msg(ChatBrowser::msgReadyForUse(server));
    m_statusLabel->setText(tr("Успешно подключены к серверу %1").arg(server));
    setWindowTitle(tr("IMPOMEZIA Simple Chat"));
  }
  else {
    m_main->msg(ChatBrowser::msgReadyForUse(network, server));
    m_statusLabel->setText(tr("Успешно подключены к сети %1 (%2)").arg(network).arg(server));
    setWindowTitle(tr("IMPOMEZIA Simple Chat - %1").arg(network));
  }
}


void SChatWindow::addTab(const QString &nick)
{
  int index = tabIndex(nick);

  if (index == -1) {
    AbstractProfile profile(m_users->profile(nick));
    Tab *tab = new Tab(QIcon(":/images/" + profile.gender() + ".png"), m_settings, this);
    tab->setChannel(nick);
    index = m_tabs->addTab(tab, tab->icon(), nick);
    m_tabs->setTabToolTip(index, UserView::userToolTip(profile));
    connect(tab, SIGNAL(nickClicked(const QString &)), m_users, SLOT(nickClicked(const QString &)));
  }

  m_tabs->setCurrentIndex(index);
}


/** [private slots]
 *
 */
void SChatWindow::closeChat()
{
  m_clientService->quit();

  saveGeometry();
  m_settings->write();
  qApp->quit();
}


/** [private slots]
 *
 */
void SChatWindow::closeTab(int tab)
{
  int index = tab;
  if (index == -1)
    index = m_tabs->currentIndex();

  if (index) {
    QWidget *widget = m_tabs->widget(index);
    m_tabs->removeTab(index);
    QTimer::singleShot(0, widget, SLOT(deleteLater()));
  }
  else
    m_clientService->quit();
}


/** [private slots]
 * Слот вызывается когда m_clientService пытается подключится к серверу.
 * ПАРАМЕТРЫ:
 *   server  -> название сети если network = true, либо адрес сервера.
 *   network -> подключение к сети (true) либо к серверу (false).
 */
void SChatWindow::connecting(const QString &server, bool network)
{
  if (network)
    m_statusLabel->setText(tr("Идёт подключение к сети %1...").arg(server));
  else
    m_statusLabel->setText(tr("Идёт подключение к серверу %1...").arg(server));

  m_main->displayChoiceServer(false);
}


void SChatWindow::copy()
{
  AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->currentWidget());
  QTextCursor cursor = tab->textCursor();

  if (cursor.hasSelection())
    tab->copy();
  else
    m_send->copy();
}


/** [private slots]
 *
 */
void SChatWindow::fatal()
{
  m_main->displayChoiceServer(true);
}


/** [private slots]
 *
 */
void SChatWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::MiddleClick:
      if (isHidden())
        showChat();
      else
        hideChat();

    default:
      break;
  }
}


void SChatWindow::linkLeave(quint8 /*numeric*/, const QString &network, const QString &name)
{
  m_main->msg(ChatBrowser::msgLinkLeave(network, name));
}


/** [private slots]
 *
 */
void SChatWindow::message(const QString &sender, const QString &message)
{
  startNotice(0);

  m_main->msgNewMessage(sender, message);
}


/** [private slots]
 *
 */
void SChatWindow::messageClicked()
{
  QProcess::startDetached('"' + qApp->applicationDirPath() + "/schat.exe\"");

  closeChat();
}


void SChatWindow::newLink(quint8 /*numeric*/, const QString &network, const QString &name)
{
  m_main->msg(ChatBrowser::msgNewLink(network, name));
}


/** [private slots]
 *
 */
void SChatWindow::newNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name)
{
  if (m_users->isUser(nick)) {
    m_users->rename(nick, newNick);

    int oldIndex = tabIndex(nick);
    int newIndex = tabIndex(newNick);

    if (oldIndex != -1) {
      AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->widget(oldIndex));

      if (newIndex == -1) {
        m_tabs->setTabText(oldIndex, newNick);
        tab->setChannel(newNick);
      }
      else {
        AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->widget(newIndex));
        tab->msg(ChatBrowser::msgChangedNick(gender, nick, newNick));
        if (m_tabs->currentIndex() == oldIndex)
          m_tabs->setCurrentIndex(newIndex);
      }

      tab->msg(ChatBrowser::msgChangedNick(gender, nick, newNick));
    }

    newProfile(gender, newNick, name);
    m_main->msg(ChatBrowser::msgChangedNick(gender, nick, newNick));
  }
}


/*!
 * \brief Обработка изменения профиля пользователя.
 *
 * При наличии пользователя в списке функция обновляет информацию о пользователе,
 * а также если имеются открытые приваты, то вкладка также обновляется.
 *
 * \param gender Новый пол пользователя.
 * \param nick   Ник пользователя.
 * \param name   Новое полное имя.
 */
void SChatWindow::newProfile(quint8 gender, const QString &nick, const QString &name)
{
  if (m_users->isUser(nick)) {
    AbstractProfile profile(m_users->profile(nick));
    profile.setGender(gender);
    profile.setNick(nick);
    profile.setFullName(name);
    m_users->update(nick, profile);

    int index = tabIndex(nick);
    if (index != -1) {
      m_tabs->setTabToolTip(index, UserView::userToolTip(profile));
      AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->widget(index));
      tab->setIcon(QIcon(":/images/" + profile.gender() + ".png"));

      if (!tab->notice())
        m_tabs->setTabIcon(index, tab->icon());
    }
  }
}


/*!
 * Добавление нового пользователя.
 *
 * Если не удалось добавить пользователя в \a m_users, то выходим из функции.
 * Если ник равен нашему собственному нику, то принудительно отключаем эхо.
 * Если включено эхо, добавляем в основной канал и приваты, сообщение о новом участнике
 * и если есть открытый приват вкладка также обновляется.
 *
 * Слот вызывается по событию в \a m_clientService.
 *
 * \param list    Стандартный список, содержащий в себе полные данные пользователя.
 * \param echo    Необходимость добавить в канал уведомление о новом пользователе при echo == 1.
 * \param numeric Не используется.
 */
void SChatWindow::newUser(const QStringList &list, quint8 echo, quint8 /*numeric*/)
{
  AbstractProfile profile(list);
  QString nick = profile.nick();

  if (!m_users->add(profile))
    return;

  if (m_profile->nick() == nick)
    echo = 0;

  if (echo == 1) {
    QString msg = ChatBrowser::msgNewUser(profile.genderNum(), nick);
    m_main->msg(msg);

    int index = tabIndex(nick);
    if (index != -1) {
      AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->widget(index));
      if (tab->type() == AbstractTab::Private) {
        m_tabs->setTabToolTip(index, UserView::userToolTip(profile));
        tab->setIcon(QIcon(":/images/" + profile.gender() + ".png"));

        if (!tab->notice())
          m_tabs->setTabIcon(index, tab->icon());

        tab->msg(msg);
      }
    }
  }
}


/** [private slots]
 *
 */
void SChatWindow::privateMessage(quint8 flag, const QString &nick, const QString &message)
{
#ifdef SCHAT_DEBUG
  qDebug() << "SChatWindow::privateMessage()" << flag << nick << message;
#endif

  if (!m_users->isUser(nick))
    return;

  Tab *tab = 0;
  int index = tabIndex(nick);

  if (index == -1) {
    AbstractProfile profile(m_users->profile(nick));
    tab = new Tab(QIcon(":/images/" + profile.gender() + ".png"), m_settings, this);
    tab->setChannel(nick);
    index = m_tabs->addTab(tab, tab->icon(), nick);
    m_tabs->setTabToolTip(index, UserView::userToolTip(profile));
    m_tabs->setCurrentIndex(index);
    connect(tab, SIGNAL(nickClicked(const QString &)), m_users, SLOT(nickClicked(const QString &)));
  }
  else
    tab = qobject_cast<Tab *>(m_tabs->widget(index));

  if (tab)
    if (flag == 1)
      tab->msgNewMessage(m_profile->nick(), message);
    else
      tab->msgNewMessage(nick, message);

  startNotice(index);
}


/*!
 * \brief Обработка отправки сообщения.
 */
void SChatWindow::sendMsg(const QString &message)
{
  AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->currentWidget());

  if (parseCmd(tab, message))
    return;

  if (m_clientService) {
    QString channel;
    m_tabs->currentIndex() == 0 ? channel = "" : channel = m_tabs->tabText(m_tabs->currentIndex());
    if (m_clientService->sendMessage(channel, message))
      m_send->clear();
  }
}


/*!
 * \brief Показ сообщения от сервера.
 */
void SChatWindow::serverMessage(const QString &msg)
{
  if (m_tabs->currentIndex() == 0)
    m_main->msg(msg);
  else {
    AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->currentWidget());
    tab->msg(msg);
  }
}


void SChatWindow::settings()
{
  if (isHidden())
    show();

  if (!m_settingsDialog) {
    m_settingsDialog = new SettingsDialog(m_profile, m_settings, this);
    m_settingsDialog->show();
  }

  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
    m_settingsDialog->setPage(action->data().toInt());

  m_settingsDialog->activateWindow();
}


/** [private slots]
 *
 */
#ifdef SCHAT_UPDATE
void SChatWindow::update()
{
  if (!m_updateTimer->isActive())
    m_updateTimer->start();

  if (!m_updateNotify) {
    m_updateNotify = new UpdateNotify(m_settings, this);
    connect(m_updateNotify, SIGNAL(done(int)), SLOT(updateGetDone(int)));
  }

  m_updateNotify->execute();
}
#endif


/** [private slots]
 *
 */
void SChatWindow::settingsChanged(int notify)
{
  switch (notify) {
    case Settings::NetworkSettingsChanged:
    case Settings::ServerChanged:
      m_clientService->connectToHost();
      break;

    case Settings::ProfileSettingsChanged:
      m_clientService->sendNewProfile();
      break;

    case Settings::ByeMsgChanged:
      m_clientService->sendByeMsg();
      break;

    #ifdef SCHAT_UPDATE
    case Settings::UpdateSettingsChanged:
      m_updateTimer->setInterval(m_settings->getInt("Updates/CheckInterval") * 60 * 1000);
      break;
    #endif

    default:
      break;
  }
}


/*!
 * При необходимости сбрасывает уведомление с текущей вкладки и общее уведомление в трее.
 * Если нет ни одной вкладки с уведомление о новом сообщении, то сбрасываем уведомление в трее.
 *
 * Слот вызывается при изменении текущей вкладки и при событии QEvent::WindowActivate.
 *
 * \param index Номер текущей вкладки.
 */
void SChatWindow::stopNotice(int index)
{
  if (index == -1)
    return;

  AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->widget(index));
  if (tab->notice()) {
    m_tabs->setTabIcon(index, tab->icon());
    tab->notice(false);
  }

  int count = m_tabs->count();
  for (int i = 0; i < count; ++i) {
    AbstractTab *t = static_cast<AbstractTab *>(m_tabs->widget(i));
    if (t->notice())
      return;
  }

  m_tray->notice(false);
}


/** [private slots]
 * Слот вызывается когда в `m_clientService` нет активного подключения.
 */
void SChatWindow::unconnected(bool echo)
{
  m_statusLabel->setText(tr("Не подключено"));
  m_users->clear();

  if (echo)
    m_main->msg(ChatBrowser::msgDisconnect());
}


/** [private slots]
 * Выход удалённого пользователя из чата, опкод `OpcodeUserLeave`.
 */
void SChatWindow::userLeave(const QString &nick, const QString &bye, quint8 flag)
{
  if (m_users->isUser(nick)) {
    QStringList list = m_users->profile(nick);
    m_users->remove(nick);

    if (flag == 1) {
      AbstractProfile profile(list);
      int index = tabIndex(nick);
      if (index != -1) {
        AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->widget(index));
        if (tab->type() == AbstractTab::Private)
          tab->msg(ChatBrowser::msgUserLeft(profile.genderNum(), nick, bye));
      }

      m_main->msg(ChatBrowser::msgUserLeft(profile.genderNum(), nick, bye));
    }
  }
}


/** [private slots]
 * Слот вызывается при завершении работы программы обновления
 */
#ifdef SCHAT_UPDATE
void SChatWindow::updateGetDone(int code)
{
  if (!m_updateNotify)
    m_updateNotify->deleteLater();

  QSettings s(qApp->applicationDirPath() + "/schat.conf", QSettings::IniFormat, this);

  if (code == 0) {
    QString version = s.value("Updates/LastDownloadedCoreVersion", "").toString();
    m_tray->showMessage(tr("Доступно обновление до версии %1").arg(version), tr("Щёлкните здесь для того чтобы установить это обновление прямо сейчас."), QSystemTrayIcon::Information, 60000);
  }
  else
    s.setValue("Updates/ReadyToInstall", false);
}
#endif


/** [private slots]
 *
 */
void SChatWindow::welcomeOk()
{
  m_welcome->deleteLater();

  m_clientService->connectToHost();
}


bool SChatWindow::eventFilter(QObject *object, QEvent *event)
{
  if (m_tabs == object) {
    bool mousRel  = event->type() == QEvent::MouseButtonRelease;

    if (mousRel) {
      QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
      int index = -1;
      QTabBar *tabBar = qFindChild<QTabBar*>(m_tabs);
      for (int i = 0; i < tabBar->count(); ++i) {
        if (tabBar->tabRect(i).contains(mouseEvent->pos())) {
          index = i;
          break;
        }
      }
      if (index > -1 && (mouseEvent->button() == Qt::MidButton)) {
        closeTab(index);
        return true;
      }
    }
  }

  return QMainWindow::eventFilter(object, event);
}


/** [private]
 *
 */
bool SChatWindow::parseCmd(AbstractTab *tab, const QString &message)
{
  QString text     = ChannelLog::toPlainText(message).trimmed();
  QString textFull = text;
  text = text.toLower();

  /// /bye
  if (text.startsWith("/bye")) {
    if (text.startsWith("/bye "))
      m_clientService->sendByeMsg(textFull.mid(textFull.indexOf(QChar(' '))));

    m_clientService->quit();
  }
  /// /clear
  else if (text == "/clear") {
    tab->clear();
  }
  /// /exit
  else if (text == "/exit") {
    closeChat();
  }
  /// /help
  else if (text.startsWith("/help")) {
    if (text.startsWith("/help "))
      cmdHelp(tab, textFull.mid(textFull.indexOf(QChar(' '))).trimmed());
    else
      cmdHelp(tab, "");
  }
  /// /log
  else if (text == "/log") {
    QDesktopServices::openUrl(QUrl::fromLocalFile(qApp->applicationDirPath() + "/log"));
  }
  /// /nick
  else if (text.startsWith("/nick ")) {
    QString newNick = textFull.mid(textFull.indexOf(QChar(' ')));
    if (AbstractProfile::isValidNick(newNick) && m_profile->nick() != newNick) {
      m_profile->setNick(newNick);
      m_clientService->sendNewProfile();
    }
  }
  else
    return false;

  m_send->clear();
  return true;
}


/** [private]
 *
 */
int SChatWindow::tabIndex(const QString &s, int start) const
{
  int count = m_tabs->count();
  int tab = -1;

  if (count > start)
    for (int i = start; i <= count; ++i)
      if (m_tabs->tabText(i) == s) {
        tab = i;
        break;
      }

  return tab;
}


void SChatWindow::cmdHelp(AbstractTab *tab, const QString &cmd)
{
  if (m_cmds.isEmpty()) {
    m_cmds.insert("bye",    tr("<b>/bye [текст сообщения]</b><span class='info'> - Отключится от сервера/сети, опционально можно указать альтернативное сообщение о выходе.</span>"));
    m_cmds.insert("clear",  tr("<b>/clear</b><span class='info'> - Очистка окна чата.</span>"));
    m_cmds.insert("exit",   tr("<b>/exit</b><span class='info'> - Выход из чата.</span>"));
    m_cmds.insert("help",   tr("<b>/help</b><span class='info'> - Отображает подсказу о командах.</span>"));
    m_cmds.insert("log",    tr("<b>/log</b><span class='info'> - Открывает папку с файлами журнала чата."));
    m_cmds.insert("me",     tr("<b>/me &lt;текст сообщения&gt;</b><span class='info'> - Отправка сообщения о себе от третьего лица, например о том что вы сейчас делаете.</span>"));
    m_cmds.insert("nick",   tr("<b>/nick &lt;новый ник&gt;</b><span class='info'> - Позволяет указать новый ник, если указанный ник уже занят, произойдёт автоматическое отключение.</span>"));
    m_cmds.insert("server", tr("<b>/server</b><span class='info'> - Просмотр информации о сервере</span>"));
  }

  if (cmd.isEmpty()) {
    QString out = tr("<b class='info'>Доступные команды:</b><br />");
    out += "<b>";

    QMapIterator<QString, QString> i(m_cmds);
    while (i.hasNext()) {
      i.next();
      out += ('/' + i.key() + "<br />");
    }

    out += "</b>";
    out += tr("<span class='info'>Используйте <b>/help команда</b>, для просмотра подробной информации о команде.</span>");
    tab->msg(out);
    return;
  }

  QString command = cmd.toLower();
  if (command.startsWith(QChar('/'))) {
    command = command.mid(1);
  }

  if (m_cmds.contains(command)) {
    tab->msg(m_cmds.value(command));
  }
  else
    tab->msg(ChatBrowser::msgUnknownCmd(command));
}


/** [private]
 *
 */
void SChatWindow::createActions()
{
  // О Программе...
  m_aboutAction = new QAction(QIcon(":/images/logo16.png"), tr("О Программе..."), this);
  connect(m_aboutAction, SIGNAL(triggered()), SLOT(about()));

  // Закрыть вкладку
  m_closeTabAction = new QAction(QIcon(":/images/tab_close.png"), tr("Закрыть вкладку"), this);
  m_closeTabAction->setStatusTip(tr("Закрыть вкладку"));
  connect(m_closeTabAction, SIGNAL(triggered()), SLOT(closeTab()));

  // Смайлики...
  m_emoticonsSetAction = new QAction(QIcon(":/images/emoticon.png"), tr("Смайлики..."), this);
  m_emoticonsSetAction->setData(SettingsDialog::EmoticonsPage);
  connect(m_emoticonsSetAction, SIGNAL(triggered()), SLOT(settings()));

  // Интерфейс...
  m_interfaceSetAction = new QAction(QIcon(":/images/appearance.png"), tr("Интерфейс..."), this);
  m_interfaceSetAction->setData(SettingsDialog::InterfacePage);
  connect(m_interfaceSetAction, SIGNAL(triggered()), SLOT(settings()));

  // Сеть...
  m_networkSetAction = new QAction(QIcon(":/images/network.png"), tr("Сеть..."), this);
  m_networkSetAction->setData(SettingsDialog::NetworkPage);
  connect(m_networkSetAction, SIGNAL(triggered()), SLOT(settings()));

  // Личные данные...
  m_profileSetAction = new QAction(QIcon(":/images/profile.png"), tr("Личные данные..."), this);
  m_profileSetAction->setShortcut(tr("Ctrl+F12"));
  m_profileSetAction->setData(SettingsDialog::ProfilePage);
  connect(m_profileSetAction, SIGNAL(triggered()), SLOT(settings()));

  // Обновления...
  #ifdef SCHAT_UPDATE
  updateSetAction = new QAction(QIcon(":/images/update.png"), tr("Обновления..."), this);
  updateSetAction->setData(SettingsDialog::UpdatePage);
  connect(updateSetAction, SIGNAL(triggered()), SLOT(settings()));
  #endif

  // Выход из программы
  m_quitAction = new QAction(QIcon(":/images/quit.png"), tr("&Выход"), this);
  connect(m_quitAction, SIGNAL(triggered()), SLOT(closeChat()));
}


/** [private]
 *
 */
void SChatWindow::createCornerWidgets()
{
//  QToolButton *addTabButton = new QToolButton(this);
//  addTabButton->setDefaultAction(addTabAction);
//  addTabButton->setAutoRaise(true);
//  tabWidget->setCornerWidget(addTabButton, Qt::TopLeftCorner);

  QToolButton *closeTabButton = new QToolButton(this);
  closeTabButton->setDefaultAction(m_closeTabAction);
  closeTabButton->setAutoRaise(true);
  m_tabs->setCornerWidget(closeTabButton, Qt::TopRightCorner);
}


/** [private]
 *
 */
void SChatWindow::createService()
{
  m_clientService = new ClientService(m_profile, &m_settings->network, this);
  connect(m_clientService, SIGNAL(connecting(const QString &, bool)), SLOT(connecting(const QString &, bool)));
  connect(m_clientService, SIGNAL(unconnected(bool)), SLOT(unconnected(bool)));
  connect(m_clientService, SIGNAL(newUser(const QStringList &, quint8, quint8)), SLOT(newUser(const QStringList &, quint8, quint8)));
  connect(m_clientService, SIGNAL(accessGranted(const QString &, const QString &, quint16)), SLOT(accessGranted(const QString &, const QString &, quint16)));
  connect(m_clientService, SIGNAL(userLeave(const QString &, const QString &, quint8)), SLOT(userLeave(const QString &, const QString &, quint8)));
  connect(m_clientService, SIGNAL(accessDenied(quint16)), SLOT(accessDenied(quint16)));
  connect(m_clientService, SIGNAL(message(const QString &, const QString &)), SLOT(message(const QString &, const QString &)));
  connect(m_clientService, SIGNAL(privateMessage(quint8, const QString &, const QString &)), SLOT(privateMessage(quint8, const QString &, const QString &)));
  connect(m_clientService, SIGNAL(fatal()), SLOT(fatal()));
  connect(m_clientService, SIGNAL(serverMessage(const QString &)), SLOT(serverMessage(const QString &)));
  connect(m_clientService, SIGNAL(newNick(quint8, const QString &, const QString &, const QString &)), SLOT(newNick(quint8, const QString &, const QString &, const QString &)));
  connect(m_clientService, SIGNAL(newProfile(quint8, const QString &, const QString &)), SLOT(newProfile(quint8, const QString &, const QString &)));
  connect(m_clientService, SIGNAL(newLink(quint8, const QString &, const QString &)), SLOT(newLink(quint8, const QString &, const QString &)));
  connect(m_clientService, SIGNAL(linkLeave(quint8, const QString &, const QString &)), SLOT(linkLeave(quint8, const QString &, const QString &)));
}


/** [private]
 *
 */
void SChatWindow::createToolButtons()
{
  QMenu *iconMenu = new QMenu(this);
  iconMenu->addAction(m_profileSetAction);
  iconMenu->addAction(m_networkSetAction);
  iconMenu->addAction(m_interfaceSetAction);
  iconMenu->addAction(m_emoticonsSetAction);

  #ifdef SCHAT_UPDATE
  iconMenu->addAction(updateSetAction);
  #endif

  // Настройка
  m_settingsButton = new QToolButton(this);
  m_settingsButton->setIcon(QIcon(":/images/settings.png"));
  m_settingsButton->setToolTip(tr("Настройка..."));
  m_settingsButton->setAutoRaise(true);
  m_settingsButton->setMenu(iconMenu);
  m_settingsButton->setPopupMode(QToolButton::InstantPopup);

  QToolButton *aboutButton = new QToolButton(this);
  aboutButton->setDefaultAction(m_aboutAction);
  aboutButton->setAutoRaise(true);

  QFrame *line = new QFrame(this);
  line->setFrameShape(QFrame::VLine);
  line->setFrameShadow(QFrame::Sunken);

  m_toolsLay->addWidget(line);
  m_toolsLay->addWidget(m_settingsButton);
  m_toolsLay->addWidget(aboutButton);
  m_toolsLay->addStretch();
  m_toolsLay->setSpacing(0);
}


void SChatWindow::createTrayIcon()
{
  m_trayMenu = new QMenu(this);
  m_trayMenu->addAction(m_aboutAction);
  m_trayMenu->addAction(m_profileSetAction);
  m_trayMenu->addSeparator();
  m_trayMenu->addAction(m_quitAction);

  m_tray = new TrayIcon(this);
  m_tray->setContextMenu(m_trayMenu);
  m_tray->show();
}


void SChatWindow::hideChat()
{
  if (m_settingsDialog)
    m_settingsDialog->hide();

  if (m_about)
    m_about->hide();

  hide();
}


/*!
 * \brief Восстанавливает геометрию окна.
 */
void SChatWindow::restoreGeometry()
{
  resize(m_settings->size());
  QPoint pos = m_settings->pos();
  if (pos.x() != -999 && pos.y() != -999)
    move(pos);

  m_splitter->restoreState(m_settings->splitter());
}


/*!
 * \brief Сохраняет еометрию окна.
 */
void SChatWindow::saveGeometry()
{
  m_settings->setPos(pos());
  m_settings->setSize(size());
  m_settings->setSplitter(m_splitter->saveState());
}


/*!
 * Показ окна чата.
 */
void SChatWindow::showChat()
{
  setWindowState(windowState() & ~Qt::WindowMinimized);
  show();
  activateWindow();

  if (m_about)
    m_about->show();

  if (m_settingsDialog)
    m_settingsDialog->show();
}


/*!
 * При необходимости вызывает уведомление о новом сообщении.
 * Механизм уведомления запускается, если номер текущей вкладки не равен номеру вкладки
 * с уведомлением или окно чата неактивно.
 *
 * Функция вызывается при получении нового сообщения в главном канале или при приватном сообщении.
 *
 * \param index Номер вкладки, в которой есть новое сообщение.
 */
void SChatWindow::startNotice(int index)
{
  if (index == -1)
    return;

  if ((m_tabs->currentIndex() != index) || (!isActiveWindow())) {
    AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->widget(index));
    tab->notice(true);
    m_tabs->setTabIcon(index, QIcon(":/images/notice.png"));
    m_tray->notice(true);
  }
}


/** [private]
 *
 */
void SChatWindow::uniqueNick()
{
  m_profile->setNick(m_profile->nick() + QString().setNum(qrand() % 99));
}