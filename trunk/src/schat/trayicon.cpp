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

#include "trayicon.h"
#include "settings.h"

/*!
 * \brief Конструктор класса TrayIcon.
 */
TrayIcon::TrayIcon(QObject *parent)
  : QSystemTrayIcon(parent)
{
  m_icon = QIcon(":/images/logo16.png");
  setIcon(m_icon);

  init();
}


/*!
 * \brief Конструктор класса TrayIcon.
 */
TrayIcon::TrayIcon(const QIcon &icon, QObject *parent)
  : QSystemTrayIcon(icon, parent)
{
  m_icon = icon;

  init();
}


/*!
 * Включает/выключает режим нотификации.
 *
 * \param enable true - включить, false - выключить.
 */
void TrayIcon::notice(bool enable)
{
  if (enable && m_timer->isActive())
    return;

  if (!enable && !m_timer->isActive())
    return;

  if (enable) {
    m_timer->start();
    setIcon(m_noticeIcon);
  }
  else {
    m_timer->stop();
    setIcon(m_icon);
  }

  m_normal = !enable;

}


void TrayIcon::notify(int code)
{
  switch (code) {
    case Settings::UpdateReady:
      updateReady();
      break;

    default:
      break;
  }
}


/*!
 * Обработка события таймера \a m_timer.
 * Изменяет иконку на противоположную.
 */
void TrayIcon::timeout()
{
  if (m_normal)
    setIcon(m_noticeIcon);
  else
    setIcon(m_icon);

  m_normal = !m_normal;
}


/*!
 * Инициализация членов класса.
 */
void TrayIcon::init()
{
  m_settings = settings;
  setToolTip(QApplication::applicationName() + " " + QApplication::applicationVersion());
  m_normal = true;
  m_noticeIcon = QIcon(":/images/notice.png");
  m_timer = new QTimer(this);
  m_timer->setInterval(800);
  connect(m_timer, SIGNAL(timeout()), SLOT(timeout()));
  connect(m_settings, SIGNAL(changed(int)), SLOT(notify(int)));
}


/*!
 * Уведомление о готовности к установке обновлений.
 *
 * \todo SCHAT_NO_UPDATE
 */
void TrayIcon::updateReady()
{
  m_message = UpdateReady;
  QString version = m_settings->getString("Updates/LastVersion");
  showMessage(
      tr("Доступно обновление до версии %1").arg(version),
      tr("Щёлкните здесь для того чтобы установить это обновление прямо сейчас."),
      QSystemTrayIcon::Information,
      60000);
}
