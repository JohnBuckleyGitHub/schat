/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include <QtGui>

#include "abstractprofile.h"
#include "protocol.h"
#include "settings.h"
#include "trayicon.h"

/*!
 * \brief Конструктор класса TrayIcon.
 */
TrayIcon::TrayIcon(QObject *parent)
  : QSystemTrayIcon(parent)
{
  setStatus(schat::StatusNormal);
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
    playSound();
  }
  else {
    m_timer->stop();
    setIcon(m_icon);
  }

  m_normal = !enable;
}


/*!
 * При необходимости добавляет звук в очередь.
 */
void TrayIcon::playSound(const QString &key)
{
  if (m_settings->getBool("Sound/" + key + "Enable") && !m_soundQueue.contains(key))
    m_soundQueue.enqueue(key);
}


/*!
 * Обработка щелчка мыши по сообщению в трее.
 */
void TrayIcon::messageClicked()
{
  if (m_message == TrayIcon::UpdateAvailable)
    #ifndef SCHAT_NO_UPDATE
      m_settings->updatesGet();
    #else
      QDesktopServices::openUrl(QUrl("http://impomezia.com"));
    #endif
}


void TrayIcon::notify(int code)
{
  switch (code) {
    case Settings::UpdateAvailable:
      updateAvailable();
      break;

    case Settings::UpdateAvailableForce:
      updateAvailable(true);
      break;

    #ifndef SCHAT_NO_UPDATE
    case Settings::UpdateReady:
      updateReady();
      break;
    #endif

    default:
      break;
  }
}


/*!
 * Обработка изменения статуса пользователя.
 *
 * \param status Новый статус.
 */
void TrayIcon::setStatus(quint32 status)
{
  if (status == schat::StatusAway || status == schat::StatusAutoAway)
    m_icon = QIcon(":/images/logo16-away.png");
  else if (status == schat::StatusDnD)
    m_icon = QIcon(":/images/logo16-dnd.png");
  else if (Settings::isNewYear())
    m_icon = QIcon(":/images/logo16-ny.png");
  else
    m_icon = QIcon(":/images/logo16.png");

  setIcon(m_icon);
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

  playSound();

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
  m_timer->setInterval(700);
  m_soundsPath = QApplication::applicationDirPath() + "/sounds/";
  connect(m_timer, SIGNAL(timeout()), SLOT(timeout()));
  connect(m_settings, SIGNAL(changed(int)), SLOT(notify(int)));
  connect(this, SIGNAL(messageClicked()), SLOT(messageClicked()));
  connect(m_settings->profile(), SIGNAL(statusChanged(quint32)), SLOT(setStatus(quint32)));
}


/*!
 * Воспроизводит звуки из очереди \a m_soundQueue.
 */
void TrayIcon::playSound()
{
  while (!m_soundQueue.isEmpty()) {
    QString key = m_soundQueue.dequeue();
    QString file = m_soundsPath + m_settings->getString("Sound/" + key);

    #ifdef Q_WS_X11
    if (m_settings->getBool("Sound/UseExternalCmd") && !m_settings->getString("Sound/ExternalCmd").isEmpty())
      QProcess::startDetached(m_settings->getString("Sound/ExternalCmd").arg(file));
    else
    #endif
      QSound::play(file);
  }
}


/*!
 * Уведомление о доступности новой версии.
 */
void TrayIcon::updateAvailable(bool force)
{
  m_message = UpdateAvailable;
  static QString last;

  QString version = m_settings->getString("Updates/LastVersion");

  if (last.isEmpty() || last != version || force) {

    #ifndef SCHAT_NO_UPDATE
    showMessage(
        tr("Доступно обновление до версии %1").arg(version),
        tr("Щёлкните здесь для того чтобы скачать это обновление прямо сейчас.\n"
           "Размер файлов: %1").arg(bytesToHuman(m_settings->getInt("Updates/DownloadSize"))),
        QSystemTrayIcon::Information,
        60000);
    #else
    showMessage(
            tr("Доступна новая версия %1").arg(version),
            tr("Щёлкните здесь для того чтобы перейти на страницу загрузки"),
            QSystemTrayIcon::Information,
            60000);
    #endif

    last = version;
  }
}


/*!
 * Возвращает строку, содержащую удобный для человека размер файлов.
 */
#ifndef SCHAT_NO_UPDATE
QString TrayIcon::bytesToHuman(int size)
{
  if (size < 1024)
    return tr("%n Байт", "", size);
  else if (size < 1048576)
    return tr("%1 Кб").arg((int) size / 1024);
  else
    return tr("%1 Мб").arg((double) size / 1048576, 0, 'f', 2);
}
#endif


/*!
 * Уведомление о готовности к установке обновлений.
 */
#ifndef SCHAT_NO_UPDATE
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
#endif
