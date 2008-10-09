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

#include "daemonui.h"
#include "version.h"
#include "daemonsettings.h"

/*!
 * \brief Конструктор класса DaemonUi.
 */
DaemonUi::DaemonUi(QWidget *parent)
  : QDialog(parent)
{
  setWindowFlags(Qt::Tool);

  m_settings = new DaemonSettings(qApp->applicationDirPath() + "/schatd.conf", this);
  m_checkTimer.setInterval(5000);
  connect(&m_checkTimer, SIGNAL(timeout()), SLOT(checkStart()));

  createActions();
  createButtons();

  // Кнопки управления сервером
  QFrame *line1 = new QFrame(this);
  line1->setFrameShape(QFrame::VLine);
  line1->setFrameShadow(QFrame::Sunken);

  QGroupBox *controlGroup = new QGroupBox(tr("Управление"), this);
  QHBoxLayout *controlGroupLay = new QHBoxLayout(controlGroup);
  controlGroupLay->setMargin(2);
  controlGroupLay->setSpacing(0);
  controlGroupLay->addWidget(m_startButton);
  controlGroupLay->addWidget(m_stopButton);
  controlGroupLay->addWidget(m_restartButton);
  controlGroupLay->addWidget(line1);
  controlGroupLay->addWidget(m_settingsButton);

  // Отображение статуса
  m_statusLabel = new QLabel(this);
  m_ledLabel = new QLabel(this);
  QGroupBox *statusGroup = new QGroupBox(tr("Статус"), this);
  QHBoxLayout *statusGroupLay = new QHBoxLayout(statusGroup);
  statusGroupLay->setMargin(2);
  statusGroupLay->setSpacing(0);
  statusGroupLay->addWidget(m_statusLabel);
  statusGroupLay->addStretch();
  statusGroupLay->addWidget(m_ledLabel);

  QHBoxLayout *controlLay = new QHBoxLayout;
  controlLay->addWidget(controlGroup);
  controlLay->addWidget(statusGroup);

  // Кнопки внизу окна
  QHBoxLayout *bottomLay = new QHBoxLayout;
  bottomLay->addStretch();
  bottomLay->addWidget(m_hideButton);
  bottomLay->addWidget(m_quitButton);

  // Все основные виджеты
  QVBoxLayout *bodyLay = new QVBoxLayout;
  bodyLay->setMargin(6);
  bodyLay->setSpacing(6);
  bodyLay->addLayout(controlLay);
  bodyLay->addLayout(bottomLay);

  // Надпись вверху окна
  m_aboutLabel = new QLabel(QString(
      "<html><body style='color:#333;margin:6px;'>"
      "<h4 style='margin-bottom:0px;'>IMPOMEZIA Simple Chat Daemon UI %1</h4>"
      "<p style='margin-left:16px;margin-top:5px;'>Copyright © 2008 IMPOMEZIA &lt;<a href='mailto:schat@impomezia.com' style='color:#1a4d82;'>schat@impomezia.com</a>&gt; GPL</p>"
      "</body></html>").arg(SCHAT_VERSION), this);
  m_aboutLabel->setStyleSheet("background:#fff; border:4px solid #fff;");
  m_aboutLabel->setOpenExternalLinks(true);

  QFrame *line2 = new QFrame(this);
  line2->setFrameShape(QFrame::HLine);
  line2->setFrameShadow(QFrame::Sunken);

  // End
  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);
  mainLay->addWidget(m_aboutLabel);
  mainLay->addWidget(line2);
  mainLay->addLayout(bodyLay);

  createTray();
  setStatus(Unknown);

  setWindowTitle(tr("Управление сервером"));
  setWindowIcon(QIcon(":/images/logo16-green.png"));

  m_appDir = qApp->applicationDirPath();
  #ifdef Q_WS_WIN
    m_daemonFile = m_appDir + "/schatd.exe";
  #else
    m_daemonFile = m_appDir + "/schatd";
  #endif

  QTimer::singleShot(0, this, SLOT(init()));
}


#ifndef SCHAT_NO_SINGLE_APP
void DaemonUi::handleMessage(const QString& message)
{
  QStringList list = message.split(", ");

  if (list.contains("-exit")) {
    qApp->quit();
    return;
  }

  if (!arguments(list))
    showUi();
}
#endif


