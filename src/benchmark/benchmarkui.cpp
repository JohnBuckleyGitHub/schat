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
//
//  m_tabWidget = new QTabWidget(this);
//  m_tabWidget->addTab(new AboutMain(this),      tr("О Программе"));
//  m_tabWidget->addTab(new AboutMembers(this),   tr("Участники"));
//  m_tabWidget->addTab(new AboutChangeLog(this), tr("История версий"));
//  m_tabWidget->addTab(new AboutLicense(this),   tr("Лицензия"));
//
//  m_closeButton = new QPushButton(QIcon(":/images/dialog-ok.png"), tr("Закрыть"), this);
//  m_closeButton->setDefault(true);
//
//  connect(m_closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));
//
//  m_bottom = new QWidget(this);
//  m_bottom->setObjectName("AboutBottom");
//  QHBoxLayout *buttonLay = new QHBoxLayout(m_bottom);
//  buttonLay->setMargin(2);
//  #ifndef SCHAT_NO_UPDATE_WIDGET
//    m_update = new UpdateWidget(this);
//    buttonLay->addWidget(m_update);
//  #endif
//  buttonLay->addStretch();
//  buttonLay->addWidget(m_closeButton);
//
//  m_mainLay = new QVBoxLayout(this);
//  m_mainLay->addWidget(m_tabWidget);
//  m_mainLay->addWidget(m_bottom);
//  setStyleSheet();
//
  setWindowTitle(QApplication::applicationName() + " " + QApplication::applicationVersion());
//
//  #ifndef SCHAT_NO_UPDATE_WIDGET
//    QTimer::singleShot(0, m_update, SLOT(start()));
//  #endif

  m_start = new QPushButton(tr("&Start"), this);
  m_start->setDefault(true);
  connect(m_start, SIGNAL(clicked(bool)), this, SIGNAL(start()));

  m_stop = new QPushButton(tr("S&top"), this);

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
