/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef WELCOMEDIALOG_H_
#define WELCOMEDIALOG_H_

#include <QDialog>
#include <QPushButton>

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
  WelcomeDialog(Settings *s, Profile *p, QWidget *parent = 0);
  void accept();

private slots:
  inline void validNick(bool b) { okButton->setEnabled(b); }
  void changeIcon(bool s);  
  
private:
  Profile *profile;
  ProfileWidget *profileWidget;
  QCheckBox *askCheckBox;
  QLabel *serverLabel;
  QLineEdit *serverEdit;
  QPushButton *moreButton;
  QPushButton *okButton;
  Settings *settings;
};

#endif /*WELCOMEDIALOG_H_*/
