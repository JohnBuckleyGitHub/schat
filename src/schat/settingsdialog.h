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
class Profile;
class ProfileSettings;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QSpinBox;
class QStackedWidget;
class SChatWindow;
class Settings;
class ProfileWidget;


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
    InterfacePage
  };
  
  SettingsDialog(Profile *p, Settings *s, QWidget *parent = 0);
  void setPage(int page = 0);

public slots:
  void accept();
  void changePage(QListWidgetItem *current, QListWidgetItem *previous);
  void reset();

private slots:
  inline void validNick(bool b) { okButton->setEnabled(b); }
  
private:
  Profile *profile;
  Settings *settings;
  InterfaceSettings *interfaceSettings;
  NetworkSettings *networkPage;
  ProfileSettings *profilePage;
  QListWidget *contentsWidget;
  QPushButton *cancelButton;
  QPushButton *okButton;
  QPushButton *resetButton;
  QStackedWidget *pagesWidget;
  SChatWindow *chat;
};


/**
 * Класс `ProfileSettings`
 */
class ProfileSettings : public QWidget
{
  Q_OBJECT
  
public:
  ProfileSettings(SChatWindow *w, Profile *p, QWidget *parent = 0);
  void reset();
  void save();
  
signals:
  void validNick(bool b);
  
private:
  ProfileWidget *profileWidget;
  SChatWindow *chat;
};


/**
 * Класс `NetworkSettings`
 */
class NetworkSettings : public QWidget
{
  Q_OBJECT
  
public:
  NetworkSettings(SChatWindow *w, Settings *s, QWidget *parent = 0);
  void reset();
  void save();
  
private:
  QCheckBox *welcomeCheckBox;
  QLineEdit *serverEdit;
  QSpinBox *port;
  SChatWindow *chat;
  Settings *settings;
};


/**
 * Класс `InterfaceSettings`
 */
class InterfaceSettings : public QWidget
{
  Q_OBJECT
  
public:
  InterfaceSettings(Settings *s, QWidget *parent = 0);
  void reset();
  void save();
  
private:
  QComboBox *styleComboBox;
  Settings *settings;
};

#endif /*SETTINGSDIALOG_H_*/
