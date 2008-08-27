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

//#include <kdebug.h>
//#include <kemoticons.h>

EmoticonItem::EmoticonItem(const QString &emoticonText, const QString &pixmapPath, QListWidget *parent)
  : QListWidgetItem(parent)
{
  m_text = emoticonText;
  m_pixmapPath = pixmapPath;
  QPixmap p(m_pixmapPath);
    //
    // Some of the custom icons are rather large
    // so lets limit them to a maximum size for this display panel
    //
    if (p.width() > 32 || p.height() > 32)
    p = p.scaled(QSize(32,32), Qt::KeepAspectRatio);

  setIcon(p);
}

QString EmoticonItem::text() const
{
  return m_text;
}

QString EmoticonItem::pixmapPath() const
{
  return m_pixmapPath;
}

EmoticonSelector::EmoticonSelector(Settings *settings, QWidget *parent)
  : QWidget(parent), m_settings(settings)
{
  QHBoxLayout *lay = new QHBoxLayout(this);
  lay->setSpacing( 0 );
  lay->setContentsMargins( 0, 0, 0, 0 );
  m_emoticonList = new QListWidget(this);
  lay->addWidget(m_emoticonList);
  m_emoticonList->setViewMode(QListView::IconMode);
  m_emoticonList->setSelectionMode(QAbstractItemView::SingleSelection);
  m_emoticonList->setMouseTracking(true);
  m_emoticonList->setDragEnabled(false);

  QLabel *m_currentEmoticon  = new QLabel( this );
  m_currentEmoticon->setFrameShape( QFrame::Box );
  m_currentEmoticon->setMinimumSize(QSize(128,128));
  m_currentEmoticon->setAlignment( Qt::AlignCenter );
  lay->addWidget(m_currentEmoticon);

  m_currentMovie = new QMovie(this);
  m_currentEmoticon->setMovie(m_currentMovie);

  connect(m_emoticonList, SIGNAL(itemEntered(QListWidgetItem*)),
      this, SLOT(mouseOverItem(QListWidgetItem*)));
  connect(m_emoticonList, SIGNAL(itemSelectionChanged()),
      this, SLOT(currentChanged()));
  connect(m_emoticonList, SIGNAL(itemActivated(QListWidgetItem*)),
      this, SLOT(emoticonClicked(QListWidgetItem*)));

}

void EmoticonSelector::prepareList(void)
{
  m_emoticonList->clear();
//  kDebug(14000) << "called.";
//  QHash<QString, QStringList> list = Kopete::Emoticons::self()->theme().emoticonsMap();

//  for (QHash<QString, QStringList>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it )
    new EmoticonItem(":)", qApp->applicationDirPath() + "/emoticons/" + m_settings->getString("EmoticonTheme") + "/angel.gif", m_emoticonList);
    new EmoticonItem(";-)", qApp->applicationDirPath() + "/emoticons/" + m_settings->getString("EmoticonTheme") + "/wink.gif", m_emoticonList);

  m_emoticonList->setIconSize(QSize(32,32));
}

void EmoticonSelector::emoticonClicked(QListWidgetItem *i)
{
  EmoticonItem *item = dynamic_cast<EmoticonItem*>(i);
  if (!item)
    return;

  // KDE4/Qt TODO: use qobject_cast instead.
  emit itemSelected ( item->text() );
  if ( isVisible() && parentWidget() &&
    parentWidget()->inherits("QMenu") )
  {
    parentWidget()->close();
  }
}

void EmoticonSelector::mouseOverItem(QListWidgetItem *item)
{
  item->setSelected(true);  
  if (!m_emoticonList->hasFocus())
    m_emoticonList->setFocus();
}

void EmoticonSelector::currentChanged()
{

  if (!m_emoticonList->selectedItems().count())
    return;

  EmoticonItem *item = dynamic_cast<EmoticonItem*>(m_emoticonList->selectedItems().first());
  if (!item)
    return;

  // FIXME: the label is not correctly cleared when changing from a bigger movie to a smaller one
  m_currentMovie->stop();
  m_currentMovie->setFileName(item->pixmapPath());
  m_currentMovie->start();
}

void EmoticonSelector::hideEvent( QHideEvent* )
{
  m_currentMovie->stop();
}

void EmoticonSelector::showEvent( QShowEvent* )
{
  m_currentMovie->start();
}
