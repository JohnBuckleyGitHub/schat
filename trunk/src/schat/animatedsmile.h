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

#ifndef ANIMATEDSMILE_H_
#define ANIMATEDSMILE_H_

#include <QMovie>
#include <QObject>
#include <QTextCursor>
#include <QTextDocument>

class AnimatedSmile : public QObject
{
  Q_OBJECT

public:
  AnimatedSmile(QObject *parent = 0);
  ~AnimatedSmile();
  inline bool animated()        {return true;}
  inline void setPaused(bool b) { m_running = !b; }
  inline void start()           { m_smile->start(); m_running = true; nextFrame(); }
  inline void stop()            { m_smile->stop(); m_running = false; }
  void init(int pos, const QString &smile, QTextDocument *doc);
  void pauseIfHidden(int min, int max);

public slots:
  void nextFrame();
    
private:
  bool m_running;
  int m_pos;
  QMovie *m_smile;
  QString m_filename;
  QTextDocument *m_document;
  static QMap<QByteArray, QMovie*> m_allSmiles;
};

#endif /*ANIMATEDSMILE_H_*/
