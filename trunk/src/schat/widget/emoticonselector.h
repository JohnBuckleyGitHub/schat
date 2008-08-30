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
#include <QMovie>
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

public slots:
  inline void setPaused(bool paused) { m_movie->setPaused(paused); }

private:
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

  bool m_ok;
  QMovie *m_movie;
  QString m_text;
};


class EmoticonSelector : public QWidget
{
  Q_OBJECT

public:
  EmoticonSelector(Settings *settings, QWidget *parent = 0);

signals:
  void deleteLabels();
  void itemSelected(const QString &str);
  void setPaused(bool paused);

public slots:
  void prepareList();
  void aboutToHide() { emit deleteLabels(); }

protected:
//  virtual void hideEvent(QHideEvent*);
  void showEvent(QShowEvent* /*event*/) { emit setPaused(false); }

protected slots:
  void emoticonClicked(const QString &);

private:  
  QList<QMovie*> m_movieList;
  QGridLayout *m_lay;
  Settings *m_settings;
};

#endif /*EMOTICONSELECTOR_H_*/
