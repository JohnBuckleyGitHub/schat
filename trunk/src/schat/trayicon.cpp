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
 * Возвращает строку, содержащую удобный для человека размер файлов.
 */
QString TrayIcon::bytesToHuman(int size)
{
  if (size < 1024)
    return tr("%n Байт", "", size);
  else if (size < 1048576)
    return tr("%1 Кб").arg((int) size / 1024);
  else
    return tr("%1 Мб").arg((double) size / 1048576, 0, 'f', 2);
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


/*!
 * Обработка щелчка мыши по сообщению в трее.
 */
void TrayIcon::messageClicked()
{
  if (m_message == TrayIcon::UpdateAvailable)
    m_settings->updatesGet();
}


void TrayIcon::notify(int code)
{
  switch (code) {
    case Settings::UpdateReady:
      updateReady();
      break;

    case Settings::UpdateAvailable:
      updateAvailable();
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
  connect(this, SIGNAL(messageClicked()), SLOT(messageClicked()));
}


/*!
 * Уведомление о доступности новой версии.
 *
 * \todo SCHAT_NO_UPDATE
 */
void TrayIcon::updateAvailable()
{
  m_message = UpdateAvailable;
  static QString last;

  QString version = m_settings->getString("Updates/LastVersion");

  if (last.isEmpty() || last != version) {
    showMessage(
        tr("Доступно обновление до версии %1").arg(version),
        tr("Щёлкните здесь для того чтобы скачать это обновление прямо сейчас.\n"
           "Размер файлов: %1").arg(bytesToHuman(m_settings->getInt("Updates/DownloadSize"))),
        QSystemTrayIcon::Information,
        60000);

    last = version;
  }
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
      tr("Всё готово к установке версии %1").arg(version),
      tr("Щёлкните здесь для того чтобы установить это обновление прямо сейчас."),
      QSystemTrayIcon::Information,
      60000);
}
