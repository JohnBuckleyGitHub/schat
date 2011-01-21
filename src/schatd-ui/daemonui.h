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

#ifndef DAEMONUI_H_
#define DAEMONUI_H_

#include <QDialog>
#include <QPointer>
#include <QSystemTrayIcon>

#include "daemonsettingsdialog.h"
#include "ipc/localclientservice.h"

class DaemonSettings;
class LocalClientService;
class QAction;
class QLabel;
class QMenu;
class QProcess;
class QToolButton;
class Translation;

#ifndef SCHATD_NO_SERVICE
class QtServiceController;
#endif

/*!
 * \brief Диалог управления сервером
 */
class DaemonUi : public QDialog
{
  Q_OBJECT

public:
  DaemonUi(QWidget *parent = 0);
  ~DaemonUi();

public slots:
  void handleMessage(const QString& message);

private slots:
  void checkStart();
  void exit();
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void init();
  void notify(LocalClientService::Reason reason);
  void restart();
  void settings();
  void start();
  void stop();

private:
  enum Status {
    Unknown,
    Error,
    Starting,
    Started,
    Stopped,
    Restarting
  };

  enum LedColor {
    Red,
    Yellow,
    Green
  };

  bool arguments(const QStringList &args);
  void createActions();
  void createButtons();
  void createTray();
  void setActionsState(bool start = true, bool stop = true, bool restart = true, bool quit = true, bool settings = true);
  void setLedColor(LedColor color = Red);
  void setStatus(Status status);
  void showUi();

  DaemonSettings *m_settings;
  LocalClientService *m_client;
  QAction *m_quitAction;
  QAction *m_quitAllAction;
  QAction *m_restartAction;
  QAction *m_settingsAction;
  QAction *m_startAction;
  QAction *m_stopAction;
  QLabel *m_aboutLabel;
  QLabel *m_ledLabel;
  QLabel *m_statusLabel;
  QMenu *m_menu;
  QPointer<DaemonSettingsDialog> m_settingsDialog;
  QPushButton *m_hideButton;
  QPushButton *m_quitButton;
  QString m_appDir;
  QString m_daemonFile;
  QSystemTrayIcon *m_tray;
  QTimer m_checkTimer;
  QToolButton *m_restartButton;
  QToolButton *m_settingsButton;
  QToolButton *m_startButton;
  QToolButton *m_stopButton;
  Status m_status;
  Translation *m_translation;
  #ifndef SCHATD_NO_SERVICE
  QtServiceController *m_controller;
  #endif
};

#endif /*DAEMONUI_H_*/
