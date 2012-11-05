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
#include <QDesktopServices>
#include <QFile>
#include <QProcess>
#include <QSound>
#include <QTimer>

#include "abstractprofile.h"
#include "protocol.h"
#include "settings.h"
#include "trayicon.h"

/*!
 * \brief Конструктор класса TrayIcon.
 */
TrayIcon::TrayIcon(QObject *parent)
  : QSystemTrayIcon(parent),
  m_deferredMessage(false),
  m_normal(true),
  m_lastCheckedVersion(SimpleSettings->getString("Updates/LastVersion")),
  m_settings(SimpleSettings)
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
void TrayIcon::playSound(const QString &key, bool force)
{
  if (m_settings->getBool("Sound/" + key + "Enable") && !m_soundQueue.contains(key))
    m_soundQueue.enqueue(key);

  if (force)
    playSound();
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
      QDesktopServices::openUrl(QUrl("http://impomezia.ru"));
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
      displayMessage(UpdateReady);
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
    m_icon = QIcon(":/images/schat16-away.png");
  else if (status == schat::StatusDnD)
    m_icon = QIcon(":/images/schat16-dnd.png");
  else if (Settings::isNewYear())
    m_icon = QIcon(":/images/schat16-ny.png");
  else
    m_icon = QIcon(":/images/schat16.png");

  if (status != schat::StatusAutoAway && status != schat::StatusDnD && m_deferredMessage) {
    displayMessage(m_message);
  }

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
 * Отображение сообщения заданного типа.
 */
void TrayIcon::displayMessage(Message message, bool force)
{
  QString version = m_settings->getString("Updates/LastVersion");
  m_message = message;

  if (!force) {
    if (SimpleSettings->profile()->status() == schat::StatusAutoAway || SimpleSettings->profile()->status() == schat::StatusDnD) {
      m_deferredMessage = true;
      return;
    }
  }

  m_deferredMessage = false;

  if (message == UpdateAvailable) {
    #ifndef SCHAT_NO_UPDATE
    showMessage(
      tr("Update to version %1 is available").arg(version),
      tr("Click here to download update right now.\n"
         "File size: %1").arg(bytesToHuman(m_settings->getInt("Updates/DownloadSize"))),
      QSystemTrayIcon::Information,
      60000);
    #else
    showMessage(
          tr("A new version %1 is available").arg(version),
          tr("Click here to go to a download page"),
          QSystemTrayIcon::Information,
          60000);
    #endif
  }
  #ifndef SCHAT_NO_UPDATE
  else if (message == UpdateReady) {
    showMessage(
        tr("Everything is ready to install version %1").arg(version),
        tr("Click here to install the update right now."),
        QSystemTrayIcon::Information,
        60000);
  }
  #endif
}


/*!
 * Инициализация членов класса.
 */
void TrayIcon::init()
{
  setToolTip(QApplication::applicationName() + " " + QApplication::applicationVersion());
  m_noticeIcon = QIcon(":/images/balloon.png");
  m_timer = new QTimer(this);
  m_timer->setInterval(700);
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
    QString file = "/" + m_settings->getString("Sound/" + key);

    if (!m_soundCache.contains(key)) {
      QStringList sounds = m_settings->path(Settings::SoundsPath);
      for (int i = 0; i < sounds.size(); ++i) {
        if (QFile::exists(sounds.at(i) + file)) {
          m_soundCache.insert(key, sounds.at(i) + file);
          break;
        }
      }
    }

    if (!m_soundCache.contains(key))
      return;

    #ifdef Q_WS_X11
    if (m_settings->getBool("Sound/UseExternalCmd") && !m_settings->getString("Sound/ExternalCmd").isEmpty())
      QProcess::startDetached(m_settings->getString("Sound/ExternalCmd").arg(m_soundCache.value(key)));
    else
    #endif
      QSound::play(m_soundCache.value(key));
  }
}


/*!
 * Уведомление о доступности новой версии.
 */
void TrayIcon::updateAvailable(bool force)
{
  QString version = m_settings->getString("Updates/LastVersion");

  if (m_lastCheckedVersion != version || force) {
    displayMessage(UpdateAvailable, force);
    m_lastCheckedVersion = version;
  }
}


/*!
 * Возвращает строку, содержащую удобный для человека размер файлов.
 */
#ifndef SCHAT_NO_UPDATE
QString TrayIcon::bytesToHuman(int size)
{
  if (size < 1024)
    return tr("%n Byte", "", size);
  else if (size < 1048576)
    return tr("%1 kB").arg((int) size / 1024);
  else
    return tr("%1 MB").arg((double) size / 1048576, 0, 'f', 2);
}
#endif
