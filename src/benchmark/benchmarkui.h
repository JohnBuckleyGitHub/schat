/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#ifndef BENCHMARKUI_H_
#define BENCHMARKUI_H_

#include <QDialog>

class QGroupBox;
class QLabel;
class QPushButton;

class BenchmarkUi : public QDialog
{
  Q_OBJECT

public:
  BenchmarkUi(QWidget *parent = 0);

signals:
  void start();
  void stop();

public slots:
  void rejected(int count);
  void started(int count);

private slots:
  void start(bool checked);
  void stop(bool checked);

private:
  QGroupBox *m_statistics;
  QLabel *m_rejected;
  QLabel *m_started;
  QPushButton *m_start;
  QPushButton *m_stop;
};

#endif /* BENCHMARKUI_H_ */
