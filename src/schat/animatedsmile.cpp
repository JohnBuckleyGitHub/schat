/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 * 
 * class AnimatedSmile
 * Copyright © 2007 by Anistratov Oleg <ower86@gmail.com>
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

#include <QUrl>
#include <QFile>
#include <QCryptographicHash>

#include "animatedsmile.h"

/*!
 * \class AnimatedSmile
 * \brief Класс служит для поддрежки анимированных смайлов.
 * 
 * \author Anistratov Oleg <ower86@gmail.com>
 */

QMap<QByteArray, QMovie*> AnimatedSmile::m_allSmiles;

/*!
 * \brief Конструктор класса AnimatedSmile.
 */
AnimatedSmile::AnimatedSmile(QObject *parent)
 : QObject(parent)
{
  m_smile    = 0;
  m_document = 0;
  m_running  = false;
}


AnimatedSmile::~AnimatedSmile()
{
  disconnect(m_smile, SIGNAL(frameChanged(int)), this, SLOT(nextFrame()));
}


void AnimatedSmile::init(int pos, const QString& smile, QTextDocument* doc)
{
  Q_ASSERT(!m_cursor && !m_smile);

  QFile file(smile);
  QCryptographicHash hash(QCryptographicHash::Md5);
  QByteArray result;

  if(!file.open(QIODevice::ReadOnly))
    return;

  hash.addData(file.readAll());

  result = hash.result();

  m_smile = m_allSmiles[result];

  if (!m_smile) {
    m_smile = new QMovie(smile);
    m_allSmiles.insert(result, m_smile);
  }

  m_filename = smile;
  m_document = doc;
  m_pos      = pos;

  connect(m_smile, SIGNAL(frameChanged(int)), this, SLOT(nextFrame()));
}


void AnimatedSmile::pauseIfHidden(int min, int max)
{
  setPaused(!(m_pos >= min && m_pos <= max));
}


void AnimatedSmile::nextFrame()
{
  if (m_running && m_smile && m_document) {
    QTextCursor cursor(m_document);

    m_document->addResource(QTextDocument::ImageResource, QUrl(m_filename), m_smile->currentImage());

    cursor.setPosition(m_pos);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

    cursor.insertImage(m_filename);
  }
}
