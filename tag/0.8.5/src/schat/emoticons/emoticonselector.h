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

#ifndef EMOTICONSELECTOR_H_
#define EMOTICONSELECTOR_H_

#include <QLabel>
#include <QMovie>
#include <QObject>
#include <QWidget>

class QGridLayout;
class QHideEvent;
class QShowEvent;

/*!
 * \brief Отображает один смайлик.
 */
class EmoticonLabel : public QLabel
{
  Q_OBJECT

public:
  EmoticonLabel(const QString &text, const QString &file, QWidget *parent = 0);

signals:
  void clicked(const QString &text);

private:
  inline void mousePressEvent(QMouseEvent* /*event*/) { m_ok = true; }
  void mouseReleaseEvent(QMouseEvent *event);

  bool m_ok;
};


/*!
 * \brief Виджет для выбора смайликов.
 */
class EmoticonSelector : public QFrame
{
  Q_OBJECT

public:
  EmoticonSelector(QWidget *parent = 0);

signals:
  void deleteLabels();
  void itemSelected(const QString &str);

public slots:
  inline void aboutToHide() { emit deleteLabels(); }
  void prepareList();

protected slots:
  void emoticonClicked(const QString &);

private:
  QGridLayout *m_lay;
};

#endif /*EMOTICONSELECTOR_H_*/
