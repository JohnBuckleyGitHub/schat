/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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

#ifndef PROGRESSPAGE_H_
#define PROGRESSPAGE_H_

#include <QProcess>
#include <QQueue>
#include <QWizardPage>

class QLabel;
class QProgressBar;
class QTextEdit;
class WizardSettings;

/*!
 * \brief Страница отображающая процесс создания дистрибутива.
 */
class ProgressPage : public QWizardPage
{
  Q_OBJECT

public:
  ProgressPage(QWidget *parent = 0);
  void initializePage();

private slots:
  void nextJob();
  void processError();
  void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void processStandardOutput();

private:
  enum Nsi {
    Main,
    Core,
    Runtime
  };

  enum Jobs {
    CreateNSI,
    CreateEXE
  };

  bool createExe();
  bool createNsi();
  bool createNsi(Nsi type);
  void compile();

  bool m_mirror;
  bool m_mirrorCore;
  bool m_mirrorQt;
  QLabel *m_label;
  QProcess *m_process;
  QProgressBar *m_progress;
  QProgressBar *packageProgressBar;
  QQueue<Jobs> m_queue;
  QQueue<Nsi> m_nsi;
  QString m_currentExe;
  QString m_makensisFile;
  QString m_suffix;
  QString m_version;
  QTextEdit *m_log;
  WizardSettings *m_settings;
};

#endif /* PROGRESSPAGE_H_ */
