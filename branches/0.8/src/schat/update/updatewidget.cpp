/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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
#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>

#include "update/updatewidget.h"
#include "settings.h"

/*!
 * \brief Конструктор класса UpdateWidget.
 */
UpdateWidget::UpdateWidget(QWidget *parent)
  : QWidget(parent)
{
  m_settings = SimpleSettings;
  m_movie = new QLabel(this);
  m_movie->setMovie(new QMovie(":/images/load.gif", QByteArray(), this));

  m_icon = new QLabel(this);
  m_icon->setVisible(false);
  m_text = new QLabel(this);

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(m_movie);
  mainLay->addWidget(m_icon);
  mainLay->addWidget(m_text);
  mainLay->addStretch();
  mainLay->setMargin(0);

  connect(m_text, SIGNAL(linkActivated(const QString &)), SLOT(linkActivated(const QString &)));
  connect(m_settings, SIGNAL(changed(int)), SLOT(notify(int)));
}


/*!
 * Запуск проверки обновлений.
 */
void UpdateWidget::start()
{
  if (m_settings->getBool("Updates/Enable")) {
    m_movie->setVisible(true);
    m_movie->movie()->start();
    m_icon->setVisible(false);
    m_text->setText(tr("Check for updates..."));
    #ifdef SCHAT_NO_UPDATE
      m_settings->updatesCheck();
    #else
      if (!m_settings->updatesCheck())
        if (m_settings->updateState() == Update::GettingUpdates)
          notify(Settings::UpdateGetting);
    #endif
  }
  else {
    m_text->setText(QString("%1, <a href='ue' style='text-decoration:none; color:#1a4d82;'>%2</a>").arg(tr("Check for updates is disabled")).arg(tr("enable?")));
  }
}


/*!
 * Обработка щелчка по ссылке.
 */
void UpdateWidget::linkActivated(const QString &link)
{
  if (link == "ue") {
    m_settings->setBool("Updates/Enable", true);
    start();
  }
}


/*!
 * Обработка уведомлений о состоянии проверки обновлений.
 */
void UpdateWidget::notify(int code)
{
  switch (code) {
    case Settings::UpdateError:
      m_text->setText(tr("Check for updates failed"));
      setIcon("<img src=':/images/warning.png' />");
      break;

    case Settings::UpdateNoAvailable:
      m_text->setText(tr("Version <b>%1</b> is the latest").arg(QApplication::applicationVersion()));
      setIcon("<img src=':/images/ok2.png' />");
      break;

    case Settings::UpdateAvailable:
    #ifndef SCHAT_NO_UPDATE
    case Settings::UpdateGetting:
    case Settings::UpdateReady:
    #endif
      m_text->setText(tr("New version <b>%1</b> is available").arg(m_settings->getString("Updates/LastVersion")));
      setIcon("<img src=':/images/update.png' />");

      if (code == Settings::UpdateAvailable)
        m_settings->notify(Settings::UpdateAvailableForce);
      break;

    default:
      break;
  }
}


/*!
 * Останавливает и скрывает анимацию и устанавливает текст \a m_icon.
 *
 * \param icon текст, содержащий html код для вставки картинки.
 */
void UpdateWidget::setIcon(const QString &icon)
{
  m_movie->movie()->stop();
  m_movie->setVisible(false);
  m_icon->setVisible(true);
  m_icon->setText(icon);
}
