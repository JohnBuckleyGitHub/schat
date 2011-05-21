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

#ifndef NSISPAGE_H_
#define NSISPAGE_H_

#include <QWizardPage>

class QCheckBox;
class QLineEdit;
class QPushButton;
class WizardSettings;

/*!
 * \brief Страница переопределения опций инсталлятора.
 */
class NsisPage : public QWizardPage
{
  Q_OBJECT

public:
  NsisPage(QWidget *parent = 0);
  bool validatePage();

private slots:
  void getOpenFileName();

private:
  QCheckBox *m_allPrograms;
  QCheckBox *m_autostart;
  QCheckBox *m_autostartDaemon;
  QCheckBox *m_desktop;
  QCheckBox *m_quickLaunch;
  QLineEdit *m_nsis;
  QPushButton *m_button;
  WizardSettings *m_settings;
};

#endif /* NSISPAGE_H_ */
