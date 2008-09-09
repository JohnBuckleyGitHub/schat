/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.com)
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

#include <QtGui>

#include "aboutdialog.h"
#include "version.h"

/*!
 * \brief Конструктор класса AboutDialog.
 */
AboutDialog::AboutDialog(QWidget *parent)
  : QDialog(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

  m_tabWidget = new QTabWidget(this);
  m_tabWidget->addTab(new MainTab(this), tr("О Программе"));
  m_tabWidget->addTab(new MembersTab(this), tr("Участники"));
  m_tabWidget->addTab(new ChangeLogTab(this), tr("История версий"));
  m_tabWidget->addTab(new LicenseTab(this), tr("Лицензия"));

  m_closeButton = new QPushButton(tr("Закрыть"), this);
  m_closeButton->setDefault(true);

  connect(m_closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch();
  buttonLayout->addWidget(m_closeButton);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_tabWidget);
  mainLayout->addLayout(buttonLayout);
  mainLayout->setMargin(3);
  mainLayout->setSpacing(3);
  setLayout(mainLayout);

  setWindowTitle(tr("О Программе"));
}


/*!
 * \brief Конструктор класса MainTab.
 */
MainTab::MainTab(QWidget *parent)
  : QWidget(parent)
{
  QLabel *aboutLogo = new QLabel(this);  
  aboutLogo->setPixmap(QPixmap(":/images/logo.png"));
  aboutLogo->setAlignment(Qt::AlignLeft|Qt::AlignTop);
  
  QLabel *qtLabel = new QLabel(this);
  qtLabel->setPixmap(QPixmap(":/images/qt-logo.png"));
  
  QLabel *gplLabel = new QLabel(this);
  gplLabel->setPixmap(QPixmap(":/images/gplv3-88x31.png"));
  
  QLabel *nameLabel = new QLabel(tr("<h2>IMPOMEZIA Simple Chat %1</h2>").arg(SCHAT_VERSION), this);
  nameLabel->setWordWrap(false);
  QLabel *aboutLabel = new QLabel(tr(
      "<p><i>Copyright © 2008 <b>IMPOMEZIA</b>. All rights reserved.</i></div>"
      "<p>Официальный сайт программы: <a href='http://impomezia.com' style='color:#1a4d82;'>http://impomezia.com</a>.</p>"
      "<p>Эта программа использует библиотеку:<br>"
      "<b>Qt Open Source Edition %2<b>"
  ).arg(qVersion()), this);
  
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
  aboutLabel->setWordWrap(true);
  aboutLabel->setAlignment(Qt::AlignLeft|Qt::AlignTop);
  aboutLabel->setSizePolicy(sizePolicy);
  aboutLabel->setOpenExternalLinks(true);
  aboutLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
  
  QHBoxLayout *topLayout = new QHBoxLayout;
  topLayout->addWidget(aboutLogo);
  topLayout->addWidget(aboutLabel);
  
  QHBoxLayout *bottomLayout = new QHBoxLayout;
  bottomLayout->addStretch();
  bottomLayout->addWidget(gplLabel);
  bottomLayout->addWidget(qtLabel);
  
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(nameLabel);
  mainLayout->addLayout(topLayout);
  mainLayout->addLayout(bottomLayout);
}


/*!
 * \brief Конструктор класса MembersTab.
 */
MembersTab::MembersTab(QWidget *parent)
  : QWidget(parent)
{
  QTextBrowser *textBrowser = new QTextBrowser(this);
  textBrowser->setOpenExternalLinks(true);
  textBrowser->setSource(QUrl().fromLocalFile(":/doc/members.html"));

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->addWidget(textBrowser);
  mainLayout->setMargin(0);
}


/*!
 * \brief Конструктор класса ChangeLogTab.
 */
ChangeLogTab::ChangeLogTab(QWidget *parent)
  : QWidget(parent)
{
  QTextBrowser *textBrowser = new QTextBrowser(this);
  textBrowser->setOpenExternalLinks(true);
  
  QString file = qApp->applicationDirPath() + "/doc/ChangeLog.html";
  if (QFile::exists(file)) {
    textBrowser->setSearchPaths(QStringList() << (qApp->applicationDirPath() + "/doc"));
    textBrowser->setSource(QUrl("ChangeLog.html"));
  }
  else
    textBrowser->setText(tr("<h3 style='color:#da251d;'>ОШИБКА</h3>"
                            "<p style='color:#da251d;'>Файл <b>%1</b> не найден!</p>").arg(file));
  
  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->addWidget(textBrowser);
  mainLayout->setMargin(0);
}


/*!
 * \brief Конструктор класса LicenseTab.
 */
LicenseTab::LicenseTab(QWidget *parent)
  : QWidget(parent)
{
  QTextBrowser *textBrowser = new QTextBrowser(this);
  textBrowser->setText(tr(
      "<p><b>Simple Chat %1</b><br />"
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
  ).arg(SCHAT_VERSION));

  textBrowser->setOpenExternalLinks(true);

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->addWidget(textBrowser);
  mainLayout->setMargin(0);
}
