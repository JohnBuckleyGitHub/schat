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

EmoticonLabel::EmoticonLabel(const QString &emoticonText, const QString &pixmapPath, QWidget *parent)
  : QLabel(parent)
{
  mText = emoticonText;
  setMovie(new QMovie(pixmapPath) );
  setAlignment(Qt::AlignCenter);
//  QToolTip::add(this,emoticonText);
  // Somehow QLabel doesn't tell a reasonable size when you use setMovie
  // although it does it correctly for setPixmap. Therefore here is a little workaround
  // to tell our minimum size.
  QPixmap p(pixmapPath);
    //
    // Some of the custom icons are rather large
    // so lets limit them to a maximum size for this display panel
    //
    if (p.width() > 32 || p.height() > 32)
        p.scaled(32, 32);
  setMinimumSize(p.size());
}


EmoticonLabel::~EmoticonLabel()
{
  qDebug() << "EmoticonLabel::~EmoticonLabel()";
}

void EmoticonLabel::mouseReleaseEvent(QMouseEvent*)
{
  emit clicked(mText);
}

EmoticonSelector::EmoticonSelector(Settings *settings, QWidget *parent)
  : QWidget(parent), m_settings(settings)
{
//  kdDebug(14000) << k_funcinfo << "called." << endl;
  m_lay = 0;
}

void EmoticonSelector::prepareList(void)
{
//  QLabel *test = new QLabel("ПЫЩ", this);
//  QHBoxLayout *_lay = new QHBoxLayout(this);
//  _lay->addWidget(test);
//  kdDebug(14000) << k_funcinfo << "called." << endl;
  int row = 0;
  int col = 0;
  QHash<QString, QStringList> list;
  list.insert("smile.gif", QStringList() << ":)");
  list.insert("sad.gif", QStringList() << ":-(");
//  QMap<QString, QStringList> list = Kopete::Emoticons::self()->emoticonAndPicList();
  int emoticonsPerRow = static_cast<int>(sqrt(list.count()));
  //kdDebug(14000) << "emoticonsPerRow=" << emoticonsPerRow << endl;

  if (m_lay) {
//    if (!m_movieList.isEmpty()) {
//      qDebug() << "<<<";
////      foreach (QMovie *movie, m_movieList)
////        delete movie;
//      m_movieList.clear();
//    }
    QList<EmoticonLabel *> labels = this->findChildren<EmoticonLabel *>();
    qDebug() << labels;
    if (!labels.isEmpty()) {
      foreach (EmoticonLabel *label, labels)
        delete label;
      labels.clear();
    }
    delete m_lay;
//    QObjectList *objList = queryList( "EmoticonLabel" );
    //kdDebug(14000) << k_funcinfo << "There are " << objList->count() << " EmoticonLabels to delete." << endl;
//    objList->setAutoDelete(true);
//    objList->clear();
//    delete objList;
//    delete lay;
  }
  QString emoticonsPath = qApp->applicationDirPath() + "/emoticons/" + m_settings->getString("EmoticonTheme");

  m_lay = new QGridLayout(this);
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
  
//  for (QMap<QString, QStringList>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it )
//  {
//    QWidget *w = new EmoticonLabel(it.data().first(), it.key(), this);
//    movieList.push_back( ((QLabel*)w)->movie() );
//    connect(w, SIGNAL(clicked(const QString&)), this, SLOT(emoticonClicked(const QString&)));
////    kdDebug(14000) << "adding Emoticon to row=" << row << ", col=" << col << "." << endl;
//    lay->addWidget(w, row, col);
//    if ( col == emoticonsPerRow )
//    {
//      col = 0;
//      row++;
//    }
//    else
//      col++;
//  }
//  resize(minimumSizeHint());
}

void EmoticonSelector::emoticonClicked(const QString &str)
{
//  kdDebug(14000) << "selected emoticon '" << str << "'" << endl;
  // KDE4/Qt TODO: use qobject_cast instead.
  emit ItemSelected ( str );
  if ( isVisible() && parentWidget() &&
    parentWidget()->inherits("QMenu") )
  {
    parentWidget()->close();
  }
}

void EmoticonSelector::hideEvent( QHideEvent* )
{
  foreach (QMovie *movie, m_movieList)
    movie->setPaused(true);
}


void EmoticonSelector::showEvent( QShowEvent* )
{
//  kdDebug( 14000 ) << k_funcinfo << endl;
  foreach (QMovie *movie, m_movieList)
    movie->setPaused(false);
  
//  MovieList::iterator it;
//  for( it = m_movieList.begin(); it != m_movieList.end(); ++it )
//  {
//    (*it)->setPaused(false);
//  }
}

// vim: set noet ts=4 sts=4 sw=4:


