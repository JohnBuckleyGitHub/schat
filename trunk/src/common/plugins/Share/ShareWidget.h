/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#ifndef SHAREWIDGET_H_
#define SHAREWIDGET_H_

#include <QFrame>

class QLabel;
class QToolButton;

class ShareWidget : public QFrame
{
  Q_OBJECT

public:
  ShareWidget(QWidget *parent = 0);

signals:
  void addFromDisk();
  void addFromWeb();

public slots:
  void close();

private:
  QLabel *m_addLabel;
  QToolButton *m_diskBtn;
  QToolButton *m_webBtn;
};

#endif /* SHAREWIDGET_H_ */
