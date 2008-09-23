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

#ifndef DAEMONSETTINGSDIALOG_H_
#define DAEMONSETTINGSDIALOG_H_

#include <QObject>

#include "abstractsettingsdialog.h"

class DaemonCommonSettings;
class DaemonUiSettings;
class QCheckBox;
class QComboBox;
class QSpinBox;

/*!
 * \class DaemonSettingsDialog
 * \brief Настройка сервера
 */
class DaemonSettingsDialog : public AbstractSettingsDialog
{
  Q_OBJECT

public:
  enum Page {
    CommonPage
  };

  DaemonSettingsDialog(DaemonUiSettings *settings, QWidget *parent = 0);
  ~DaemonSettingsDialog();

private:
  DaemonCommonSettings *m_commonPage;
  DaemonUiSettings *m_settings;
};


/*!
 * \brief Диалог "Настройка", страница "Интерфейс".
 */
class DaemonCommonSettings : public AbstractSettingsPage
{
  Q_OBJECT

public:
  DaemonCommonSettings(DaemonUiSettings *settings, QWidget *parent = 0);

public slots:
  void reset(int page);
  void save();

private:
  DaemonUiSettings *m_settings;
  QCheckBox *m_mainChannelLog;
  QComboBox *m_listenCombo;
  QSpinBox *m_logSpin;
  QSpinBox *m_portSpin;
};

#endif /*DAEMONSETTINGSDIALOG_H_*/
