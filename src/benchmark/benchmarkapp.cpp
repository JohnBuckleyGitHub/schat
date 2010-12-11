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

#include <QTextCodec>
#include <QDebug>

#include "benchmark.h"
#include "benchmarkapp.h"
#include "benchmarkui.h"
#include "version.h"

/*!
 * Конструктор класса BenchmarkApp.
 */
BenchmarkApp::BenchmarkApp(int &argc, char **argv)
#if !defined(BENCHMARK_NO_UI)
  : QApplication(argc, argv)
#else
  : QCoreApplication(argc, argv)
#endif
{
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

  setApplicationName("Simple Chat Benchmark");
  setApplicationVersion(SCHAT_VERSION);
  setOrganizationName(SCHAT_ORGANIZATION);
  setOrganizationDomain(SCHAT_DOMAIN);
  addLibraryPath(applicationDirPath() + "/plugins");

  m_ui = new BenchmarkUi();
  connect(m_ui, SIGNAL(start()), this, SLOT(start()));
  connect(m_ui, SIGNAL(stop()), this, SLOT(stop()));

  m_ui->show();
}


BenchmarkApp::~BenchmarkApp()
{
}


void BenchmarkApp::start()
{
  m_benchmark = new Benchmark(this);
  connect(m_benchmark, SIGNAL(started(int)), m_ui, SLOT(started(int)));

  m_benchmark->start();
}


void BenchmarkApp::stop()
{
  m_benchmark->quit();
  m_benchmark->deleteLater();
}
