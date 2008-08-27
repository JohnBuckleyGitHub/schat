/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 * 
 *  a button that pops up a list of all emoticons and returns
 *  the emoticon-string if one is selected in the list
 *
 *   Copyright (c) 2002 by Stefan Gehn            <metz AT gehn.net>
 *   Kopete    (c) 2002-2003 by the Kopete developers  <kopete-devel@kde.org>
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

#include "emoticonselector.h"
#include "settings.h"

#include <math.h>

#include <QtGui>

/*!
 * \class EmoticonLabel
 * \brief Отображает один смайлик.
 */

/*!
 * \brief Конструктор класса EmoticonLabel.
 */
EmoticonLabel::EmoticonLabel(const QString &emoticonText, const QString &pixmapPath, QWidget *parent)
  : QLabel(parent)
{
  m_ok = false;
  m_text = emoticonText;
  setMovie(new QMovie(pixmapPath, QByteArray(), this));
  setAlignment(Qt::AlignCenter);
  QPixmap p(pixmapPath);
  if (p.width() > 32 || p.height() > 32)
    p.scaled(32, 32);
  setMinimumSize(p.size());
}


void EmoticonLabel::mousePressEvent(QMouseEvent*)
{
  m_ok = true;
}

void EmoticonLabel::mouseReleaseEvent(QMouseEvent*)
{
  if (m_ok)
    emit clicked(m_text);
}


/*!
 * \class EmoticonSelector
 * \brief Виджет для выбора смайликов.
 */

/*!
 * \brief Конструктор класса EmoticonSelector.
 */
EmoticonSelector::EmoticonSelector(Settings *settings, QWidget *parent)
  : QWidget(parent), m_settings(settings)
{
  m_lay = 0;
}


void EmoticonSelector::prepareList()
{
  int row = 0;
  int col = 0;
  QHash<QString, QStringList> list = m_settings->emoticons();
  int emoticonsPerRow = static_cast<int>(sqrt(list.count()));

  if (m_lay) {
    QList<EmoticonLabel *> labels = this->findChildren<EmoticonLabel *>();
    if (!labels.isEmpty()) {
      foreach (EmoticonLabel *label, labels)
        delete label;
      labels.clear();
    }
    delete m_lay;
  }
  QString emoticonsPath = qApp->applicationDirPath() + "/emoticons/" + m_settings->getString("EmoticonTheme");

  m_lay = new QGridLayout(this);
  m_lay->setMargin(2);
  m_lay->setSpacing(2);
  m_movieList.clear();

  QHashIterator<QString, QStringList> i(list);
  while (i.hasNext()) {
    i.next();
    QWidget *w = new EmoticonLabel(i.value().first(), emoticonsPath + "/" + i.key(), this);
    m_movieList.push_back( ((QLabel*)w)->movie() );
    connect(w, SIGNAL(clicked(const QString &)), SLOT(emoticonClicked(const QString &)));
    m_lay->addWidget(w, row, col);
    if ( col == emoticonsPerRow ) {
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
  if (isVisible() && popup)
    popup->close();
}


void EmoticonSelector::hideEvent( QHideEvent* )
{
  foreach (QMovie *movie, m_movieList)
    movie->setPaused(true);
}


void EmoticonSelector::showEvent( QShowEvent* )
{
  foreach (QMovie *movie, m_movieList)
    movie->setPaused(false);
}
