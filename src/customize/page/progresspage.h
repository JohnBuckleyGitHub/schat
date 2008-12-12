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

#ifndef PROGRESSPAGE_H_
#define PROGRESSPAGE_H_

#include <QWizardPage>

class QLabel;
class QProgressBar;
class QTextEdit;
class WizardSettings;

/*!
 * \brief Страница отображающая процесс создания дистрибутива.
 */
class ProgressPage : public QWizardPage
{
  Q_OBJECT

public:
  ProgressPage(QWidget *parent = 0);

private:
  QLabel *m_label;
  QProgressBar *m_progress;
  QProgressBar *packageProgressBar;
  QTextEdit *m_log;
  WizardSettings *m_settings;
};

#endif /* PROGRESSPAGE_H_ */
