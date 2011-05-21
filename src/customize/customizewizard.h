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

#ifndef CUSTOMIZEWIZARD_H_
#define CUSTOMIZEWIZARD_H_

#include <QObject>
#include <QPointer>
#include <QWizard>

#include "aboutdialog.h"

class Translation;
class WizardSettings;

class CustomizeWizard : public QWizard
{
  Q_OBJECT

public:
  enum Pages {
    Page_Intro,
    Page_Select,
    Page_Settings,
    Page_Nsis,
    Page_Progress,
  };

  CustomizeWizard(QWidget *parent = 0);

private slots:
  void customButtonClicked(int which);

private:
  bool m_dist;
  QPointer<AboutDialog> m_about;
  Translation *m_translation;
  WizardSettings *m_settings;
};

#endif /* CUSTOMIZEWIZARD_H_ */
