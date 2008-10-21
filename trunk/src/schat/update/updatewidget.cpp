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
  m_movie = new QMovie(":/images/load.gif", QByteArray(), this);
  QLabel *movie = new QLabel(this);
  movie->setMovie(m_movie);

  m_text = new QLabel(tr("Проверка обновлений..."), this);

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(movie);
  mainLay->addWidget(m_text);
  mainLay->addStretch();
  mainLay->setMargin(0);
}


void UpdateWidget::start()
{
  m_movie->start();
}
