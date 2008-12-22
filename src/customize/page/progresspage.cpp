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

#include <QtGui>

#include "progresspage.h"
#include "wizardsettings.h"

/*!
 * \brief Конструктор класса ProgressPage.
 */
ProgressPage::ProgressPage(QWidget *parent)
  : QWizardPage(parent), m_process(0)
{
  m_settings = settings;
  setTitle(tr("Идёт создание дистрибутива"));
  setSubTitle(tr("Подождите идёт создание дистрибутива"));

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
  qDebug() << "ProgressPage::initializePage()";

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

  if (m_overrideLevels || m_overrideNetwork || m_overrideEmoticons || m_overrideMirror || QFile::exists(QApplication::applicationDirPath() + "/custom/default.conf"))
    m_useDefaulConf = true;
  else
    m_useDefaulConf = false;

  m_queue.enqueue(CreateNSI);
  if (m_useDefaulConf)
    m_queue.enqueue(WriteConf);
  m_queue.enqueue(CreateEXE);

  processRange();

  QTimer::singleShot(0, this, SLOT(disableFinish()));
  QTimer::singleShot(0, this, SLOT(nextJob()));
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
  qDebug() << "ProgressPage::nextJob()";

  if (m_queue.isEmpty()) {
    wizard()->button(QWizard::FinishButton)->setEnabled(true);
    m_label->setText(tr("Готово"));
    m_log->append(tr("<b style='color:#090;'>Все задачи успешно выполнены</b>"));
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
    if (!createExe())
      return;
  }
}


/*!
 * Уведомление об ошибке запуска процесса.
 */
void ProgressPage::processError()
{
  if (m_timer->isActive())
    m_timer->stop();

  m_log->append(tr("<span style='color:#900;'>Произошла ошибка при запуске <b>%1</b> [%2]</span>")
      .arg(m_makensisFile)
      .arg(m_process->errorString()));
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
    m_log->append(tr("<span style='color:#900;'>Произошёл сбой при запуске <b>%1</b> [%2]</span>")
          .arg(m_makensisFile)
          .arg(m_process->errorString()));
  else if (exitCode != 0)
    m_log->append(tr("<span style='color:#900;'>Произошла ошибка при выполнении <b>%1</b></span>")
          .arg(m_makensisFile));
  else {
    m_log->append(tr("Файл <b>%1</b> создан").arg(m_currentExe));
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
  qDebug() << "ProgressPage::createExe()";

  if (m_makensisFile.isEmpty()) {
    m_log->append(tr("<span style='color:#900;'>Не удалось определить путь к <b>makensis.exe</b></span>"));
    return false;
  }

  if (!QFile::exists(m_makensisFile)) {
    m_log->append(tr("<span style='color:#900;'>Не удалось найти файл <b>%1</b></span>").arg(m_makensisFile));
    return false;
  }

  if (!m_process) {
    m_process = new QProcess(this);
    m_process->setWorkingDirectory(QApplication::applicationDirPath() + "/custom");
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
 * Запускает создание NSI файлов.
 *
 * \return Возвращает \a true если все файлы успешно созданы, иначе \a false.
 */
bool ProgressPage::createNsi()
{
  m_label->setText(tr("Создание NSI файлов..."));

  if (!createNsi(Main))
    return false;

  if (m_mirror && m_mirrorCore) {
    if (!createNsi(Core))
      return false;

    if (m_mirrorQt)
      if (!createNsi(Runtime))
        return false;
  }

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
  qDebug() << "ProgressPage::createNsi()";

  QString fileName;
  if (type == Main)
    fileName = "setup.nsi";
  else if (type == Core)
    fileName = "setup-core.nsi";
  else if (type == Runtime)
    fileName = "setup-runtime.nsi";

  QFile file(QApplication::applicationDirPath() + "/custom/" + fileName);

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
           << "!define VC90_REDIST_DIR \"..\"" << endl
           << "!define OPT_DESKTOP " << bool2int("NsisDesktop") << endl
           << "!define OPT_QUICKLAUNCH " << bool2int("NsisQuickLaunch") << endl
           << "!define OPT_ALLPROGRAMS " << bool2int("NsisAllPrograms") << endl
           << "!define OPT_AUTOSTART " << bool2int("NsisAutostart") << endl
           << "!define OPT_AUTODAEMONSTART " << bool2int("NsisAutostartDaemon") << endl << endl
           << "!include \"engine\\core.nsh\"" << endl;

    m_log->append(tr("Файл <b>%1</b> создан").arg(fileName));
    m_progress->setValue(m_progress->value() + 2);
    return true;
  }
  else {
    m_log->append(tr("<span style='color:#900;'>Ошибка создания файла <b>%1</b> [%2]</span>").arg(fileName).arg(file.errorString()));
    return false;
  }
}


/*!
 * Запись файла \b default.conf, для переопределения настроек по умолчанию.
 *
 * \return Возвращает \a true в случае успешной записи.
 */
bool ProgressPage::writeDefaultConf()
{
  m_label->setText(tr("Запись файла default.conf..."));
  AbstractSettings s(QApplication::applicationDirPath() + "/custom/default.conf", this);

  if (m_overrideNetwork) s.setString("Network", m_settings->getString("Network"));
  if (m_overrideMirror) s.setString("Updates/Mirrors", m_settings->getString("MirrorUrl"));

  if (m_overrideLevels) {
    s.setInt("Updates/LevelQt", m_settings->getInt("LevelQt"));
    s.setInt("Updates/LevelCore", m_settings->getInt("LevelCore"));
  }

  if (m_overrideEmoticons) {
    s.setBool("UseEmoticons", true);
    s.setString("EmoticonTheme", m_settings->getString("Emoticons"));
  }

  s.write(true);

  if (s.status() != QSettings::NoError) {
    m_log->append(tr("<span style='color:#900;'>Произошла ошибка при записи файла <b>default.conf</b></span>"));
    return false;
  }

  m_log->append(tr("Файл <b>default.conf</b> записан"));
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
 */
void ProgressPage::compile()
{
  qDebug() << "ProgressPage::compile()";

  QStringList args;
  args << "/V1";
  Nsi nsi = m_nsi.dequeue();

  if (nsi == Main) {
    args << "setup.nsi";
    m_currentExe = "schat-";
    m_step = 40;
  }
  else if (nsi == Core) {
    args << "setup-core.nsi";
    m_currentExe = "schat-core-";
    m_step = 10;
  }
  else if (nsi == Runtime) {
    args << "setup-runtime.nsi";
    m_currentExe = "schat-runtime-";
    m_step = 30;
  }
  m_currentExe += (m_version + m_suffix + ".exe");

  m_label->setText(tr("Создание %1...").arg(m_currentExe));
  m_timer->start();
  m_process->start('"' + m_makensisFile + '"', args);
}


void ProgressPage::processRange()
{
  int max = 42;

  if (m_useDefaulConf)
    max += 2;

  if (m_mirror && m_mirrorCore) {
    max += 12;

    if (m_mirrorQt)
      max += 32;
  }

  m_progress->setRange(0, max);
}
