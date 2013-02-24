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

#include <QApplication>
#include <QDir>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTimer>

#include "3rdparty/qtwin.h"
#include "aboutdialog.h"
#include "abstractsettings.h"

#if defined(SCHAT_CLIENT)
  #include "simplechatapp.h"
  #include "translation.h"
#endif

#ifndef SCHAT_NO_UPDATE_WIDGET
  #include "update/updatewidget.h"
#endif

#if defined(Q_WS_WIN)
  #include <qt_windows.h>
  #define WM_DWMCOMPOSITIONCHANGED 0x031E // Composition changed window message
#endif

/*!
 * \brief Конструктор класса AboutDialog.
 */
AboutDialog::AboutDialog(QWidget *parent)
  : QDialog(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

  m_tabWidget = new QTabWidget(this);
  m_tabWidget->addTab(new AboutMain(this),      tr("About"));
  m_tabWidget->addTab(new AboutMembers(this),   tr("Members"));
  m_tabWidget->addTab(new AboutChangeLog(this), tr("Changelog"));
  m_tabWidget->addTab(new AboutLicense(this),   tr("License"));

  m_closeButton = new QPushButton(QIcon(":/images/dialog-ok.png"), tr("Close"), this);
  m_closeButton->setDefault(true);

  connect(m_closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));

  m_bottom = new QWidget(this);
  m_bottom->setObjectName("AboutBottom");
  QHBoxLayout *buttonLay = new QHBoxLayout(m_bottom);
  buttonLay->setMargin(2);
  #ifndef SCHAT_NO_UPDATE_WIDGET
    m_update = new UpdateWidget(this);
    buttonLay->addWidget(m_update);
  #endif
  buttonLay->addStretch();
  buttonLay->addWidget(m_closeButton);

  m_mainLay = new QVBoxLayout(this);
  m_mainLay->addWidget(m_tabWidget);
  m_mainLay->addWidget(m_bottom);
  setStyleSheet();

  setWindowTitle(tr("About Simple Chat"));

  #ifndef SCHAT_NO_UPDATE_WIDGET
    QTimer::singleShot(0, m_update, SLOT(start()));
  #endif
}


AboutDialog::~AboutDialog()
{
# if defined(Q_WS_WIN)
  QtWin::release(this);
# endif
}


#if defined(Q_WS_WIN)
bool AboutDialog::winEvent(MSG *message, long *result)
{
  if (message && message->message == WM_DWMCOMPOSITIONCHANGED) {
    setStyleSheet();
  }
  return QWidget::winEvent(message, result);
}
#endif


void AboutDialog::setStyleSheet()
{
  #if defined(Q_WS_WIN)
  m_bottom->setStyleSheet(QString("QWidget#AboutBottom { background-color: %1; }").arg(palette().color(QPalette::Window).name()));
  #endif

  if (QtWin::isCompositionEnabled()) {
    m_mainLay->setMargin(0);
    m_mainLay->setSpacing(0);
    QtWin::extendFrameIntoClientArea(this);
  }
  else {
    m_mainLay->setMargin(3);
    m_mainLay->setSpacing(3);
  }
}


/*!
 * \brief Конструктор класса AboutMain.
 */
