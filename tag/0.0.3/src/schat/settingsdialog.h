/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef SETTINGSDIALOG_H_
#define SETTINGSDIALOG_H_

#include <QDialog>
#include <QPushButton>

class InterfaceSettings;
class NetworkSettings;
class NetworkWidget;
class Profile;
class ProfileSettings;
class ProfileWidget;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QSpinBox;
class QStackedWidget;
class SChatWindow;
class Settings;

#ifdef SCHAT_UPDATE
class UpdateSettings;
#endif


/**
 * Класс `SettingsDialog`
 */
class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  enum {
    ProfilePage,
    NetworkPage,
    InterfacePage,
    UpdatePage
  };
  
  SettingsDialog(Profile *p, Settings *s, QWidget *parent = 0);
  void setPage(int page = 0);

public slots:
  void accept();
  void changePage(QListWidgetItem *current, QListWidgetItem *previous);
  void reset();

private slots:
  inline void validNick(bool b) { m_okButton->setEnabled(b); }
  
private:
  InterfaceSettings *m_interfacePage;
  NetworkSettings *m_networkPage;
  ProfileSettings *m_profilePage;
  QListWidget *m_contentsWidget;
  QPushButton *m_cancelButton;
  QPushButton *m_okButton;
  QPushButton *m_resetButton;
  QStackedWidget *m_pagesWidget;
  
  #ifdef SCHAT_UPDATE
  UpdateSettings *m_updatePage;
  #endif
};


/**
 * Класс `ProfileSettings`
 */
class ProfileSettings : public QWidget
{
  Q_OBJECT
  
public:
  ProfileSettings(Settings *settings, Profile *profile, QWidget *parent = 0);
  void reset();
  void save();
  
signals:
  void validNick(bool b);
  
private:
  ProfileWidget *m_profileWidget;
  Settings *m_settings;
};


/**
 * Класс `NetworkSettings`
 */
class NetworkSettings : public QWidget
{
  Q_OBJECT
  
public:
  NetworkSettings(Settings *settings, QWidget *parent = 0);
  void reset();
  void save();
  
private:
  NetworkWidget *m_networkWidget;
  QCheckBox *m_welcomeCheckBox;
  Settings *m_settings;
};


/**
 * Класс `InterfaceSettings`
 */
class InterfaceSettings : public QWidget
{
  Q_OBJECT
  
public:
  InterfaceSettings(Settings *settings, QWidget *parent = 0);
  void reset();
  void save();
  
private:
  QComboBox *m_styleComboBox;
  Settings *m_settings;
};


/**
 * Класс `UpdateSettings`
 */
#ifdef SCHAT_UPDATE
class UpdateSettings : public QWidget
{
  Q_OBJECT
  
public:
  UpdateSettings(Settings *settings, QWidget *parent = 0);
  void reset();
  void save();
  
private:
  QCheckBox *m_autoClean;
  QCheckBox *m_autoDownload;
  QSpinBox *m_interval;
  Settings *m_settings;
};
#endif

#endif /*SETTINGSDIALOG_H_*/
