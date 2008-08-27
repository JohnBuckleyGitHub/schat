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

#ifndef EMOTICONSELECTOR_H_
#define EMOTICONSELECTOR_H_

#include <QLabel>
#include <QWidget>

class QGridLayout;
class QHideEvent;
class QShowEvent;
class Settings;

class EmoticonLabel : public QLabel
{
  Q_OBJECT

public:
  EmoticonLabel(const QString &emoticonText, const QString &pixmapPath, QWidget *parent = 0);
  ~EmoticonLabel();

signals:
  void clicked(const QString &text);

protected:
  void mouseReleaseEvent(QMouseEvent*);
  QString mText;
};

class EmoticonSelector : public QWidget
{
  Q_OBJECT

public:

  EmoticonSelector(Settings *settings, QWidget *parent = 0);
//  ~EmoticonSelector();

  typedef QList<QMovie*> MovieList;
signals:
  /**
  * gets emitted when an emoticon has been selected from the list
  * the QString holds the emoticon as a string or is 0L if nothing was selected
  **/
  void ItemSelected(const QString &);

public slots:
  void prepareList();

protected:
  virtual void hideEvent( QHideEvent* );
  virtual void showEvent( QShowEvent* );
  MovieList m_movieList;
  QGridLayout *m_lay;

protected slots:
  void emoticonClicked(const QString &);
  
private:
  Settings *m_settings;
};

#endif /*EMOTICONSELECTOR_H_*/
