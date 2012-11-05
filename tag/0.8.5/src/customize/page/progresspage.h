/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

class Md5CalcThread;
class QLabel;
class QProgressBar;
class QTextEdit;
class QTimer;
class WizardSettings;

struct FileInfoLite {
  qint64 size;
  QString name;
  QByteArray md5;
};

/*!
 * \brief Страница отображающая процесс создания дистрибутива.
 */
class ProgressPage : public QWizardPage
{
  Q_OBJECT

public:
  enum Nsi {
    Main,
    Core,
    Runtime,
    Customize
  };

  ProgressPage(QWidget *parent = 0);
  void initializePage();

private slots:
  void calcDone(int type, const QByteArray &hash, qint64 size);
  void calcDone(bool error);
  void disableFinish();
  void nextJob();
  void processError();
  void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void processStandardOutput();
  void timer();

private:
  enum Jobs {
    CreateNSI,
    WriteConf,
    CreateEXE,
    CalcMd5,
    CreateMirrorXml
  };

  bool createExe();
  bool createMirrorXml();
  bool createNsi();
  bool createNsi(Nsi type);
  bool writeDefaultConf();
  int bool2int(const QString &key) const;
  void compile();
  void processRange();

  bool m_dist;
  bool m_mirror;
  bool m_mirrorCore;
  bool m_mirrorQt;
  bool m_overrideEmoticons;
  bool m_overrideLevels;
  bool m_overrideMirror;
  bool m_overrideNetwork;
  bool m_useDefaulConf;
  int m_step;
  Md5CalcThread *m_md5Calc;
  Nsi m_currentExe;
  QLabel *m_label;
  QMap<Nsi, FileInfoLite> m_exe;
  QProcess *m_process;
  QProgressBar *m_progress;
  QProgressBar *packageProgressBar;
  QQueue<Jobs> m_queue;
  QQueue<Nsi> m_nsi;
  QString m_makensisFile;
  QString m_rootPath;
  QString m_suffix;
  QString m_version;
  QTextEdit *m_log;
  QTimer *m_timer;
  WizardSettings *m_settings;
};

#endif /* PROGRESSPAGE_H_ */
