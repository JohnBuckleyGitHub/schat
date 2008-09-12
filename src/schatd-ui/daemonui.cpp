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
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

  createActions();
  createButtons();

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
  
  m_statusLabel = new QLabel(tr("<b>Ошибка</b>"), this);
  m_ledLabel = new QLabel("<img src=':/images/led/greenled.png' aling='left' />", this);
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

  QHBoxLayout *bottomLay = new QHBoxLayout;
  bottomLay->addStretch();
  bottomLay->addWidget(m_hideButton);
  
  QVBoxLayout *bodyLay = new QVBoxLayout;
  bodyLay->setMargin(6);
  bodyLay->setSpacing(6);
  bodyLay->addLayout(controlLay);
  bodyLay->addLayout(bottomLay);

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

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);
  mainLay->addWidget(m_aboutLabel);
  mainLay->addWidget(line2);
  mainLay->addLayout(bodyLay);
  

  createTray();

  setWindowTitle(tr("Управление сервером"));
}


void DaemonUi::createActions()
{
  m_quitAllAction = new QAction(QIcon(":/images/quit.png"), tr("Выход с &остановкой сервера"), this);
  m_quitAction = new QAction(QIcon(":/images/logout.png"), tr("&Выход"), this);
  m_restartAction = new QAction(QIcon(":/images/restart.png"), tr("&Перезапуск"), this);
  m_startAction = new QAction(QIcon(":/images/play.png"), tr("&Запуск"), this);
  m_stopAction = new QAction(QIcon(":/images/stop.png"), tr("&Остановка"), this);
  m_settingsAction = new QAction(QIcon(":/images/daemonsettings.png"), tr("&Настройка..."), this);
}


/*!
 * Создаёт кнопки
 */
void DaemonUi::createButtons()
{
  m_hideButton = new QPushButton(tr("Скрыть"), this);

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
  m_tray->show();
}
