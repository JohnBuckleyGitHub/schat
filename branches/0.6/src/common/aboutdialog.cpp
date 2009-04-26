/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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
#include <QtGui>

#include "aboutdialog.h"
#include "abstractsettings.h"

#ifndef SCHAT_NO_UPDATE_WIDGET
  #include "update/updatewidget.h"
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
  m_tabWidget->addTab(new AboutMain(this),      tr("О Программе"));
  m_tabWidget->addTab(new AboutMembers(this),   tr("Участники"));
  m_tabWidget->addTab(new AboutChangeLog(this), tr("История версий"));
  m_tabWidget->addTab(new AboutLicense(this),   tr("Лицензия"));

  m_closeButton = new QPushButton(QIcon(":/images/dialog-ok.png"), tr("Закрыть"), this);
  m_closeButton->setDefault(true);

  connect(m_closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));

  #ifndef SCHAT_NO_UPDATE_WIDGET
    m_update = new UpdateWidget(this);
  #endif

  QHBoxLayout *buttonLay = new QHBoxLayout;
  #ifndef SCHAT_NO_UPDATE_WIDGET
    buttonLay->addWidget(m_update);
  #endif
  buttonLay->addStretch();
  buttonLay->addWidget(m_closeButton);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_tabWidget);
  mainLay->addLayout(buttonLay);
  mainLay->setMargin(3);
  mainLay->setSpacing(3);

  setWindowTitle(tr("О Программе"));

  #ifndef SCHAT_NO_UPDATE_WIDGET
    QTimer::singleShot(0, m_update, SLOT(start()));
  #endif
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
    aboutLogo->setPixmap(QPixmap(":/images/logo-ny.png"));
  else
    aboutLogo->setPixmap(QPixmap(":/images/logo.png"));
  aboutLogo->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  QLabel *copyrightLabel = new QLabel("Copyright © 2008 - 2009 <b>IMPOMEZIA</b>. All rights reserved.", this);
  QLabel *homeLabel = new QLabel(QString("<b><a href='http://%1' style='text-decoration:none; color:#1a4d82;'>%2</a></b>")
      .arg(qApp->organizationDomain())
      .arg(tr("Официальный сайт")), this);
  homeLabel->setOpenExternalLinks(true);
  homeLabel->setToolTip("http://" + qApp->organizationDomain());

  QLabel *docLabel = new QLabel(QString("<b><a href='http://simple.impomezia.com' style='text-decoration:none; color:#1a4d82;'>%1</a></b>")
      .arg(tr("Документация")), this);
  docLabel->setOpenExternalLinks(true);
  docLabel->setToolTip("http://simple.impomezia.com");

  QLabel *libLabel = new QLabel(QString("%1<br /><b>Qt Open Source Edition %2</b>").arg(tr("Эта программа использует библиотеку:")).arg(qVersion()), this);

  QVBoxLayout *infoLay = new QVBoxLayout;
  infoLay->addWidget(copyrightLabel);
  infoLay->addSpacing(8);
  infoLay->addWidget(homeLabel);
  infoLay->addWidget(docLabel);
  infoLay->addSpacing(8);
  infoLay->addWidget(libLabel);
  infoLay->addStretch();
  infoLay->setSpacing(0);

  QLabel *impomeziaLabel = new QLabel("<a href='http://impomezia.com/'><img src=':/images/impomezia.png' /></a>", this);
  impomeziaLabel->setToolTip("IMPOMEZIA");
  impomeziaLabel->setOpenExternalLinks(true);
  impomeziaLabel->setAlignment(Qt::AlignBottom);

  QLabel *gplLabel = new QLabel("<a href='http://www.gnu.org/licenses/gpl.html'><img src=':/images/gplv3-88x31.png' /></a>", this);
  gplLabel->setToolTip("GNU General Public License");
  gplLabel->setOpenExternalLinks(true);
  gplLabel->setAlignment(Qt::AlignBottom);

  QLabel *qtLabel = new QLabel("<a href='http://www.qtsoftware.com/'><img src=':/images/qt-logo.png' /></a>", this);
  qtLabel->setToolTip("Qt Open Source Edition");
  qtLabel->setOpenExternalLinks(true);
  qtLabel->setAlignment(Qt::AlignBottom);

  #ifndef SCHAT_NO_DONATE
    QLabel *donateLabel = new QLabel(tr("Вы можете помочь развитию Simple Chat<br /><a href='http://impomezia.com/donate' style='text-decoration:none; color:#1a4d82;'>делом</a> или <a href='http://impomezia.com/donate' style='text-decoration:none; color:#1a4d82;'>материально</a>."), this);
    donateLabel->setStyleSheet("border: 1px solid #7581a9;"
        "border-radius: 3px;"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f5f6ff, stop:1 #f2f2ff)");
    donateLabel->setOpenExternalLinks(true);
  #endif

  QHBoxLayout *logosLay = new QHBoxLayout;
  #ifdef SCHAT_NO_DONATE
    logosLay->addStretch();
  #endif
  logosLay->addWidget(impomeziaLabel);
  logosLay->addWidget(gplLabel);
  logosLay->addWidget(qtLabel);
  #ifndef SCHAT_NO_DONATE
    logosLay->addStretch();
    logosLay->addWidget(donateLabel);
  #endif

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
  browser->setSource(QUrl().fromLocalFile(":/doc/members.html"));

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

  QString file = qApp->applicationDirPath() + "/doc/ChangeLog.html";
  if (QFile::exists(file)) {
    browser->setSearchPaths(QStringList() << (qApp->applicationDirPath() + "/doc"));
    browser->setSource(QUrl("ChangeLog.html"));
  }
  else
    browser->setText(QString("<h3 style='color:#da251d;'>%1</h3>"
                            "<p style='color:#da251d;'>%2</p>")
                            .arg(tr("ОШИБКА"))
                            .arg(tr("Файл <b>%1</b> не найден!").arg(file)));

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
      "<i>Copyright © 2008 - 2009 <b>IMPOMEZIA</b>. All rights reserved.</i></p>"
      "<p>This program is free software: you can redistribute it and/or modify "
      "it under the terms of the GNU General Public License as published by "
      "the Free Software Foundation, either version 3 of the License, or "
      "(at your option) any later version.</p>"
      "<p>This program is distributed in the hope that it will be useful, "
      "but WITHOUT ANY WARRANTY; without even the implied warranty of "
      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
      "GNU General Public License for more details.</p>"
      "<p>You should have received a copy of the GNU General Public License "
      "along with this program.  If not, see &lt;<a href='http://www.gnu.org/licenses/gpl.html' style='color:#1a4d82;'>http://www.gnu.org/licenses/gpl.html</a>&gt;.</p>"
  )
  .arg(QApplication::applicationName())
  .arg(QApplication::applicationVersion()));

  browser->setOpenExternalLinks(true);

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(browser);
  mainLay->setMargin(0);
}
