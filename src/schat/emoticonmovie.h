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

#ifndef EMOTICONMOVIE_H_
#define EMOTICONMOVIE_H_

#include <QMovie>

class EmoticonMovie : public QMovie
{
  Q_OBJECT

public:
  EmoticonMovie(const QString &filename, int pos = -1, QWidget *parent = 0);
  inline QList<int> positions() const { return m_positions; }
  inline QString key() const          { return m_key; }
  inline void addPos(int pos)         { if (pos >= 0) m_positions << pos; }

signals:
  void frameChanged(const QString &key);

public slots:
  void pauseIfHidden(int min, int max);

private slots:
  void next();

private:
  QString m_key;
  QList<int> m_positions;
};

#endif /*EMOTICONMOVIE_H_*/
