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

#include "update/updatewidget.h"
#include "settings.h"

/*!
 * \brief Конструктор класса UpdateWidget.
 */
UpdateWidget::UpdateWidget(QWidget *parent)
  : QWidget(parent)
{
  m_settings = settings;
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


void UpdateWidget::start()
{
  if (m_settings->getBool("Updates/Enable")) {
    m_movie->setVisible(true);
    m_movie->movie()->start();
    m_icon->setVisible(false);
    m_text->setText(tr("Проверка обновлений..."));
    if (!m_settings->updatesCheck())
      if (m_settings->updateState() == Update::GettingUpdates)
        notify(Settings::UpdateGetting);
  }
  else {
    m_text->setText(tr("Проверка обновлений отключена, <a href='ue' style='text-decoration:none; color:#1a4d82;'>включить?</a>"));
  }
}


void UpdateWidget::linkActivated(const QString &link)
{
  if (link == "ue") {
    m_settings->setBool("Updates/Enable", true);
    start();
  }
}


void UpdateWidget::notify(int code)
{
  switch (code) {
    case Settings::UpdateError:
      m_text->setText(tr("Не удалось проверить обновления"));
      setIcon("<img src=':/images/warning.png' />");
      break;

    case Settings::UpdateNoAvailable:
      m_text->setText(tr("Версия <b>%1</b> самая свежая").arg(QApplication::applicationVersion()));
      setIcon("<img src=':/images/ok2.png' />");
      break;

    case Settings::UpdateAvailable:
    case Settings::UpdateGetting:
    case Settings::UpdateReady:
      m_text->setText(tr("Доступна новая версия <b>%1</b>").arg(m_settings->getString("Updates/LastVersion")));
      setIcon("<img src=':/images/update.png' />");

      if (code == Settings::UpdateAvailable)
        m_settings->notify(Settings::UpdateAvailableForce);
      break;

    default:
      break;
  }
}


void UpdateWidget::setIcon(const QString &icon)
{
  m_movie->movie()->stop();
  m_movie->setVisible(false);
  m_icon->setVisible(true);
  m_icon->setText(icon);
}
