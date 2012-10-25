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

#include <QBasicTimer>
#include <QDesktopWidget>
#include <QFile>
#include <QTimerEvent>

#include "ChatAlerts.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "Path.h"
#include "PopupManager.h"
#include "PopupWindow.h"
#include "sglobal.h"
#include "ui/TabWidget.h"

PopupManager::PopupManager(QObject *parent)
  : QObject(parent)
  , m_flashed(true)
  , m_stylesLoaded(false)
{
  m_desktop = new QDesktopWidget();
  m_timeout = ChatCore::settings()->setDefaultAndRead(LS("Alerts/PopupTimeout"), 10).toUInt();

  m_timer = new QBasicTimer();

  connect(ChatAlerts::i(), SIGNAL(popup(Alert)), SLOT(popup(Alert)));
  connect(ChatCore::settings(), SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
}


PopupManager::~PopupManager()
{
  delete m_desktop;
  delete m_timer;
}


void PopupManager::timerEvent(QTimerEvent *event)
{
  if (event->timerId() == m_timer->timerId()) {
    m_flashed = !m_flashed;
    foreach (PopupWindow *window, m_windows) {
      window->setStyleSheet(m_flashed ? m_flashedCSS : m_windowsCSS);
    }
  }

  QObject::timerEvent(event);
}


void PopupManager::popup(const Alert &alert)
{
  if (!m_stylesLoaded) {
    m_textCSS = loadCSS(LS("text"));
    m_windowsCSS = loadCSS(LS("window"));
    m_flashedCSS = m_windowsCSS + loadCSS(LS("flashed"));
    m_stylesLoaded = true;
  }

  if (!m_timer->isActive())
    m_timer->start(1500, this);

  PopupWindow *window = new PopupWindow(alert, m_timeout, m_textCSS);
  window->setStyleSheet(m_flashedCSS);
  connect(window, SIGNAL(destroyed(QObject*)), SLOT(windowDestroyed(QObject*)));

  m_windows.prepend(window);
  layoutWidgets();
}


void PopupManager::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == LS("Alerts/PopupTimeout"))
    m_timeout = value.toUInt();
}


void PopupManager::windowDestroyed(QObject *obj)
{
  m_windows.removeAll(static_cast<PopupWindow *>(obj));
  layoutWidgets();

  if (m_windows.isEmpty()) {
    m_timer->stop();
    m_flashed = true;
  }
}


QString PopupManager::loadCSS(const QString &baseName)
{
  QString out;
  QFile file(LS(":/css/Popup/") + baseName + LS(".css"));

  if (file.open(QFile::ReadOnly)) {
    out = file.readAll();
    file.close();
  }

  file.setFileName(Path::cache() + LS("/popup/") + baseName + LS(".css"));
  if (file.exists() && file.open(QFile::ReadOnly)) {
    out += file.readAll();
    file.close();
  }

  return out;
}


void PopupManager::layoutWidgets()
{
  QRect display = m_desktop->availableGeometry(TabWidget::i());
  int bottom = display.bottom() - PopupWindow::Space;
  foreach (PopupWindow *window, m_windows) {
    if (!window->isVisible())
      window->show();

    QRect geometry = window->frameGeometry();
    window->move(display.right() - geometry.width(), bottom - geometry.height());
    bottom -= geometry.height() + PopupWindow::Space;
  }
}
