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

#include "customizewizard.h"
#include "page/intropage.h"
#include "page/selectpage.h"
#include "wizardsettings.h"
#include "page/progresspage.h"

/**
 * Конструктор InstallWizard
 */
CustomizeWizard::CustomizeWizard(QWidget *parent)
  : QWizard(parent)
{
  setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);

  setOption(QWizard::HaveCustomButton1, true);
  setOption(QWizard::NoBackButtonOnStartPage, true);
  setOption(QWizard::DisabledBackButtonOnLastPage, true);
  setButtonText(QWizard::CustomButton1, tr("О Программе"));

  QList<QWizard::WizardButton> layout;
  layout << QWizard::CustomButton1 << QWizard::Stretch
         << QWizard::BackButton << QWizard::NextButton << QWizard::CommitButton
         << QWizard::FinishButton << QWizard::CancelButton;
  setButtonLayout(layout);

  m_settings = new WizardSettings(QApplication::applicationDirPath() + "/customize.conf", this);
  m_settings->read();

  setPage(Page_Intro,    new IntroPage(this));
  setPage(Page_Select,   new SelectPage(this));
  setPage(Page_Progress, new ProgressPage(this));
  setStartId(Page_Intro);

  connect(this, SIGNAL(accepted()), SLOT(close()));
  connect(this, SIGNAL(rejected()), SLOT(close()));

  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/impomezia48.png"));
  setWindowTitle(tr("%1 Customize").arg(QApplication::applicationName()));
  setWizardStyle(QWizard::ModernStyle);
}
