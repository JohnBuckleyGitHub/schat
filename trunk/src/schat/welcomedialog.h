/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef WELCOMEDIALOG_H_
#define WELCOMEDIALOG_H_

#include <QDialog>
#include <QPushButton>

class NetworkWidget;
class Profile;
class ProfileWidget;
class QCheckBox;
class QLabel;
class QLineEdit;
class SChatWindow;
class Settings;

class WelcomeDialog : public QDialog
{
  Q_OBJECT

public:
  WelcomeDialog(Settings *settings, Profile *profile, QWidget *parent = 0);
  void accept();

private slots:
  inline void validNick(bool b) { m_okButton->setEnabled(b); }
  void changeIcon(bool s);  
  
private:
  NetworkWidget *m_networkWidget;
  ProfileWidget *m_profileWidget;
  QCheckBox *m_askCheckBox;
  QLabel *m_serverLabel;
  QLineEdit *m_serverEdit;
  QPushButton *m_moreButton;
  QPushButton *m_okButton;
  Settings *m_settings;
};

#endif /*WELCOMEDIALOG_H_*/
