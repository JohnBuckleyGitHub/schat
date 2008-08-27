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

signals:
  void clicked(const QString &text);

private:
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  QString m_text;
  bool m_ok;
};


class EmoticonSelector : public QWidget
{
  Q_OBJECT

public:
  EmoticonSelector(Settings *settings, QWidget *parent = 0);
//  ~EmoticonSelector();

  typedef QList<QMovie*> MovieList;
signals:
  void itemSelected(const QString &str);

public slots:
  void prepareList();

protected:
  virtual void hideEvent(QHideEvent*);
  virtual void showEvent(QShowEvent*);

protected slots:
  void emoticonClicked(const QString &);

private:
  MovieList m_movieList;
  QGridLayout *m_lay;
  Settings *m_settings;
};

#endif /*EMOTICONSELECTOR_H_*/
