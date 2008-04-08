/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef WELCOMEDIALOG_H_
#define WELCOMEDIALOG_H_

#include <QDialog>

#include "profile.h"
#include "ui_welcomedialog.h"

class SChatWindow;

class WelcomeDialog : public QDialog, public Ui::WelcomeDialog
{
  Q_OBJECT

public:
  WelcomeDialog(Profile *p, QWidget *parent = 0);
  bool hideWelcome() { return askCheckBox->isChecked(); }
  QString server() { return serverEdit->text(); }
  void accept();  
  void setHideWelcome(bool w) { askCheckBox->setChecked(w); }
  void setServer(const QString &s) { serverEdit->setText(s); }

private slots:
  void changeIcon(bool s);
  
private:
  Profile *profile;
};

#endif /*WELCOMEDIALOG_H_*/
