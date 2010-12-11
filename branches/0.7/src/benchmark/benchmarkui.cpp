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

#include <QApplication>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>

#include "benchmarkui.h"

/*!
 * \brief Конструктор класса AboutDialog.
 */
BenchmarkUi::BenchmarkUi(QWidget *parent)
  : QDialog(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

  setWindowTitle(QApplication::applicationName() + " " + QApplication::applicationVersion());

  m_start = new QPushButton(tr("&Start"), this);
  m_start->setDefault(true);
  connect(m_start, SIGNAL(clicked(bool)), this, SIGNAL(start()));

  m_stop = new QPushButton(tr("S&top"), this);
  connect(m_stop, SIGNAL(clicked(bool)), this, SIGNAL(stop()));

  m_statistics = new QGroupBox(tr("Statistics"), this);
  m_started = new QLabel("0", this);

  QHBoxLayout *controlLay = new QHBoxLayout();
  controlLay->addWidget(m_start);
  controlLay->addWidget(m_stop);

  QFormLayout *statisticsLay = new QFormLayout(m_statistics);
  statisticsLay->addRow("Started:", m_started);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addLayout(controlLay);
  mainLay->addWidget(m_statistics);
}


void BenchmarkUi::started(int count)
{
  m_started->setText(QString::number(count));
}
