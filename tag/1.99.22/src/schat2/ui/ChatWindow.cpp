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

#include <QApplication>
#include <QKeyEvent>
#include <QTimer>
#include <QVBoxLayout>

#include "ChatCore.h"
#include "ChatNotify.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/SimpleClient.h"
#include "debugstream.h"
#include "ui/ChatWindow.h"
#include "ui/SendWidget.h"
#include "ui/StatusBar.h"
#include "ui/StatusMenu.h"
#include "ui/TabWidget.h"

#if defined(Q_WS_WIN)
  #include "qtwin/qtwin.h"
  #include <qt_windows.h>
  #define WM_DWMCOMPOSITIONCHANGED 0x031E // Composition changed window message
#endif

#if defined(SCHAT_OPTION)
  #undef SCHAT_OPTION
  #define SCHAT_OPTION(x) m_settings->value(QLatin1String(x))
#endif

ChatWindow::ChatWindow(QWidget *parent)
  : QMainWindow(parent)
  , m_settings(ChatCore::settings())
{
  new StatusMenu(this);

  m_central = new QWidget(this);
  m_tabs = new TabWidget(this);
  m_statusBar = new StatusBar(this);
  m_send = new SendWidget(this);
  pageChanged(-1, false);

  setStatusBar(m_statusBar);

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

  resize(SCHAT_OPTION("Width").toInt(), SCHAT_OPTION("Height").toInt());

  connect(m_send, SIGNAL(send(const QString &)), ChatCore::i(), SLOT(send(const QString &)));
  connect(m_settings, SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
  connect(m_tabs, SIGNAL(pageChanged(int, bool)), SLOT(pageChanged(int, bool)));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));

  setWindowTitle(QApplication::applicationName());
}


void ChatWindow::showChat()
{
  SCHAT_DEBUG_STREAM(this << "showChat()")

  if (!SCHAT_OPTION("Maximized").toBool()) {
    setWindowState(windowState() & ~Qt::WindowMinimized);
    show();
  }
  else
    showMaximized();

  activateWindow();

  if (m_send->isVisible())
    m_send->setInputFocus();
}


void ChatWindow::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();
  else if (event->type() == QEvent::ActivationChange)
    m_activationChanged = QTime::currentTime();

  QMainWindow::changeEvent(event);
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
  if (!SCHAT_OPTION("Maximized").toBool()) {
    m_settings->setValue(QLatin1String("Width"), width(), false);
    m_settings->setValue(QLatin1String("Height"), height(), false);
  }

  QMainWindow::resizeEvent(event);
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
  ChatClient::io()->leave();
  hideChat();

  QApplication::quit();
}


void ChatWindow::notify(const Notify &notify)
{
  if (notify.type() == Notify::Quit) {
    closeChat();
  }
  else if (notify.type() == Notify::ToggleVisibility) {
    if (isHidden() || isMinimized()) {
      showChat();
      return;
    }

    if (isActiveWindow() || qAbs(m_activationChanged.msecsTo(QTime::currentTime())) < QApplication::doubleClickInterval())
      hideChat();
    else
      showChat();
  }
  else if (notify.type() == Notify::ShowChat) {
    showChat();
  }
}


void ChatWindow::pageChanged(int type, bool visible)
{
  Q_UNUSED(type)

  m_send->setVisible(visible);
  if (visible)
    m_send->setInputFocus();
}


void ChatWindow::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == QLatin1String("Maximized")) {
    if (value.toBool())
      showMaximized();
    else
      showNormal();
  }
  else if (key == QLatin1String("Width") || key == QLatin1String("Height")) {
    resize(SCHAT_OPTION("Width").toInt(), SCHAT_OPTION("Height").toInt());
  }
}


/*!
 * Скрытие окна чата.
 */
void ChatWindow::hideChat()
{
  SCHAT_DEBUG_STREAM(this << "hideChat()")
  m_settings->setValue(QLatin1String("Maximized"), isMaximized(), false);
  hide();
}


void ChatWindow::retranslateUi()
{
}


#if defined(Q_WS_WIN)
void ChatWindow::setWindowsAero()
{
  if (SCHAT_OPTION("WindowsAero").toBool() && QtWin::isCompositionEnabled()) {
    QtWin::extendFrameIntoClientArea(this);
    m_mainLay->setMargin(0);
  }
  else {
    m_mainLay->setContentsMargins(3, 3, 3, 0);
  }
}
#endif
