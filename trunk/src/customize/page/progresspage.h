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

#include <QMap>
#include <QProcess>
#include <QQueue>
#include <QWizardPage>

class QLabel;
class QProgressBar;
class QTextEdit;
class QTimer;
class WizardSettings;

struct FileInfoLite {
  qint64 size;
  QString name;
};

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
  void disableFinish();
  void nextJob();
  void processError();
  void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void processStandardOutput();
  void timer();

private:
  enum Nsi {
    Main,
    Core,
    Runtime
  };

  enum Jobs {
    CreateNSI,
    WriteConf,
    CreateEXE
  };

  bool createExe();
  bool createNsi();
  bool createNsi(Nsi type);
  bool writeDefaultConf();
  int bool2int(const QString &key) const;
  void compile();
  void processRange();

  bool m_mirror;
  bool m_mirrorCore;
  bool m_mirrorQt;
  bool m_overrideEmoticons;
  bool m_overrideLevels;
  bool m_overrideMirror;
  bool m_overrideNetwork;
  bool m_useDefaulConf;
  int m_step;
  Nsi m_currentExe;
  QLabel *m_label;
  QMap<Nsi, FileInfoLite> m_exe;
  QProcess *m_process;
  QProgressBar *m_progress;
  QProgressBar *packageProgressBar;
  QQueue<Jobs> m_queue;
  QQueue<Nsi> m_nsi;
  QString m_makensisFile;
  QString m_suffix;
  QString m_version;
  QTextEdit *m_log;
  QTimer *m_timer;
  WizardSettings *m_settings;
};

#endif /* PROGRESSPAGE_H_ */
