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
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QtGui>

#include "aboutdialog.h"
#include "update/updatewidget.h"

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

  m_closeButton = new QPushButton(QIcon(":/images/ok.png"), tr("Закрыть"), this);
  m_closeButton->setDefault(true);

  connect(m_closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));

  m_update = new UpdateWidget(this);

  QHBoxLayout *buttonLay = new QHBoxLayout;
  buttonLay->addWidget(m_update);
  buttonLay->addStretch();
  buttonLay->addWidget(m_closeButton);

  QVBoxLayout *mainLay = new QVBoxLayout;
  mainLay->addWidget(m_tabWidget);
  mainLay->addLayout(buttonLay);
  mainLay->setMargin(3);
  mainLay->setSpacing(3);
  setLayout(mainLay);

  setWindowTitle(tr("О Программе"));

  QTimer::singleShot(0, m_update, SLOT(start()));
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
  aboutLogo->setPixmap(QPixmap(":/images/logo.png"));
  aboutLogo->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  QLabel *copyrightLabel = new QLabel("Copyright © 2008 <b>IMPOMEZIA</b>. All rights reserved.", this);
  QLabel *homeLabel = new QLabel(QString("<b><a href='http://%1' style='text-decoration:none; color:#1a4d82;'>%2</a></b>")
      .arg(qApp->organizationDomain())
      .arg(tr("Официальный сайт")), this);

  homeLabel->setOpenExternalLinks(true);
  homeLabel->setToolTip("http://" + qApp->organizationDomain());
  QLabel *libLabel = new QLabel(QString("%1<br /><b>Qt Open Source Edition %2</b>").arg(tr("Эта программа использует библиотеку:")).arg(qVersion()), this);

  QVBoxLayout *infoLay = new QVBoxLayout;
  infoLay->addWidget(copyrightLabel);
  infoLay->addSpacing(8);
  infoLay->addWidget(homeLabel);
  infoLay->addSpacing(8);
  infoLay->addWidget(libLabel);
  infoLay->addStretch();

  QLabel *impomeziaLabel = new QLabel("<a href='http://impomezia.com/'><img src=':/images/impomezia.png' /></a>", this);
  impomeziaLabel->setToolTip("IMPOMEZIA");
  impomeziaLabel->setOpenExternalLinks(true);

  QLabel *gplLabel = new QLabel("<a href='http://www.gnu.org/licenses/gpl.html'><img src=':/images/gplv3-88x31.png' /></a>", this);
  gplLabel->setToolTip("GNU General Public License");
  gplLabel->setOpenExternalLinks(true);

  QLabel *qtLabel = new QLabel("<a href='http://trolltech.com/'><img src=':/images/qt-logo.png' /></a>", this);
  qtLabel->setToolTip("Qt Open Source Edition");
  qtLabel->setOpenExternalLinks(true);

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
      "<i>Copyright © 2008 <b>IMPOMEZIA</b>. All rights reserved.</i></p>"
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
