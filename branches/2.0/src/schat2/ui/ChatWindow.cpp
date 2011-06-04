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

#include <QApplication>
#include <QKeyEvent>
#include <QTimer>
#include <QVBoxLayout>

#include "debugstream.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "net/SimpleClient.h"
#include "ui/ChatWindow.h"
#include "ui/SendWidget.h"
#include "ui/StatusBar.h"
#include "ui/TabWidget.h"
#include "ui/TrayIcon.h"

#if defined(Q_WS_WIN)
  #include "qtwin/qtwin.h"
  #include <qt_windows.h>
  #define WM_DWMCOMPOSITIONCHANGED 0x031E // Composition changed window message
#endif

#if defined(SCHAT_OPTION)
  #undef SCHAT_OPTION
  #define SCHAT_OPTION(x) m_settings->value(ChatSettings::x)
#endif

ChatWindow::ChatWindow(QWidget *parent)
  : QMainWindow(parent)
{
  m_core = new ChatCore(this);
  m_settings = m_core->settings();
  m_central = new QWidget(this);

  m_tabs = new TabWidget(this);
  m_statusBar = new StatusBar(m_core->client(), this);

  m_send = new SendWidget(this);

  setStatusBar(m_statusBar);

  m_tray = new TrayIcon(this);
  QTimer::singleShot(0, m_tray, SLOT(show()));

  m_mainLay = new QVBoxLayout(m_central);
  m_mainLay->addWidget(m_tabs);
  m_mainLay->addWidget(m_send);
  m_mainLay->setStretchFactor(m_tabs, 999);
  m_mainLay->setStretchFactor(m_send, 1);
  m_mainLay->setMargin(0);
  m_mainLay->setSpacing(0);
  setCentralWidget(m_central);

  #if defined(Q_WS_WIN)
  setWindowsAero();
  #endif

  resize(SCHAT_OPTION(Width).toInt(), SCHAT_OPTION(Height).toInt());

  connect(m_send, SIGNAL(send(const QString &)), SLOT(send(const QString &)));
  connect(m_core, SIGNAL(notify(int, const QVariant &)), SLOT(notify(int, const QVariant &)));
  connect(m_settings, SIGNAL(changed(const QList<int> &)), SLOT(settingsChanged(const QList<int> &)));
  connect(m_tabs, SIGNAL(pageChanged(int, bool)), SLOT(pageChanged(int, bool)));

  setWindowTitle(QApplication::applicationName());
}


void ChatWindow::showChat()
{
  SCHAT_DEBUG_STREAM(this << "showChat()")

  if (!SCHAT_OPTION(Maximized).toBool()) {
    setWindowState(windowState() & ~Qt::WindowMinimized);
    show();
  }
  else
    showMaximized();

  activateWindow();
}


void ChatWindow::closeEvent(QCloseEvent *event)
{
  SCHAT_DEBUG_STREAM(this << "closeEvent()")

  hideChat();
  QMainWindow::closeEvent(event);
}


void ChatWindow::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Escape)
    hideChat();
  else
    QMainWindow::keyPressEvent(event);
}


void ChatWindow::resizeEvent(QResizeEvent *event)
{
  if (!isMaximized()) {
    m_settings->setValue(ChatSettings::Width, width());
    m_settings->setValue(ChatSettings::Height, height());
  }

  QMainWindow::resizeEvent(event);
}


void ChatWindow::showEvent(QShowEvent *event)
{
//  if (!isMaximized()) {
//    resize(SCHAT_OPTION(Width).toInt(), SCHAT_OPTION(Height).toInt());
//  }
  QMainWindow::showEvent(event);
}


#if defined(Q_WS_WIN)
bool ChatWindow::winEvent(MSG *message, long *result)
{
  if (message && message->message == WM_DWMCOMPOSITIONCHANGED) {
    setWindowsAero();
  }

  return QMainWindow::winEvent(message, result);
}
#endif


/*!
 * Закрытие чата.
 */
void ChatWindow::closeChat()
{
  m_core->client()->leave();
  hideChat();

  QApplication::quit();
}


void ChatWindow::notify(int notice, const QVariant &data)
{
  Q_UNUSED(data)

  if (notice == ChatCore::QuitNotice) {
    closeChat();
  }
  else if (notice == ChatCore::ToggleVisibilityNotice) {
    if (isHidden())
      showChat();
    else
      hideChat();
  }
}


void ChatWindow::pageChanged(int type, bool visible)
{
  Q_UNUSED(type)

  m_send->setVisible(visible);
}


void ChatWindow::send(const QString &text)
{
  m_core->send(m_tabs->currentId(), text);
}


void ChatWindow::settingsChanged(const QList<int> &keys)
{
  if (keys.contains(ChatSettings::Maximized)) {
    if (SCHAT_OPTION(Maximized).toBool())
      showMaximized();
    else
      showNormal();
  }
  else if (keys.contains(ChatSettings::Width) || keys.contains(ChatSettings::Height)) {
    resize(SCHAT_OPTION(Width).toInt(), SCHAT_OPTION(Height).toInt());
  }
}


/*!
 * Скрытие окна чата.
 */
void ChatWindow::hideChat()
{
  SCHAT_DEBUG_STREAM(this << "hideChat()")
  m_settings->setValue(ChatSettings::Maximized, isMaximized());
  m_settings->write();
  hide();
}


#if defined(Q_WS_WIN)
void ChatWindow::setWindowsAero()
{
  if (SCHAT_OPTION(WindowsAero).toBool() && QtWin::isCompositionEnabled()) {
    QtWin::extendFrameIntoClientArea(this);
    m_mainLay->setMargin(0);
  }
  else {
    m_mainLay->setContentsMargins(3, 3, 3, 0);
  }
}
#endif
