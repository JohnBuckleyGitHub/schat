/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
 *
 *   a button that pops up a list of all emoticons and returns
 *   the emoticon-string if one is selected in the list
 *   Copyright (c) 2002 by Stefan Gehn                 <metz AT gehn.net>
 *   Kopete    (c) 2002-2003 by the Kopete developers  <kopete-devel@kde.org>
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

#include "emoticonselector.h"
#include "settings.h"

#include <math.h>

#include <QtGui>

/*!
 * \brief Конструктор класса EmoticonLabel.
 *
 * \param text   Текстовый эквивалент смайлика.
 * \param file   Файл смайлика.
 * \param parent Указатель на родительский виджет.
 *
 * \todo Должна поддерживаться возможность выбрать смайлик с клавиатуры.
 */
EmoticonLabel::EmoticonLabel(const QString &text, const QString &file, QWidget *parent)
  : QLabel(parent),
    m_ok(false)
{
  QMovie *movie = new QMovie(file, QByteArray(), this);
  setMovie(movie);
  setAlignment(Qt::AlignCenter);
  movie->start();
  setToolTip(text);

  #ifdef Q_WS_X11
  setStyleSheet("EmoticonLabel {background: #ffffff; border: 1px solid #eeeeee;}");
  #else
  setStyleSheet("EmoticonLabel {background: #ffffff}");
  #endif
}


void EmoticonLabel::mouseReleaseEvent(QMouseEvent* /*event*/)
{
  if (m_ok)
    emit clicked(" " + toolTip() + " ");
}


/*!
 * \brief Конструктор класса EmoticonSelector.
 */
EmoticonSelector::EmoticonSelector(QWidget *parent)
  : QFrame(parent),
    m_lay(0)
{
  #ifndef Q_WS_X11
  setStyleSheet("EmoticonSelector {background: #eeeeee}");
  #endif
}


void EmoticonSelector::prepareList()
{
  int row = 0;
  int col = 0;

  EmoticonsTheme theme = SimpleSettings->emoticons()->theme();
  QMap<QString, QStringList> list = theme.emoticonsMap();
  int emoticonsPerRow = (int) sqrt((float)list.count());
  if (emoticonsPerRow * emoticonsPerRow == list.count())
    emoticonsPerRow--;

  if (m_lay)
    delete m_lay;

  QString path = theme.themePath() + "/";

  m_lay = new QGridLayout(this);
  #ifdef Q_WS_X11
  m_lay->setMargin(0);
  m_lay->setSpacing(0);
  #else
  m_lay->setMargin(2);
  m_lay->setSpacing(1);
  #endif

  QMapIterator<QString, QStringList> i(list);
  while (i.hasNext()) {
    i.next();
    EmoticonLabel *label = new EmoticonLabel(i.value().first(), path + i.key(), this);
    connect(label, SIGNAL(clicked(const QString &)), SLOT(emoticonClicked(const QString &)));
    connect(this, SIGNAL(deleteLabels()), label, SLOT(deleteLater()));
    m_lay->addWidget(label, row, col);
    if (col == emoticonsPerRow) {
      col = 0;
      row++;
    }
    else
      col++;
  }
}


void EmoticonSelector::emoticonClicked(const QString &str)
{
  emit itemSelected(str);

  QMenu *popup = qobject_cast<QMenu *>(parentWidget());
  if (isVisible() && popup) {
    popup->close();
  }
}