AboutMain::AboutMain(QWidget *parent)
  : QWidget(parent)
{
  QLabel *nameLabel = new QLabel(QString("<h2>%1 %2</h2>").arg(QApplication::applicationName()).arg(QApplication::applicationVersion()), this);
  nameLabel->setWordWrap(false);

  QLabel *aboutLogo = new QLabel(this);
  if (AbstractSettings::isNewYear())
    aboutLogo->setPixmap(QPixmap(":/images/schat-ny.png"));
  else
    aboutLogo->setPixmap(QPixmap(":/images/schat.png"));
  aboutLogo->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  QLabel *copyrightLabel = new QLabel("Copyright © 2008-2013 <b>IMPOMEZIA</b>. All rights reserved.", this);
  QLabel *homeLabel = new QLabel(QString("<b><a href='https://%1' style='text-decoration:none; color:#1a4d82;'>%2</a></b>")
      .arg(QApplication::organizationDomain())
      .arg(tr("Official website")), this);
  homeLabel->setOpenExternalLinks(true);
  homeLabel->setToolTip("https://" + QApplication::organizationDomain());

  QLabel *docLabel = new QLabel(QString("<b><a href='http://wiki.schat.me' style='text-decoration:none; color:#1a4d82;'>%1</a></b>")
      .arg(tr("Documentation")), this);
  docLabel->setOpenExternalLinks(true);
  docLabel->setToolTip("http://wiki.schat.me");

  QLabel *libLabel = new QLabel(QString("%1<br /><b>Qt Open Source Edition %2</b> (%3)")
      .arg(tr("This program uses library:"))
      .arg(qVersion()).arg(QSysInfo::WordSize == 32 ? tr("32 bit") : tr("64 bit")), this);

  QVBoxLayout *infoLay = new QVBoxLayout;
  infoLay->addWidget(copyrightLabel);
  infoLay->addSpacing(8);
  infoLay->addWidget(homeLabel);
  infoLay->addWidget(docLabel);
  infoLay->addSpacing(8);
  infoLay->addWidget(libLabel);
  infoLay->addStretch();
  infoLay->setSpacing(0);

  QLabel *impomeziaLabel = new QLabel("<a href='https://schat.me/'><img src=':/images/impomezia.png' /></a>", this);
  impomeziaLabel->setToolTip("IMPOMEZIA");
  impomeziaLabel->setOpenExternalLinks(true);
  impomeziaLabel->setAlignment(Qt::AlignBottom);

  QLabel *gplLabel = new QLabel("<a href='http://www.gnu.org/licenses/gpl.html'><img src=':/images/gplv3-88x31.png' /></a>", this);
  gplLabel->setToolTip("GNU General Public License");
  gplLabel->setOpenExternalLinks(true);
  gplLabel->setAlignment(Qt::AlignBottom);

  QLabel *qtLabel = new QLabel("<a href='http://qt.nokia.com/'><img src=':/images/qt-logo.png' /></a>", this);
  qtLabel->setToolTip("Qt Open Source Edition");
  qtLabel->setOpenExternalLinks(true);
  qtLabel->setAlignment(Qt::AlignBottom);

  QHBoxLayout *logosLay = new QHBoxLayout;
  logosLay->addStretch();
  logosLay->addWidget(impomeziaLabel);
  logosLay->addWidget(gplLabel);
  logosLay->addWidget(qtLabel);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(nameLabel, 0, 0, 1, 2);
  mainLay->addWidget(aboutLogo, 1, 0);
  mainLay->addLayout(infoLay, 1, 1);
  mainLay->addLayout(logosLay, 2, 0, 1, 2);
  mainLay->setColumnStretch(1, 1);
  mainLay->setRowStretch(1, 1);
}


/*!
 * \brief Конструктор класса AboutMembers.
 */
AboutMembers::AboutMembers(QWidget *parent)
  : QWidget(parent)
{
  QTextBrowser *browser = new QTextBrowser(this);
  browser->setOpenExternalLinks(true);

  #if defined(SCHAT_CLIENT)
  if (SimpleChatApp::instance()->translation()->name().left(2) == "ru")
    browser->setSource(QUrl().fromLocalFile(":/doc/members_ru.html"));
  else
  #endif
    browser->setSource(QUrl::fromLocalFile(":/doc/members.html"));

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(browser);
  mainLay->setMargin(0);
}


/*!
 * \brief Конструктор класса AboutChangeLog.
 */
AboutChangeLog::AboutChangeLog(QWidget *parent)
  : QWidget(parent)
{
  QTextBrowser *browser = new QTextBrowser(this);
  browser->setOpenExternalLinks(true);

  QString path;
  if (AbstractSettings::isUnixLike())
    #if defined(Q_OS_MAC)
    path = SCHAT_UNIX_DOC("doc");
    #else
    path = SCHAT_UNIX_DOC("html");
    #endif
  else
    path = QApplication::applicationDirPath() + "/doc";

  if (QFile::exists(path + "/ChangeLog.html"))
    browser->setSource(QUrl::fromLocalFile(path + "/ChangeLog.html"));
  else
    browser->setText(QString("<p style='color:#da251d;'>%2</p>")
                            .arg(tr("File <b>%1</b> not found!").arg(path + "/ChangeLog.html")));

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(browser);
  mainLay->setMargin(0);
}


/*!
 * \brief Конструктор класса AboutLicense.
 */
AboutLicense::AboutLicense(QWidget *parent)
  : QWidget(parent)
{
  QTextBrowser *browser = new QTextBrowser(this);
  browser->setText(QString(
      "<p style='color:#333;'><b>%1 %2</b><br />"
      "<i>Copyright © 2008 - 2012 <b>IMPOMEZIA</b>. All rights reserved.</i></p>"
      "<p>This program is free software: you can redistribute it and/or modify "
      "it under the terms of the GNU General Public License as published by "
      "the Free Software Foundation, either version 3 of the License, or "
      "(at your option) any later version.</p>"
      "<p>This program is distributed in the hope that it will be useful, "
      "but WITHOUT ANY WARRANTY; without even the implied warranty of "
      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
      "GNU General Public License for more details.</p>"
      "<p>You should have received a copy of the GNU General Public License "
      "along with this program. If not, see &lt;<a href='http://www.gnu.org/licenses/gpl.html' style='color:#1a4d82;'>http://www.gnu.org/licenses/gpl.html</a>&gt;.</p>"
  )
  .arg(QApplication::applicationName())
  .arg(QApplication::applicationVersion()));

  browser->setOpenExternalLinks(true);

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(browser);
  mainLay->setMargin(0);
}
