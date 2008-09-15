/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.com)
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

/*!
 * \brief Конструктор класса DaemonUi.
 */
DaemonUi::DaemonUi(QWidget *parent)
  : QDialog(parent)
{
  setWindowFlags(Qt::Tool);

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

  m_appDir = qApp->applicationDirPath();
  QTimer::singleShot(0, this, SLOT(init()));
}


#ifndef DISABLE_SINGLE_APP
void DaemonUi::handleMessage(const QString& message)
{
  QStringList list = message.split(", ");

  if (list.contains("-exit")) {
    qApp->quit();
    return;
  }

  showUi();
}
#endif


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
  qDebug() << "DaemonUi::init()";
  
  #ifdef Q_WS_WIN
    QString daemonFile = m_appDir + "/schatd.exe";
  #else
    QString daemonFile = m_appDir + "/schatd";
  #endif

  if (!QFile::exists(daemonFile)) {
    setStatus(Error);
    return;
  }

  m_client = new LocalClientService(this);
  m_client->connectToServer();
  connect(m_client, SIGNAL(notify(LocalClientService::Reason)), SLOT(notify(LocalClientService::Reason)));
}


void DaemonUi::notify(LocalClientService::Reason reason)
{
  qDebug() << "DaemonUi::notify()" << reason;
  if (reason == LocalClientService::Start)
    setStatus(Started);
  else if (reason == LocalClientService::Stop)
    setStatus(Stopped);
}


void DaemonUi::restart()
{
  qDebug() << "DaemonUi::restart()";
  setStatus(Started);
}


void DaemonUi::start()
{
  qDebug() << "DaemonUi::start()";
  setStatus(Started);
}


void DaemonUi::stop()
{
  qDebug() << "DaemonUi::stop()";
  setStatus(Stopped);
}


/*!
 * Создаёт объекты QAction
 */
void DaemonUi::createActions()
{
  m_quitAllAction = new QAction(QIcon(":/images/quit.png"), tr("Выход с &остановкой сервера"), this);

  m_quitAction = new QAction(QIcon(":/images/logout.png"), tr("&Выход"), this);
  connect(m_quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

  m_restartAction = new QAction(QIcon(":/images/restart.png"), tr("&Перезапуск"), this);
  connect(m_restartAction, SIGNAL(triggered()), SLOT(restart()));

  m_startAction = new QAction(QIcon(":/images/play.png"), tr("&Запуск"), this);
  connect(m_startAction, SIGNAL(triggered()), SLOT(start()));

  m_stopAction = new QAction(QIcon(":/images/stop.png"), tr("&Остановка"), this);
  connect(m_stopAction, SIGNAL(triggered()), SLOT(stop()));

  m_settingsAction = new QAction(QIcon(":/images/daemonsettings.png"), tr("&Настройка..."), this);
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
      m_statusLabel->setText(tr("<b style='color:#bf8c00';>&nbsp;Ожидание...</b>"));
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
