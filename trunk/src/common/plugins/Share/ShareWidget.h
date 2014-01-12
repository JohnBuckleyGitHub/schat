/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2014 IMPOMEZIA <schat@impomezia.com>
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
#include <QUrl>

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
  void upload(const QList<QUrl> &urls, bool local);

public slots:
  void close();

private slots:
  void onDataChanged();
  void paste();

private:
  QLabel *m_addLabel;
  QToolButton *m_diskBtn;
  QToolButton *m_pasteBtn;
  QToolButton *m_webBtn;
};

#endif /* SHAREWIDGET_H_ */
