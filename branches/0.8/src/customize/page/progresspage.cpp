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

#include <QAbstractButton>
#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>

#include "md5calcthread.h"
#include "mirrorwriter.h"
#include "progresspage.h"
#include "updatexmlreader.h"
#include "wizardsettings.h"

/*!
 * \brief Конструктор класса ProgressPage.
 */
ProgressPage::ProgressPage(QWidget *parent)
  : QWizardPage(parent), m_process(0)
{
  m_settings = settings;
  m_dist = m_settings->dist();
  m_rootPath = m_settings->rootPath();

  setTitle(tr("Creation distribution package is in progress"));
  setSubTitle(tr("Please wait, creation distribution package is in progress"));

  m_label = new QLabel(this);
  m_progress = new QProgressBar(this);
  m_progress->setValue(0);
  m_log = new QTextEdit(this);
  m_log->setReadOnly(true);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_label);
  mainLay->addWidget(m_progress);
  mainLay->addWidget(m_log);
  mainLay->setMargin(0);
}


/*!
 * Инициализация страницы.
 * Создаёт очередь заданий и по нулевому таймеру запускает следующие задание.
 */
void ProgressPage::initializePage()
{
  m_mirror            = m_settings->getBool("Mirror");
  m_mirrorCore        = m_settings->getBool("MirrorCore");
  m_mirrorQt          = m_settings->getBool("MirrorQt");
  m_overrideLevels    = m_settings->getBool("OverrideLevels");
  m_overrideNetwork   = m_settings->getBool("OverrideNetwork");
  m_overrideEmoticons = m_settings->getBool("OverrideEmoticons");
  m_overrideMirror    = m_settings->getBool("OverrideMirror");
  m_suffix            = m_settings->getString("Suffix");
  m_version           = m_settings->getString("Version");
  m_makensisFile      = m_settings->getString("MakensisFile");
  if (!m_suffix.isEmpty())
    m_suffix = "-" + m_suffix;

  if (m_overrideLevels || m_overrideNetwork || m_overrideEmoticons || m_overrideMirror || QFile::exists(m_rootPath + "/custom/default.conf"))
    m_useDefaulConf = true;
  else
    m_useDefaulConf = false;

  m_queue.enqueue(CreateNSI);
  if (m_useDefaulConf)
    m_queue.enqueue(WriteConf);
  m_queue.enqueue(CreateEXE);

  if (m_mirror && m_mirrorCore) {
    m_queue.enqueue(CalcMd5);
    m_queue.enqueue(CreateMirrorXml);
  }

  processRange();

  QTimer::singleShot(0, this, SLOT(disableFinish()));
  QTimer::singleShot(0, this, SLOT(nextJob()));
}


/*!
 * Заносит результат вычислений контрольной суммы в \a m_exe.
 *
 * \param type Тип файла.
 * \param hash Контрольная сумма.
 * \param size Новый размер файла после добавление ЭЦП, 0 - если размер не изменился.
 */
void ProgressPage::calcDone(int type, const QByteArray &hash, qint64 size)
{
  Nsi key = static_cast<Nsi>(type);

  if (m_exe.contains(key)) {
    FileInfoLite info = m_exe.value(key);
    info.md5 = hash;
    if (size)
      info.size = size;
    m_exe.insert(key, info);
  }
}


/*!
 * Уведомление о завершении расчёта контрольных сумм.
 *
 * \param error \a true если произошла ошибка.
 */
void ProgressPage::calcDone(bool error)
{
  if (error)
    m_log->append("<span style='color:#900;'>" + tr("An error occurred when calculating the checksum") + "</span>");
  else {
    m_log->append(tr("Completed calculation of checksums"));
    m_progress->setValue(m_progress->value() + 10);
    QTimer::singleShot(0, this, SLOT(nextJob()));
  }
}


/*!
 * Отключает кнопку \b Finish.
 */
void ProgressPage::disableFinish()
{
  wizard()->button(QWizard::FinishButton)->setEnabled(false);
}


