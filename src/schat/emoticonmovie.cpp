/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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

#include "emoticonmovie.h"

/*!
 * \class EmoticonMovie
 * \brief Анимированный смайлик.
 */

/*!
 * \brief Конструктор класса EmoticonMovie.
 */
EmoticonMovie::EmoticonMovie(const QString &filename, int pos, int starts, QWidget *parent)
  : QMovie(parent)
{
  setFileName(filename);
  m_key = QFileInfo(filename).fileName();
  addPos(pos);
  addStarts(starts);
  start();
  connect(this, SIGNAL(frameChanged(int)), SLOT(next()));
}


void EmoticonMovie::addStarts(int starts)
{
  if (starts >= 0)
    if (!m_starts.contains(starts))
      m_starts << starts;
}


void EmoticonMovie::pauseIfHidden(int min, int max)
{
  bool pause = true;

  foreach (int pos, m_positions) {
    if (pos >= min && pos <= max) {
      pause = false;
      break;
    }
  }

  if (pause && state() == QMovie::Running)
    setPaused(true);
  else if (!pause && state() == QMovie::Paused)
    setPaused(false);
}


void EmoticonMovie::next()
{
  emit frameChanged(m_key); 
}