void DaemonUi::checkStart()
{
  if (m_status == Starting || m_status == Restarting)
    setStatus(Error);
}


void DaemonUi::exit()
{
  m_client->exit();
  QApplication::quit();
}


void DaemonUi::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::MiddleClick:
      if (isHidden())
        showUi();
      else
        hide();

    default:
      break;
  }
}


void DaemonUi::init()
{
  if (!QFile::exists(m_daemonFile)) {
    setStatus(Error);
    return;
  }

  m_settings->read();

  QSettings s(qApp->applicationDirPath() + "/schat.conf", QSettings::IniFormat, this);
  qApp->setStyle(s.value("Style", "Plastique").toString());

  m_client = new LocalClientService(this);
  connect(m_client, SIGNAL(notify(LocalClientService::Reason)), SLOT(notify(LocalClientService::Reason)));
  m_client->connectToServer();

  QStringList args = QApplication::arguments();
  args.takeFirst();
  arguments(args);
}


void DaemonUi::notify(LocalClientService::Reason reason)
{
  switch (reason) {
    case LocalClientService::Start:
      m_checkTimer.stop();
      setStatus(Started);
      break;

    case LocalClientService::Stop:
      if (!(m_status == Starting || m_status == Restarting))
        setStatus(Stopped);
      break;
  }
}


void DaemonUi::restart()
{
  if (m_status == Started)
    setStatus(Restarting);
  else
    setStatus(Starting);

  m_client->exit();
  QTimer::singleShot(1000, this, SLOT(start()));
}


void DaemonUi::settings()
{
  if (isHidden())
    show();

  if (!m_settingsDialog) {
    m_settingsDialog = new DaemonSettingsDialog(m_settings, this);
    m_settingsDialog->show();
  }

  m_settingsDialog->activateWindow();
}


void DaemonUi::start()
{
  if (!QProcess::startDetached('"' + m_daemonFile + '"'))
    setStatus(Error);
  else {
    if (m_status != Restarting)
      setStatus(Starting);

    m_checkTimer.start();
    m_client->connectToServer();
  }
}


bool DaemonUi::arguments(const QStringList &args)
{
  if (args.contains("-start") && m_startAction->isEnabled()) {
    start();
    return true;
  }
  else if (args.contains("-stop") && m_stopAction->isEnabled()) {
    stop();
    return true;
  }
  else if (args.contains("-restart") && m_restartAction->isEnabled()) {
    restart();
    return true;
  }

  return false;
}


/*!
 * Создаёт объекты QAction
 */
