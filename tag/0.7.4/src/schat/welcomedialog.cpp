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

#include <QtGui>

#include "profilewidget.h"
#include "schatwindow.h"
#include "settings.h"
#include "welcomedialog.h"
#include "widget/networkwidget.h"

/*!
 * Конструктор класса WelcomeDialog.
 */
WelcomeDialog::WelcomeDialog(QWidget *parent)
  : QDialog(parent),
  m_settings(SimpleSettings)
{
  setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

  m_profileWidget = new ProfileWidget(false, this);
  m_askCheckBox = new QCheckBox(tr("Больше не спрашивать"), this);
  m_okButton    = new QPushButton(QIcon(":/images/dialog-ok.png"), tr("ОК"), this);
  #ifndef Q_OS_WINCE
  m_moreButton  = new QPushButton(QIcon(":/images/arrow-down.png"), "", this);
  m_moreButton->setCheckable(true);
  connect(m_moreButton, SIGNAL(toggled(bool)), this, SLOT(changeIcon(bool)));
  #endif
  m_networkWidget = new NetworkWidget(this, NetworkWidget::NetworkLabel);

  QFrame *line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(m_profileWidget);

  QHBoxLayout *buttonsLayout = new QHBoxLayout;
  #ifndef Q_OS_WINCE
  buttonsLayout->addWidget(m_askCheckBox);
  #endif
  buttonsLayout->addStretch();
  buttonsLayout->addWidget(m_okButton);
  #ifndef Q_OS_WINCE
  buttonsLayout->addWidget(m_moreButton);
  #endif
  buttonsLayout->setSpacing(3);

  #ifndef Q_OS_WINCE
  mainLayout->addWidget(line);
  mainLayout->addLayout(buttonsLayout);
  mainLayout->addWidget(m_networkWidget);
  #else
  mainLayout->addSpacing(m_networkWidget->sizeHint().height());
  mainLayout->addWidget(m_networkWidget);
  mainLayout->addWidget(line);
  mainLayout->addWidget(m_askCheckBox);
  mainLayout->addLayout(buttonsLayout);
  mainLayout->addStretch();
  #endif
  mainLayout->setMargin(6);
  mainLayout->setSpacing(6);

  #ifdef Q_OS_WINCE
  setWindowState(Qt::WindowMaximized);
  #else
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  m_networkWidget->setVisible(false);
  #endif

  if (m_settings->getBool("FirstRun"))
    m_askCheckBox->setChecked(true);

  connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(m_profileWidget, SIGNAL(validNick(bool)), this, SLOT(validNick(bool)));

  setWindowTitle(tr("Выбор ника"));
}


void WelcomeDialog::accept()
{
  m_profileWidget->save();
  m_networkWidget->save();
  m_settings->setBool("HideWelcome", m_askCheckBox->isChecked());

  QDialog::accept();
}


#ifndef Q_OS_WINCE
void WelcomeDialog::changeIcon(bool s)
{
  if (s) {
    m_moreButton->setIcon(QIcon(":/images/arrow-up.png"));
    m_networkWidget->setVisible(true);
  }
  else {
    m_moreButton->setIcon(QIcon(":/images/arrow-down.png"));
    m_networkWidget->setVisible(false);
  }
}
#endif