/*!
 * Выполняет следующее задание из очереди \a m_queue.
 */
void ProgressPage::nextJob()
{
  if (m_queue.isEmpty()) {
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
    m_label->setText(tr("Done"));
    m_log->append("<b style='color:#090;'>" + tr("All tasks completed successfully") + "</b>");
    return;
  }

  Jobs job = m_queue.dequeue();

  if (job == CreateNSI) {
    m_settings->write();

    if (!createNsi())
      return;

    QTimer::singleShot(0, this, SLOT(nextJob()));
  }
  else if (job == WriteConf) {
    if (!writeDefaultConf())
      return;

    QTimer::singleShot(0, this, SLOT(nextJob()));
  }
  else if (job == CreateEXE) {
    if (m_dist)
      m_settings->write();

    if (!createExe())
      return;
  }
  else if (job == CalcMd5) {
    m_label->setText(tr("Calculation of checksums"));
    m_md5Calc = new Md5CalcThread(m_exe, m_settings->getString("PfxFile"), m_settings->getString("PfxPassword"), this);
    connect(m_md5Calc, SIGNAL(done(bool)), SLOT(calcDone(bool)));
    connect(m_md5Calc, SIGNAL(done(int, const QByteArray &, qint64)), SLOT(calcDone(int, const QByteArray &, qint64)));

    m_md5Calc->start();
  }
  else if (job == CreateMirrorXml) {
    m_label->setText(tr("Creating mirror.xml"));
    if (createMirrorXml()) {
      m_progress->setValue(m_progress->value() + 2);
      m_log->append(tr("File <b>mirror.xml</b> is created"));
      QTimer::singleShot(0, this, SLOT(nextJob()));
    }
    else
      m_log->append("<span style='color:#900;'>" + tr("An error occurred while creating <b>mirror.xml</b>") + "</span>");
  }
}


/*!
 * Уведомление об ошибке запуска процесса.
 */
void ProgressPage::processError()
{
  if (m_timer->isActive())
    m_timer->stop();

  m_log->append("<span style='color:#900;'>" + tr("An error occurred when running <b>%1</b> [%2]")
      .arg(m_makensisFile)
      .arg(m_process->errorString()) + "</span>");
}


/*!
 * Завершение процесса компиляции, в случае успешного завершения если
 * очередь \a m_nsi пуста, запускается выполнение следующей задачи, если
 * не пуста, то запускается следующая компиляция.
 */
void ProgressPage::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  if (m_timer->isActive())
    m_timer->stop();

  if (exitStatus == QProcess::CrashExit)
    m_log->append("<span style='color:#900;'>" + tr("Failed to start <b>%1</b> [%2]")
          .arg(m_makensisFile)
          .arg(m_process->errorString()) + "</span>");
  else if (exitCode != 0)
    m_log->append("<span style='color:#900;'>" + tr("An error occurred while <b>%1</b>")
          .arg(m_makensisFile) + "</span>");
  else {
    FileInfoLite liteInfo = m_exe.value(m_currentExe);
    QFileInfo info(liteInfo.name);
    liteInfo.size = info.size();
    m_exe.insert(m_currentExe, liteInfo);

    m_log->append(tr("File <b>%1 </b> is created, size is %n bytes", "", liteInfo.size).arg(info.fileName()));
    m_progress->setValue(m_progress->value() + m_step);

    if (m_nsi.isEmpty())
      QTimer::singleShot(0, this, SLOT(nextJob()));
    else
      compile();
  }
}


/*!
 * Отображение текста ошибки при выполнении компиляции.
 */
void ProgressPage::processStandardOutput()
{
  m_log->append(QString::fromLocal8Bit(m_process->readAllStandardOutput()));
}


/*!
 * Увеличения значения прогресса по таймеру.
 */
void ProgressPage::timer()
{
  if (m_step > 5) {
    m_progress->setValue(m_progress->value() + 1);
    m_step--;
  }
  else
    m_timer->stop();
}