void DaemonUi::createActions()
{
  m_quitAllAction = new QAction(QIcon(":/images/quit.png"), tr("Выход с о&становкой сервера"), this);
  connect(m_quitAllAction, SIGNAL(triggered()), SLOT(exit()));

  m_quitAction = new QAction(QIcon(":/images/logout.png"), tr("&Выход"), this);
  connect(m_quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

  m_restartAction = new QAction(QIcon(":/images/restart.png"), tr("&Перезапуск"), this);
  connect(m_restartAction, SIGNAL(triggered()), SLOT(restart()));

  m_startAction = new QAction(QIcon(":/images/play.png"), tr("&Запуск"), this);
  connect(m_startAction, SIGNAL(triggered()), SLOT(start()));

  m_stopAction = new QAction(QIcon(":/images/stop.png"), tr("&Остановка"), this);
  connect(m_stopAction, SIGNAL(triggered()), SLOT(stop()));

  m_settingsAction = new QAction(QIcon(":/images/daemonsettings.png"), tr("&Настройка..."), this);
  connect(m_settingsAction, SIGNAL(triggered()), SLOT(settings()));
}


/*!
 * Создаёт кнопки
 */
void DaemonUi::createButtons()
{
  m_hideButton = new QPushButton(QIcon(":/images/ok.png"), tr("Скрыть"), this);
  m_hideButton->setToolTip(tr("Скрыть окно программы"));
  connect(m_hideButton, SIGNAL(clicked(bool)), SLOT(hide()));

  m_quitButton = new QPushButton(QIcon(":/images/logout.png"), "", this);
  m_quitButton->setToolTip(tr("Выход"));
  connect(m_quitButton, SIGNAL(clicked(bool)), qApp, SLOT(quit()));

  m_startButton = new QToolButton(this);
  m_startButton->setAutoRaise(true);
  m_startButton->setIconSize(QSize(22, 22));
  m_startButton->setDefaultAction(m_startAction);

  m_stopButton = new QToolButton(this);
  m_stopButton->setAutoRaise(true);
  m_stopButton->setIconSize(QSize(22, 22));
  m_stopButton->setDefaultAction(m_stopAction);

  m_restartButton = new QToolButton(this);
  m_restartButton->setAutoRaise(true);
  m_restartButton->setIconSize(QSize(22, 22));
  m_restartButton->setDefaultAction(m_restartAction);

  m_settingsButton = new QToolButton(this);
  m_settingsButton->setAutoRaise(true);
  m_settingsButton->setIconSize(QSize(22, 22));
  m_settingsButton->setDefaultAction(m_settingsAction);
}


void DaemonUi::createTray()
{
  m_menu = new QMenu(this);
  m_menu->addAction(m_startAction);
  m_menu->addAction(m_stopAction);
  m_menu->addAction(m_restartAction);
  m_menu->addAction(m_settingsAction);
  m_menu->addSeparator();
  m_menu->addAction(m_quitAllAction);
  m_menu->addAction(m_quitAction);

  m_tray = new QSystemTrayIcon(this);
  m_tray->setIcon(QIcon(":/images/logo16-gray.png"));
  m_tray->setToolTip(tr("IMPOMEZIA Simple Chat Daemon UI %1").arg(SCHAT_VERSION));
  m_tray->setContextMenu(m_menu);

  connect(m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
  m_tray->show();
}


void DaemonUi::setActionsState(bool start, bool stop, bool restart, bool quit, bool settings)
{
  m_startAction->setEnabled(start);
  m_stopAction->setEnabled(stop);
  m_restartAction->setEnabled(restart);
  m_quitAllAction->setEnabled(quit);
  m_settingsAction->setEnabled(settings);
}


void DaemonUi::setLedColor(LedColor color)
{
  QString img;
  QString icon;

  if (color == Green) {
    img = ":/images/led/greenled.png";
    icon = ":/images/logo16-green.png";
  }
  else if (color == Yellow) {
    img = ":/images/led/yellowled.png";
    icon = ":/images/logo16-yellow.png";
  }
  else {
    img = ":/images/led/redled.png";
    icon = ":/images/logo16-gray.png";
  }

  m_ledLabel->setPixmap(QPixmap(img));
  m_tray->setIcon(QIcon(icon));
}


void DaemonUi::setStatus(DaemonUi::Status status)
{
  m_status = status;
  switch (status) {
    case Unknown:
      setActionsState(false, false, false, false, false);
      setLedColor(Yellow);
      m_statusLabel->setText(tr("<b style='color:#bf8c00';>&nbsp;Инициализация...</b>"));
      break;

    case Error:
      setActionsState(false, false, false, false);
      setLedColor();
      m_statusLabel->setText(tr("<b style='color:#c00;'>&nbsp;Ошибка</b>"));
      break;

    case Starting:
      setActionsState(false, false, false);
      setLedColor(Yellow);
      m_statusLabel->setText(tr("<b style='color:#bf8c00';>&nbsp;Запуск...</b>"));
      break;

    case Started:
      setActionsState(false);
      setLedColor(Green);
      m_statusLabel->setText(tr("<b style='color:#090;'>&nbsp;Успешно запущен</b>"));
      break;

    case Stopped:
      setActionsState(true, false, true, false);
      setLedColor();
      m_statusLabel->setText(tr("<b style='color:#c00;'>&nbsp;Остановлен</b>"));
      break;

    case Restarting:
      setActionsState(false, false, false, false);
      setLedColor(Yellow);
      m_statusLabel->setText(tr("<b style='color:#bf8c00';>&nbsp;Перезапуск...</b>"));
      break;

    default:
      break;
  }
}


void DaemonUi::showUi()
{
  show();
  activateWindow();
}
