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

#include <QtGui>

#include "customizewizard.h"
#include "page/intropage.h"
#include "page/nsispage.h"
#include "page/progresspage.h"
#include "page/selectpage.h"
#include "page/settingspage.h"
#include "translation.h"
#include "wizardsettings.h"

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

  QList<QWizard::WizardButton> layout;
  layout << QWizard::CustomButton1 << QWizard::Stretch
         << QWizard::BackButton << QWizard::NextButton << QWizard::CommitButton
         << QWizard::FinishButton << QWizard::CancelButton;
  setButtonLayout(layout);

  m_settings = new WizardSettings(QApplication::applicationDirPath() + "/customize.conf", this);
  m_settings->read();
  m_dist = m_settings->dist();

  m_translation = new Translation(this);
  m_translation->setPrefix("customize_");
  m_translation->setSearch(QApplication::applicationDirPath() + "/translations");
  m_translation->load(m_settings->getString("Translation"));

  setButtonText(QWizard::CustomButton1, tr("About"));

  setPage(Page_Intro,    new IntroPage(this));
  setPage(Page_Select,   new SelectPage(this));

  if (!m_dist)
    setPage(Page_Settings, new SettingsPage(this));

  setPage(Page_Nsis,     new NsisPage(this));
  setPage(Page_Progress, new ProgressPage(this));
  setStartId(Page_Intro);

  connect(this, SIGNAL(accepted()), SLOT(close()));
  connect(this, SIGNAL(rejected()), SLOT(close()));

  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/impomezia48.png"));
  setWindowTitle(QApplication::applicationName() + " Customize");
  setWizardStyle(QWizard::ModernStyle);

  connect(this, SIGNAL(customButtonClicked(int)), SLOT(customButtonClicked(int)));
}


void CustomizeWizard::customButtonClicked(int which)
{
  if (which == CustomButton1) {
    if (!m_about) {
      m_about = new AboutDialog(this);
      m_about->show();
    }

    m_about->activateWindow();
  }
}