/*!
 * Запускает создание EXE файлов.
 *
 * \return Возвращает \a true, если запуск первого процесса в очереди был успешно
 * инициирован, \a false если произошла ошибка на раннем этапе.
 */
bool ProgressPage::createExe()
{
  if (m_makensisFile.isEmpty()) {
    m_log->append("<span style='color:#900;'>" + tr("Unable to determine the path to <b>makensis.exe</b>") + "</span>");
    return false;
  }

  if (!QFile::exists(m_makensisFile)) {
    m_log->append("<span style='color:#900;'>" + tr("Could not find file <b>%1</b>").arg(m_makensisFile) + "</span>");
    return false;
  }

  if (!m_process) {
    m_process = new QProcess(this);
    if (m_dist)
      m_process->setWorkingDirectory(m_rootPath + "/os/win32");
    else
      m_process->setWorkingDirectory(m_rootPath + "/custom");

    connect(m_process, SIGNAL(error(QProcess::ProcessError)), SLOT(processError()));
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(m_process, SIGNAL(readyReadStandardOutput()), SLOT(processStandardOutput()));
  }

  m_timer = new QTimer(this);
  m_timer->setInterval(500);
  connect(m_timer, SIGNAL(timeout()), SLOT(timer()));

  if (!m_nsi.isEmpty())
    compile();
  else
    return false;

  return true;
}


/*!
 * Создаёт файл \b mirror.xml.
 *
 * \return Возвращает \a true в случае успеха.
 */
bool ProgressPage::createMirrorXml()
{
  QString fileName;
  if (m_dist)
    fileName = m_rootPath + "/os/win32/out/mirror.xml";
  else
    fileName = m_rootPath + "/custom/out/mirror.xml";

  QList<VersionInfo> versions;
  QMultiMap<int, FileInfo> files;

  if (QFile::exists(fileName)) {
    UpdateXmlReader reader;
    reader.readFile(fileName);
    if (reader.isValid()) {
      versions = reader.version();
      files = reader.files();
    }
  }

  VersionInfo runtime;
  if (m_mirrorQt) {
    runtime.level   = m_settings->getInt("LevelQt");
    runtime.type    = "qt";
    runtime.version = qVersion();
  }
  else if (!versions.isEmpty())
    runtime = versions.at(0);

  versions.clear();
  if (UpdateXmlReader::isValid(runtime))
    if (runtime.type == "qt")
      versions << runtime;

  VersionInfo core;
  core.level   = m_settings->getInt("LevelCore");
  core.type    = "core";

  if (m_dist)
    core.version = QApplication::applicationVersion();
  else
    core.version = m_settings->getString("Version");

  versions << core;

  int levelQt = m_settings->getInt("LevelQt");
  if (m_mirrorQt) {
    QMutableMapIterator<int, FileInfo> i(files);
    while (i.hasNext()) {
      i.next();
      if (i.key() == levelQt)
        if (i.value().type == "qt")
          i.remove();
    }
  }

  int levelCore = m_settings->getInt("LevelCore");
  QMutableMapIterator<int, FileInfo> i(files);
  while (i.hasNext()) {
    i.next();
    if (i.key() == levelCore)
      if (i.value().type == "core")
        i.remove();
  }

  if (m_mirrorQt) {
    FileInfo qtFile;
    FileInfoLite qtFileLite = m_exe.value(Runtime);
    qtFile.size  = qtFileLite.size;
    qtFile.md5   = qtFileLite.md5.toHex();
    qtFile.name  = QFileInfo(qtFileLite.name).fileName();
    qtFile.level = levelQt;
    qtFile.type  = "qt";

    if (!UpdateXmlReader::isValid(qtFile))
      return false;

    files.insert(levelQt, qtFile);
  }

  FileInfo coreFile;
  FileInfoLite coreFileLite = m_exe.value(Core);
  coreFile.size  = coreFileLite.size;
  coreFile.md5   = coreFileLite.md5.toHex();
  coreFile.name  = QFileInfo(coreFileLite.name).fileName();
  coreFile.level = levelCore;
  coreFile.type  = "core";

  if (!UpdateXmlReader::isValid(coreFile))
    return false;

  files.insert(levelCore, coreFile);

  MirrorWriter writer(versions, files, m_settings->getString("BaseUrl"));
  writer.comparable(m_settings->getBool("Comparable"));
  return writer.writeFile(fileName);
}


/*!
 * Запускает создание NSI файлов.
 *
 * \return Возвращает \a true если все файлы успешно созданы, иначе \a false.
 */
bool ProgressPage::createNsi()
{
  m_label->setText(tr("Creating NSI files..."));

  if (!createNsi(Main))
    return false;

  if (m_mirror && m_mirrorCore) {
    if (!createNsi(Core))
      return false;

    if (m_mirrorQt)
      if (!createNsi(Runtime))
        return false;
  }

  if (m_dist)
    m_nsi.enqueue(Customize);

  return true;
}


/*!
 * Создаёт NSI файл с заданными настройками.
 * Папка \b custom должна существовать.
 *
 * \param type Тип файла, который нужно создать.
 * \return     Возвращает \a true в случае успешного создания файла, иначе \a false.
 */
bool ProgressPage::createNsi(Nsi type)
{
  if (m_dist) {
    m_nsi.enqueue(type);
    m_progress->setValue(m_progress->value() + 2);
    return true;
  }

  QString fileName;
  if (type == Main)
    fileName = "setup.nsi";
  else if (type == Core)
    fileName = "setup-core.nsi";
  else if (type == Runtime)
    fileName = "setup-runtime.nsi";

  QFile file(m_rootPath + "/custom/" + fileName);

  if (file.open(QIODevice::WriteOnly)) {
    m_nsi.enqueue(type);

    QTextStream stream(&file);
    stream.setCodec("CP-1251");

    stream << "# Generated by " << QApplication::applicationName() << " Customize " << QApplication::applicationVersion() << endl << endl;

    if (type == Main || type == Core) {
      stream << "!define Core" << endl
             << "!define Emoticons.Kolobok" << endl
             << "!define Emoticons.Simple" << endl
             << "!define Daemon" << endl;

      if (m_useDefaulConf)
        stream << "!define SCHAT_DEFAULT_CONF 1" << endl;
    }

    if (type == Main || type == Runtime)
      stream << "!define Qt" << endl;

    if (type == Core)
      stream << "!define SCHAT_PREFIX \"core-\"" << endl;
    else if (type == Runtime)
      stream << "!define SCHAT_PREFIX \"runtime-\"" << endl;

    if (!m_suffix.isEmpty())
      stream << "!define SCHAT_SUFFIX \"" << m_suffix << '"' << endl;

    stream << "!define SCHAT_VERSION " << m_version << endl
           << "!define SCHAT_QTDIR \"..\"" << endl
           << "!define SCHAT_QT_BINDIR \"..\"" << endl
           << "!define SCHAT_BINDIR \"..\"" << endl
           << "!define SCHAT_DATADIR \"..\"" << endl
           << "!define VC100_REDIST_DIR \"..\"" << endl
           << "!define OPT_DESKTOP " << bool2int("NsisDesktop") << endl
           << "!define OPT_QUICKLAUNCH " << bool2int("NsisQuickLaunch") << endl
           << "!define OPT_ALLPROGRAMS " << bool2int("NsisAllPrograms") << endl
           << "!define OPT_AUTOSTART " << bool2int("NsisAutostart") << endl
           << "!define OPT_AUTODAEMONSTART " << bool2int("NsisAutostartDaemon") << endl << endl
           << "!include \"engine\\core.nsh\"" << endl;

    m_log->append(tr("File <b>%1</b> is created").arg(fileName));
    m_progress->setValue(m_progress->value() + 2);
    return true;
  }
  else {
    m_log->append("<span style='color:#900;'>" + tr("Error creating file <b>%1</b> [%2]").arg(fileName).arg(file.errorString()) + "</span>");
    return false;
  }
}


/*!
 * Запись файла \b default.conf, для переопределения настроек по умолчанию.
 *
 * \todo Существует неоднозначность определения настроек, если в интерфейсе переопределение не было выбрано,
 * а значение было вручную записано в default.conf.
 *
 * \return Возвращает \a true в случае успешной записи.
 */
bool ProgressPage::writeDefaultConf()
{
  m_label->setText(tr("Write file default.conf..."));
  AbstractSettings s(m_rootPath + "/custom/default.conf", this);

  if (m_overrideNetwork) s.setString("Network", m_settings->getString("Network"));
  if (m_overrideMirror) s.setString("Updates/Mirrors", m_settings->getString("MirrorUrl"));

  if (m_overrideLevels) {
    s.setInt("Updates/LevelQt", m_settings->getInt("LevelQt"));
    s.setInt("Updates/LevelCore", m_settings->getInt("LevelCore"));
  }
  else {
    s.remove("Updates/LevelQt");
    s.remove("Updates/LevelCore");
  }

  if (m_settings->getBool("AutoDownloadUpdates"))
    s.setBool("Updates/AutoDownload", true);
  else
    s.remove("Updates/AutoDownload");

  if (m_overrideEmoticons) {
    s.setBool("UseEmoticons", true);
    s.setString("EmoticonTheme", m_settings->getString("Emoticons"));
  }

  s.write(true);

  if (s.status() != QSettings::NoError) {
    m_log->append("<span style='color:#900;'>" + tr("An error occurred while writing the file <b>default.conf</b>") + "</span>");
    return false;
  }

  m_log->append(tr("File <b>default.conf</b> recorded"));
  m_progress->setValue(m_progress->value() + 2);

  return true;
}


int ProgressPage::bool2int(const QString &key) const
{
  if (m_settings->getBool(key))
    return 1;
  else
    return 0;
}


/*!
 * Запуск компилятора NSIS для создания exe файла(ов).
 *
 * Функция формирует командную строку для запуска компилятора и заносит результирующие
 * имя exe файла в \a m_exe.
 *
 * В случае успешной компиляции вызывается слот processFinished(int exitCode, QProcess::ExitStatus exitStatus).
 * В случае ошибки вызывается слот processError().
 */
void ProgressPage::compile()
{
  QStringList args;
  args << "/V1";
  Nsi nsi = m_nsi.dequeue();
  QString currentExe;

  if (nsi == Main) {
    args << "setup.nsi";
    currentExe = "schat-";
    m_step = 40;
  }
  else if (nsi == Core) {
    args << "setup-core.nsi";
    currentExe = "schat-core-";
    m_step = 10;
  }
  else if (nsi == Runtime) {
    args << "setup-runtime.nsi";
    currentExe = "schat-runtime-";
    m_step = 30;
  }
  else if (nsi == Customize) {
    args << "setup-customize.nsi";
    currentExe = "schat-customize-";
    m_step = 10;
  }

  currentExe += (m_version + m_suffix + ".exe");

  FileInfoLite fileInfo;
  fileInfo.size = 0;

  if (m_dist)
    fileInfo.name = m_rootPath + "/os/win32/out/" + currentExe;
  else
    fileInfo.name = m_rootPath + "/custom/out/" + currentExe;

  m_exe.insert(nsi, fileInfo);
  m_currentExe = nsi;

  m_label->setText(tr("Creating %1...").arg(currentExe));
  m_timer->start();
  m_process->start('"' + m_makensisFile + '"', args);
}


void ProgressPage::processRange()
{
  int max = 42;

  if (m_dist)
    max += 10;

  if (m_useDefaulConf)
    max += 2;

  if (m_mirror && m_mirrorCore) {
    max += 24;

    if (m_mirrorQt)
      max += 32;
  }

  m_progress->setRange(0, max);
}
